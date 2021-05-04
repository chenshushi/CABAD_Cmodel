#define NUMB_FRA		2
#define SIZE_FRA_Y		416
#define SIZE_FRA_X		240
#define SIZE_GOP		1
#define DATA_QP			14
#define BITSTREAM_I		"D://Decoder//CABAD//bs_check.dat"
#define DUMP_I			"D://Decoder//CABAD//output.dat"
#define CABAC_CONTEXT_MODELS_COUNT 460
#define Clip3(x, y, z) (((z) < (x)) ? (x) : (((z) > (y)) ? (y) : (z)))
typedef int                int32_t;
typedef signed char        int8_t;
typedef unsigned long long uint64_t;
typedef unsigned char      uint8_t;
typedef unsigned int       uint32_t;
typedef unsigned short     uint16_t;
using CoefType = int32_t;