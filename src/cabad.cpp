#include "cabad.h"
unsigned int CABAD::CabadDecordBypass(f264Cabad_t* cb)
{
	cnt_bypass++;
	int s1_offset = (cb->offset << 1) + ReadBitstream(1);
	int s2_offset = s1_offset - cb->range;

	cb->offset = (s2_offset >= 0) ? s2_offset : s1_offset;

	//if(DEBUG_ED) printf("decode bypass: bin=%d\n",s2_offset>=0);
	if (DEBUG_ED) printf("decode bypass: bin=%d, range=%d, offset=%d\n", s2_offset >= 0, cb->range, cb->offset);
	return s2_offset >= 0;
}
unsigned int CABAD::CabadDecordTerminate(f264Cabad_t* cb)
{
	cnt_term++;
	uint32_t bin;
	cb->range = cb->range - 2;
	if (cb->offset >= cb->range)//binval=1
	{
		bin = 1;
	}
	else//binval=0
	{
		if (cb->range < 256)
		{
			cb->range = cb->range << 1;
			cb->offset = (cb->offset << 1) + ReadBitstream(1);
		}
		bin = 0;
	}

	//printf("CTU[%d][%d]: range=%d\n",m_ctuY,m_ctuX,cb->range);
	//if(bin==1) printf("CTU[%d][%d]: range=%d,offset=%d\n",m_ctuY,m_ctuX,cb->range,cb->offset);
	//if (cb->range == 439 && cb->offset == 173)
	//	printf("break\n");
	if (DEBUG_ED) printf("CTU[%d][%d]: range=%d,offset=%d\n", mb_y, mb_x, cb->range, cb->offset);
	//if(DEBUG_ED) printf("decode terminate: bin=%d, range=%d, offset=%d\n",bin,cb->range,cb->offset);
	return bin;
}
unsigned int  CABAD::CabadDecordRegular(f264Cabad_t* cb, uint8_t* state) {

	cnt_regular++;

	//printf("state=%d\n",*state);
	uint32_t bin;
	int      shiftLPS = s_f265CabacNewRenormTable[(*state) >> 1][(cb->range >> 6) & 3];
	uint32_t rangeLPS = s_f265CabacRangeLPS[(*state) >> 1][(cb->range >> 6) & 3];

	//printf("state_i=%d, mps_i=%d, range_i=%d, offset_i=%d\n",(*state) >> 1,(*state) & 0x01,cb->range,cb->offset);

	int mps = (*state) & 0x01;
	int offsetlps = cb->offset - (cb->range - rangeLPS);

	bin = (offsetlps >= 0) ? (1 - mps) : mps;
	if (offsetlps >= 0)//LPS
	{
		(*state) = s_f265CabacNextStateLPS[*state];
		cb->offset = offsetlps;
		cb->range = rangeLPS;
		cb->range = cb->range << shiftLPS;
		cb->offset = (cb->offset << shiftLPS) + ReadBitstream(shiftLPS);
	}
	else//MPS
	{
		(*state) = s_f265CabacNextStateMPS[*state];
		cb->range = (cb->range - rangeLPS);
		if (cb->range < 256)
		{
			cb->range = cb->range << 1;
			cb->offset = (cb->offset << 1) + ReadBitstream(1);
		}
	}

	//printf("bin_o=%d, state_o=%d, mps_o= %d, range_o=%d, offset_o=%d\n",bin,(*state)>>1,(*state) & 0x01, cb->range,cb->offset);
	//if(DEBUG_ED) printf("decode regular: bin=%d, state=%d, range=%d\n",bin,*state,cb->range);
	if (DEBUG_ED) printf("decode regular: bin=%d, state=%d, range=%d, offset=%d\n", bin, *state, cb->range, cb->offset);
	//if (bin == 0 && *state == 30 && cb->range == 478 && cb->offset == 30)
	//	printf("break\n");
	return bin;
}
 uint16_t CABAD::getCtxIdxForMbType(SliceType type, const CordTermFlag2D<1>& flag, uint8_t binIdx)
{
	switch (type) {
	case SLICE_TYPE_I:
		if (binIdx == 0)
			return 3 + flag.a + flag.b;
		else if (binIdx == 1)
			return 276;
		else
			return 3 + binIdx + 1;

	default:
		CABACCheck(false);
	}

	return 0xFFFF;

}
 void CABAD::resetMbBuffer(EncodedMb* mbEnc) {
 
 
 }
void CABAD::readMbTypeInfos(int colourID, f264Cabad_t* cb) {
	uint8_t     binVal;
	uint16_t    ctxIdx;

	uint8_t     cbpChromaDCFlag;
	uint8_t     cbpChromaACFlag = 0;

	ctxIdx = getCtxIdxForMbType(type, refFlags[COLOUR_COMPONENT_Y].nonIntraNxN, 0);

	binVal = CabadDecordRegular(cb, &ctxModels[ctxIdx].pStateIdx);

	if (binVal == 0) {
		mb[colourID].mbPart = INTRA_PARTITION_4x4;
	}
	else {
		mb[colourID].mbPart = INTRA_PARTITION_16x16;

		mb[colourID].transformSize8x8Flag = 0;

		binVal = CabadDecordTerminate(cb);
		//cbpLuma AC
		ctxIdx = getCtxIdxForMbType(type, refFlags[COLOUR_COMPONENT_Y].nonIntraNxN, 2);
		cbpLumaACFlag = CabadDecordRegular(cb, &ctxModels[ctxIdx].pStateIdx);
		cbpLuma[colourID] = cbpLumaACFlag ? 0xF : 0x0;
		//cbpChroma
		ctxIdx = getCtxIdxForMbType(type, refFlags[COLOUR_COMPONENT_Y].nonIntraNxN, 3);
		cbpChromaDCFlag = CabadDecordRegular(cb, &ctxModels[ctxIdx].pStateIdx);

		if (cbpChromaDCFlag != 0) {
			ctxIdx++;
			cbpChromaACFlag = CabadDecordRegular(cb, &ctxModels[ctxIdx].pStateIdx);
		}
		cbpChroma = cbpChromaDCFlag << 1 | cbpChromaACFlag;
		//16x16PreMode
		ctxIdx = getCtxIdxForMbType(type, refFlags[COLOUR_COMPONENT_Y].nonIntraNxN, 5);
		binVal = CabadDecordRegular(cb, &ctxModels[ctxIdx].pStateIdx);

		ctxIdx++;
		mb[colourID].intra16x16PredMode = (binVal << 1) + CabadDecordRegular(cb, &ctxModels[ctxIdx].pStateIdx);

	}

}
void CABAD::DecodeSlicedata(int colourID,f264Cabad_t *cb) {
	readMbTypeInfos(colourID,  cb);

	//if ((mb[compID].mbPart != INTRA_PARTITION_16x16) && transform8x8ModeFlag)
	//	readTransformSize8x8FlagInfos(compID);

	//if (mb[compID].mbPart != INTRA_PARTITION_16x16)
	//	readIntraNxNPredModeInfos(compID);

	//if (chromaFormat == CHROMA_FORMAT_420 || chromaFormat == CHROMA_FORMAT_422)
	//	readIntraChromaPredMode();

	//if (mb[compID].mbPart != INTRA_PARTITION_16x16)
	//	readCodedBlockPatternInfos(compID);

	//if (cbpLuma[compID] ||
	//	((chromaFormat == CHROMA_FORMAT_420 || chromaFormat == CHROMA_FORMAT_422) && cbpChroma) ||
	//	mb[compID].mbPart == INTRA_PARTITION_16x16) {
	//	readMbQPDeltaInfos(compID);
	//	readCoefs(compID);
	//}
	//else
	//{
	//	mbQPDelta = 0;
	//	uint32_t QPOffsetY = 6 * (bitDepthY - 8);
	//	if (curMb == firstMbInSlice)
	//		mb[compID].qp = ((sliceQP[compID] + mbQPDelta + 52 + 2 * QPOffsetY) % (52 + QPOffsetY)) - QPOffsetY;
	//	else
	//		mb[compID].qp = ((lastMbQP[compID] + mbQPDelta + 52 + 2 * QPOffsetY) % (52 + QPOffsetY)) - QPOffsetY;

	//	lastMbQP[compID] = mb[compID].qp;
	//}


}


void CABAD::f264cabacDecoder(f264Cabad_t* cb) {
	for (int i= 0; i < 3; i++) {
		DecodeSlicedata(i, cb);
	}

}
int CABAD::getNeighbourState(int mb_num) {
	int nb = 0;

	if (mb_x != 0 && mb_num != firstMbInSlice)
		nb |= NEIGHBOUR_AVAILABLE_LEFT;

	if (mb_x != 0 && mb_num > firstMbInSlice + widthInMbs)
		nb |= NEIGHBOUR_AVAILABLE_UP_LEFT;

	if (mb_num >= firstMbInSlice + widthInMbs)
		nb |= NEIGHBOUR_AVAILABLE_UP;

	if (mb_x != widthInMbs - 1 && mb_num >= firstMbInSlice + widthInMbs - 1)
		nb |= NEIGHBOUR_AVAILABLE_UP_RIGHT;

	return nb;
}
void CABAD::getCABACRefFlags()
{
	neighbour = getNeighbourState(mb_x * widthInMbs + mb_y);
	for (int compID = 0; compID < 3 ; ++compID) {
		refFlags[compID].mbQPDelta = (mb_x == 0 && mb_y == 0) ? prevFlags[compID][0].mbQPDelta : 0;

		refFlags[compID].nonIntraNxN.a = (neighbour & NEIGHBOUR_AVAILABLE_LEFT) ?
			leftFlags[compID][0].nonIntraNxN : 0;
		refFlags[compID].nonIntraNxN.b = (neighbour & NEIGHBOUR_AVAILABLE_UP) ?
			upFlags[compID][mb_x].nonIntraNxN : 0;

		refFlags[compID].intraChromaPredMode.a = (neighbour & NEIGHBOUR_AVAILABLE_LEFT) ?
			leftFlags[compID][0].intraChromaPredMode : 0;
		refFlags[compID].intraChromaPredMode.b = (neighbour & NEIGHBOUR_AVAILABLE_UP) ?
			upFlags[compID][mb_x].intraChromaPredMode : 0;

		refFlags[compID].transform8x8.a = (neighbour & NEIGHBOUR_AVAILABLE_LEFT) ?
			leftFlags[compID][0].transform8x8 : 0;
		refFlags[compID].transform8x8.b = (neighbour & NEIGHBOUR_AVAILABLE_UP) ?
			upFlags[compID][mb_x].transform8x8 : 0;

		refFlags[compID].cbpLuma.a[0] = (neighbour & NEIGHBOUR_AVAILABLE_LEFT) ?
			leftFlags[compID][0].cbpLuma[0] : 1;
		refFlags[compID].cbpLuma.a[1] = (neighbour & NEIGHBOUR_AVAILABLE_LEFT) ?
			leftFlags[compID][0].cbpLuma[1] : 1;
		refFlags[compID].cbpLuma.b[0] = (neighbour & NEIGHBOUR_AVAILABLE_UP) ?
			upFlags[compID][mb_x].cbpLuma[0] : 1;
		refFlags[compID].cbpLuma.b[1] = (neighbour & NEIGHBOUR_AVAILABLE_UP) ?
			upFlags[compID][mb_x].cbpLuma[1] : 1;

		refFlags[compID].cbpChroma.a = (neighbour & NEIGHBOUR_AVAILABLE_LEFT) ?
			leftFlags[compID][0].cbpChromaDC << 1 | leftFlags[compID][0].cbpChromaAC << 0 : 0;
		refFlags[compID].cbpChroma.b = (neighbour & NEIGHBOUR_AVAILABLE_UP) ?
			upFlags[compID][mb_x].cbpChromaDC << 1 | upFlags[compID][mb_x].cbpChromaAC << 0 : 0;
		
	}

	for (int planeID = 0; planeID < COLOUR_COMPONENT_COUNT; ++planeID) {
		// Assume that currMbAddr uses Intra Prediction if mbAddrN does not exist

		for (uint8_t yInSbs = 0; yInSbs < 4; ++yInSbs)
			refFlags[planeID].nonZero.a[yInSbs] = (neighbour & NEIGHBOUR_AVAILABLE_LEFT) ?
			leftCBF[planeID][0].nonZero[yInSbs] : 1;
		for (uint8_t xInSbs = 0; xInSbs < 4; ++xInSbs)
			refFlags[planeID].nonZero.b[xInSbs] = (neighbour & NEIGHBOUR_AVAILABLE_UP) ?
			upCBF[planeID][mb_x].nonZero[xInSbs] : 1;

		refFlags[planeID].nonZeroDC.a = (neighbour & NEIGHBOUR_AVAILABLE_LEFT) ?
			leftCBF[planeID][0].nonZeroDC : 1;
		refFlags[planeID].nonZeroDC.b = (neighbour & NEIGHBOUR_AVAILABLE_UP) ?
			upCBF[planeID][mb_x].nonZeroDC : 1;
	}
	}



void CABAD::init() {
	firstMbInSlice = 0;
	DEBUG_ED = 0;
	frame_width = SIZE_FRA_X;
	frame_height = SIZE_FRA_Y;
	m_qp = DATA_QP;
	cnt_regular = 0;
	cnt_bypass = 0;
	cnt_term = 0;
	widthInMbs = SIZE_FRA_Y / 16;
	heightInMbs = SIZE_FRA_X / 16;
	m_fpCabadInput_I = fopen(BITSTREAM_I, "r");
	m_fpCabadOutput_I = fopen(DUMP_I, "w");
}
void CABAD::run() {
	if (mb_x == 0 && mb_y == 0) {
		bitcnt = 0;
		bytecnt = 0;
		if (m_frame % SIZE_GOP == 0)
			type = SLICE_TYPE_I;
		else
			type = SLICE_TYPE_P;
		CabadInit(&m_cb);
		f264CabacContextInit(m_qp, type);
	}

	f264cabacDecoder(&m_cb);


}
void CABAD::dump() {}
void CABAD::update(ed_t& edout) {}
void CABAD::CabadInit(f264Cabad_t* cb) {
	cb->offset = ReadBitstream(9);;
	cb->range = 510;
	if (DEBUG_ED) printf("CabadInit: range=%d, offset=%d\n", cb->range, cb->offset);
}
unsigned int CABAD::ReadBitstream(unsigned int length) {
	unsigned int ret = 0;
	if (bitcnt < length)
	{
		ret = data >> (8 - bitcnt);
		length = length - bitcnt;
		if (type == SLICE_TYPE_I)
			fscanf(m_fpCabadInput_I, "%x", &data);
		else
			fscanf(m_fpCabadInput_P, "%x", &data);
		bytecnt++;
		bitcnt = 8;
		ret = (ret << length) + ReadBitstream(length);
	}
	else
	{
		ret = data >> (8 - length);
		data = (data << length) & 0xff;
		bitcnt = bitcnt - length;
	}
	return ret;
}
void CABAD::f264CabacContextInit(int qp, SliceType type) {
	if (type == SLICE_TYPE_I) {
		for (int i = 0; i < CABAC_CONTEXT_MODELS_COUNT; ++i) {
			int preCtxState;
			preCtxState = Clip3(1, 126, ((f264_cabac_context_init_I[i][0] * Clip3((int8_t)0, (int8_t)51, qp)) >> 4) + f264_cabac_context_init_I[i][1]);
			if (preCtxState <= 63) {
				ctxModels[i].pStateIdx = (uint8_t)(63 - preCtxState);
				ctxModels[i].valMPS = 0;
			}
			else {
				ctxModels[i].pStateIdx = (uint8_t)(preCtxState - 64);
				ctxModels[i].valMPS = 1;
			}
		}
		ctxModels[276].pStateIdx = 63;
		ctxModels[276].valMPS = 0;
	}
}
void CABAD::cabad_proc(ed_t& edput) {
	if (mb_x == 0 && mb_y == 0 && m_frame == 0)
		init();
	run();
	dump();
	update(edput);
}