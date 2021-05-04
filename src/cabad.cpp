#include "cabad.h"
void CABAD::cabad_proc(ed_t& edput) {
	if (mb_x == 0 && mb_y == 0 && m_frame == 0)
		init();
	run();
	dump();
	update(edput);
}
void CABAD::init() {
	DEBUG_ED = 0;
	frame_width = SIZE_FRA_X;
	frame_height = SIZE_FRA_Y;
	m_qp = DATA_QP;
	cnt_regular = 0;
	cnt_bypass = 0;
	cnt_term = 0;
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
	}
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
	
}