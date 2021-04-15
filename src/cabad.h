#include "define.h"
#include "stdio.h"

enum SliceType {
	SLICE_TYPE_P,
	SLICE_TYPE_I
};
struct f264Cabad_t
{
	int offset;
	int range;
};
struct ed_t {};

class CABAD {
private:
	int DEBUG_ED; // for three decoder engine debug

	void init();
	void run();
	void dump();
	void update(ed_t& edout);

	unsigned int ReadBitstream(unsigned int length);


	void CabadInit(f264Cabad_t* cb);
public:
	int bitcnt = 0;
	int bytecnt = 0;

	int bytecnt = 0;
	int m_frame;
	int mb_x;
	int mb_y;
	int m_qp;
	SliceType type;


	unsigned int frame_width, frame_height;
	uint64_t cnt_regular;
	uint64_t cnt_bypass;
	uint64_t cnt_term;

	FILE* m_fpCabadInput_I;
	FILE* m_fpCabadOutput_I;

	void cabad_proc(ed_t& edput);
};
