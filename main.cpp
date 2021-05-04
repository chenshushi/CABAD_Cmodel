#include "cabad.h"

int main(void) {
	CABAD	cabad;
	ed_t	edout;

	for (int frame = 0; frame < NUMB_FRA; frame++) {
		cabad.m_frame = frame;
		for (int i = 0; i < ((SIZE_FRA_Y + 15) / 16); i++) {
			for (int j = 0; j < ((SIZE_FRA_X + 15) / 16); j++) {
				cabad.mb_x = j;
				cabad.mb_y = i;
				cabad.cabad_proc(edout);
			}
			/* printf("bytecnt=%x\n", cabad.bytecnt); */
		}
	}
	fclose(cabad.m_fpCabadInput_I);
	fclose(cabad.m_fpCabadOutput_I);
}