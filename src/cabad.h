#include "define.h"
#include "stdio.h"
#include <cassert>

#define CABACCheck(expr) assert(expr)

enum SliceType {
	SLICE_TYPE_P,
	SLICE_TYPE_I
};
enum MacroblockPartition {
    INTRA_PARTITION_4x4,
    INTRA_PARTITION_8x8,
    INTRA_PARTITION_16x16
};
enum NeighbourState {
    NEIGHBOUR_AVAILABLE_LEFT = (1 << 0),
    NEIGHBOUR_AVAILABLE_UP_LEFT = (1 << 1),
    NEIGHBOUR_AVAILABLE_UP = (1 << 2),
    NEIGHBOUR_AVAILABLE_UP_RIGHT = (1 << 3),
    //    NEIGHBOUR_AVAILABLE_RIGHT    = (1 << 4)
};
enum ColourComponent {
    COLOUR_COMPONENT_Y = 0,
    COLOUR_COMPONENT_CB = 1,
    COLOUR_COMPONENT_CR = 2,
    COLOUR_COMPONENT_COUNT
};
static const uint8_t s_f265CabacNewRenormTable[64][4] = {
    { 1, 1, 1, 1 },
    { 1, 1, 1, 1 },
    { 1, 1, 1, 1 },
    { 2, 1, 1, 1 },
    { 2, 1, 1, 1 },
    { 2, 1, 1, 1 },
    { 2, 1, 1, 1 },
    { 2, 2, 1, 1 },
    { 2, 2, 1, 1 },
    { 2, 2, 1, 1 },
    { 2, 2, 2, 1 },
    { 2, 2, 2, 1 },
    { 2, 2, 2, 1 },
    { 2, 2, 2, 2 },
    { 2, 2, 2, 2 },
    { 2, 2, 2, 2 },
    { 3, 2, 2, 2 },
    { 3, 2, 2, 2 },
    { 3, 2, 2, 2 },
    { 3, 2, 2, 2 },
    { 3, 3, 2, 2 },
    { 3, 3, 2, 2 },
    { 3, 3, 2, 2 },
    { 3, 3, 3, 2 },
    { 3, 3, 3, 2 },
    { 3, 3, 3, 2 },
    { 3, 3, 3, 3 },
    { 3, 3, 3, 3 },
    { 3, 3, 3, 3 },
    { 3, 3, 3, 3 },
    { 4, 3, 3, 3 },
    { 4, 3, 3, 3 },
    { 4, 3, 3, 3 },
    { 4, 4, 3, 3 },
    { 4, 4, 3, 3 },
    { 4, 4, 3, 3 },
    { 4, 4, 3, 3 },
    { 4, 4, 4, 3 },
    { 4, 4, 4, 3 },
    { 4, 4, 4, 4 },
    { 4, 4, 4, 4 },
    { 4, 4, 4, 4 },
    { 4, 4, 4, 4 },
    { 5, 4, 4, 4 },
    { 5, 4, 4, 4 },
    { 5, 4, 4, 4 },
    { 5, 4, 4, 4 },
    { 5, 5, 4, 4 },
    { 5, 5, 4, 4 },
    { 5, 5, 4, 4 },
    { 5, 5, 5, 4 },
    { 5, 5, 5, 4 },
    { 5, 5, 5, 4 },
    { 5, 5, 5, 5 },
    { 5, 5, 5, 5 },
    { 5, 5, 5, 5 },
    { 5, 5, 5, 5 },
    { 6, 5, 5, 5 },
    { 6, 5, 5, 5 },
    { 6, 5, 5, 5 },
    { 6, 5, 5, 5 },
    { 6, 6, 5, 5 },
    { 6, 6, 5, 5 },
    { 6, 6, 6, 6 }
};
static const uint8_t s_f265CabacRangeLPS[64][4] = {
    { 128, 176, 208, 240 },
    { 128, 167, 197, 227 },
    { 128, 158, 187, 216 },
    { 123, 150, 178, 205 },
    { 116, 142, 169, 195 },
    { 111, 135, 160, 185 },
    { 105, 128, 152, 175 },
    { 100, 122, 144, 166 },
    { 95, 116, 137, 158 },
    { 90, 110, 130, 150 },
    { 85, 104, 123, 142 },
    { 81, 99, 117, 135 },
    { 77, 94, 111, 128 },
    { 73, 89, 105, 122 },
    { 69, 85, 100, 116 },
    { 66, 80, 95, 110 },
    { 62, 76, 90, 104 },
    { 59, 72, 86, 99 },
    { 56, 69, 81, 94 },
    { 53, 65, 77, 89 },
    { 51, 62, 73, 85 },
    { 48, 59, 69, 80 },
    { 46, 56, 66, 76 },
    { 43, 53, 63, 72 },
    { 41, 50, 59, 69 },
    { 39, 48, 56, 65 },
    { 37, 45, 54, 62 },
    { 35, 43, 51, 59 },
    { 33, 41, 48, 56 },
    { 32, 39, 46, 53 },
    { 30, 37, 43, 50 },
    { 29, 35, 41, 48 },
    { 27, 33, 39, 45 },
    { 26, 31, 37, 43 },
    { 24, 30, 35, 41 },
    { 23, 28, 33, 39 },
    { 22, 27, 32, 37 },
    { 21, 26, 30, 35 },
    { 20, 24, 29, 33 },
    { 19, 23, 27, 31 },
    { 18, 22, 26, 30 },
    { 17, 21, 25, 28 },
    { 16, 20, 23, 27 },
    { 15, 19, 22, 25 },
    { 14, 18, 21, 24 },
    { 14, 17, 20, 23 },
    { 13, 16, 19, 22 },
    { 12, 15, 18, 21 },
    { 12, 14, 17, 20 },
    { 11, 14, 16, 19 },
    { 11, 13, 15, 18 },
    { 10, 12, 15, 17 },
    { 10, 12, 14, 16 },
    { 9, 11, 13, 15 },
    { 9, 11, 12, 14 },
    { 8, 10, 12, 14 },
    { 8, 9, 11, 13 },
    { 7, 9, 11, 12 },
    { 7, 9, 10, 12 },
    { 7, 8, 10, 11 },
    { 6, 8, 9, 11 },
    { 6, 7, 9, 10 },
    { 6, 7, 8, 9 },
    { 2, 2, 2, 2 }
};
static const uint8_t s_f265CabacNextStateLPS[128] = {
    1, 0, 0, 1, 2, 3, 4, 5, 4, 5, 8, 9, 8, 9, 10, 11,
    12, 13, 14, 15, 16, 17, 18, 19, 18, 19, 22, 23, 22, 23, 24, 25,
    26, 27, 26, 27, 30, 31, 30, 31, 32, 33, 32, 33, 36, 37, 36, 37,
    38, 39, 38, 39, 42, 43, 42, 43, 44, 45, 44, 45, 46, 47, 48, 49,
    48, 49, 50, 51, 52, 53, 52, 53, 54, 55, 54, 55, 56, 57, 58, 59,
    58, 59, 60, 61, 60, 61, 60, 61, 62, 63, 64, 65, 64, 65, 66, 67,
    66, 67, 66, 67, 68, 69, 68, 69, 70, 71, 70, 71, 70, 71, 72, 73,
    72, 73, 72, 73, 74, 75, 74, 75, 74, 75, 76, 77, 76, 77, 126, 127
};
static const uint8_t s_f265CabacNextStateMPS[128] = {
    2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
    18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
    34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
    50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65,
    66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81,
    82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97,
    98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113,
    114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 124, 125, 126, 127
};
//I??ctxIdx=0~460??????m??????n??
static const int8_t f264_cabac_context_init_I[460][2] =
{
    /* 0 - 10 */
    { 20, -15 }, {  2, 54 },  {  3,  74 }, { 20, -15 },
    {  2,  54 }, {  3, 74 },  { -28,127 }, { -23, 104 },
    { -6,  53 }, { -1, 54 },  {  7,  51 },

    /* 11 - 23 unused for I */
    { 0, 0 },    { 0, 0 },    { 0, 0 },      { 0, 0 },
    { 0, 0 },    { 0, 0 },    { 0, 0 },      { 0, 0 },
    { 0, 0 },    { 0, 0 },    { 0, 0 },      { 0, 0 },
    { 0, 0 },

    /* 24- 39 */
    { 0, 0 },    { 0, 0 },    { 0, 0 },      { 0, 0 },
    { 0, 0 },    { 0, 0 },    { 0, 0 },      { 0, 0 },
    { 0, 0 },    { 0, 0 },    { 0, 0 },      { 0, 0 },
    { 0, 0 },    { 0, 0 },    { 0, 0 },      { 0, 0 },

    /* 40 - 53 */
    { 0, 0 },    { 0, 0 },    { 0, 0 },      { 0, 0 },
    { 0, 0 },    { 0, 0 },    { 0, 0 },      { 0, 0 },
    { 0, 0 },    { 0, 0 },    { 0, 0 },      { 0, 0 },
    { 0, 0 },    { 0, 0 },

    /* 54 - 59 */
    { 0, 0 },    { 0, 0 },    { 0, 0 },      { 0, 0 },
    { 0, 0 },    { 0, 0 },

    /* 60 - 69 */
    { 0, 41 },   { 0, 63 },   { 0, 63 },     { 0, 63 },
    { -9, 83 },  { 4, 86 },   { 0, 97 },     { -7, 72 },
    { 13, 41 },  { 3, 62 },

    /* 70 -> 87 */
    { 0, 11 },   { 1, 55 },   { 0, 69 },     { -17, 127 },
    { -13, 102 },{ 0, 82 },   { -7, 74 },    { -21, 107 },
    { -27, 127 },{ -31, 127 },{ -24, 127 },  { -18, 95 },
    { -27, 127 },{ -21, 114 },{ -30, 127 },  { -17, 123 },
    { -12, 115 },{ -16, 122 },

    /* 88 -> 104 */
    { -11, 115 },{ -12, 63 }, { -2, 68 },    { -15, 84 },
    { -13, 104 },{ -3, 70 },  { -8, 93 },    { -10, 90 },
    { -30, 127 },{ -1, 74 },  { -6, 97 },    { -7, 91 },
    { -20, 127 },{ -4, 56 },  { -5, 82 },    { -7, 76 },
    { -22, 125 },

    /* 105 -> 135 */
    { -7, 93 },  { -11, 87 }, { -3, 77 },    { -5, 71 },
    { -4, 63 },  { -4, 68 },  { -12, 84 },   { -7, 62 },
    { -7, 65 },  { 8, 61 },   { 5, 56 },     { -2, 66 },
    { 1, 64 },   { 0, 61 },   { -2, 78 },    { 1, 50 },
    { 7, 52 },   { 10, 35 },  { 0, 44 },     { 11, 38 },
    { 1, 45 },   { 0, 46 },   { 5, 44 },     { 31, 17 },
    { 1, 51 },   { 7, 50 },   { 28, 19 },    { 16, 33 },
    { 14, 62 },  { -13, 108 },{ -15, 100 },

    /* 136 -> 165 */
    { -13, 101 },{ -13, 91 }, { -12, 94 },   { -10, 88 },
    { -16, 84 }, { -10, 86 }, { -7, 83 },    { -13, 87 },
    { -19, 94 }, { 1, 70 },   { 0, 72 },     { -5, 74 },
    { 18, 59 },  { -8, 102 }, { -15, 100 },  { 0, 95 },
    { -4, 75 },  { 2, 72 },   { -11, 75 },   { -3, 71 },
    { 15, 46 },  { -13, 69 }, { 0, 62 },     { 0, 65 },
    { 21, 37 },  { -15, 72 }, { 9, 57 },     { 16, 54 },
    { 0, 62 },   { 12, 72 },

    /* 166 -> 196 */
    { 24, 0 },   { 15, 9 },   { 8, 25 },     { 13, 18 },
    { 15, 9 },   { 13, 19 },  { 10, 37 },    { 12, 18 },
    { 6, 29 },   { 20, 33 },  { 15, 30 },    { 4, 45 },
    { 1, 58 },   { 0, 62 },   { 7, 61 },     { 12, 38 },
    { 11, 45 },  { 15, 39 },  { 11, 42 },    { 13, 44 },
    { 16, 45 },  { 12, 41 },  { 10, 49 },    { 30, 34 },
    { 18, 42 },  { 10, 55 },  { 17, 51 },    { 17, 46 },
    { 0, 89 },   { 26, -19 }, { 22, -17 },

    /* 197 -> 226 */
    { 26, -17 }, { 30, -25 }, { 28, -20 },   { 33, -23 },
    { 37, -27 }, { 33, -23 }, { 40, -28 },   { 38, -17 },
    { 33, -11 }, { 40, -15 }, { 41, -6 },    { 38, 1 },
    { 41, 17 },  { 30, -6 },  { 27, 3 },     { 26, 22 },
    { 37, -16 }, { 35, -4 },  { 38, -8 },    { 38, -3 },
    { 37, 3 },   { 38, 5 },   { 42, 0 },     { 35, 16 },
    { 39, 22 },  { 14, 48 },  { 27, 37 },    { 21, 60 },
    { 12, 68 },  { 2, 97 },

    /* 227 -> 251 */
    { -3, 71 },  { -6, 42 },  { -5, 50 },    { -3, 54 },
    { -2, 62 },  { 0, 58 },   { 1, 63 },     { -2, 72 },
    { -1, 74 },  { -9, 91 },  { -5, 67 },    { -5, 27 },
    { -3, 39 },  { -2, 44 },  { 0, 46 },     { -16, 64 },
    { -8, 68 },  { -10, 78 }, { -6, 77 },    { -10, 86 },
    { -12, 92 }, { -15, 55 }, { -10, 60 },   { -6, 62 },
    { -4, 65 },

    /* 252 -> 275 */
    { -12, 73 }, { -8, 76 },  { -7, 80 },    { -9, 88 },
    { -17, 110 },{ -11, 97 }, { -20, 84 },   { -11, 79 },
    { -6, 73 },  { -4, 74 },  { -13, 86 },   { -13, 96 },
    { -11, 97 }, { -19, 117 },{ -8, 78 },    { -5, 33 },
    { -4, 48 },  { -2, 53 },  { -3, 62 },    { -13, 71 },
    { -10, 79 }, { -12, 86 }, { -13, 90 },   { -14, 97 },

    /* 276 a bit special (not used, f264_cabac_encode_bypass is used instead) */
    { 0, 0 },

    /* 277 -> 307 */
    { -6, 93 },  { -6, 84 },  { -8, 79 },    { 0, 66 },
    { -1, 71 },  { 0, 62 },   { -2, 60 },    { -2, 59 },
    { -5, 75 },  { -3, 62 },  { -4, 58 },    { -9, 66 },
    { -1, 79 },  { 0, 71 },   { 3, 68 },     { 10, 44 },
    { -7, 62 },  { 15, 36 },  { 14, 40 },    { 16, 27 },
    { 12, 29 },  { 1, 44 },   { 20, 36 },    { 18, 32 },
    { 5, 42 },   { 1, 48 },   { 10, 62 },    { 17, 46 },
    { 9, 64 },   { -12, 104 },{ -11, 97 },

    /* 308 -> 337 */
    { -16, 96 }, { -7, 88 },  { -8, 85 },    { -7, 85 },
    { -9, 85 },  { -13, 88 }, { 4, 66 },     { -3, 77 },
    { -3, 76 },  { -6, 76 },  { 10, 58 },    { -1, 76 },
    { -1, 83 },  { -7, 99 },  { -14, 95 },   { 2, 95 },
    { 0, 76 },   { -5, 74 },  { 0, 70 },     { -11, 75 },
    { 1, 68 },   { 0, 65 },   { -14, 73 },   { 3, 62 },
    { 4, 62 },   { -1, 68 },  { -13, 75 },   { 11, 55 },
    { 5, 64 },   { 12, 70 },

    /* 338 -> 368 */
    { 15, 6 },   { 6, 19 },   { 7, 16 },     { 12, 14 },
    { 18, 13 },  { 13, 11 },  { 13, 15 },    { 15, 16 },
    { 12, 23 },  { 13, 23 },  { 15, 20 },    { 14, 26 },
    { 14, 44 },  { 17, 40 },  { 17, 47 },    { 24, 17 },
    { 21, 21 },  { 25, 22 },  { 31, 27 },    { 22, 29 },
    { 19, 35 },  { 14, 50 },  { 10, 57 },    { 7, 63 },
    { -2, 77 },  { -4, 82 },  { -3, 94 },    { 9, 69 },
    { -12, 109 },{ 36, -35 }, { 36, -34 },

    /* 369 -> 398 */
    { 32, -26 }, { 37, -30 }, { 44, -32 },   { 34, -18 },
    { 34, -15 }, { 40, -15 }, { 33, -7 },    { 35, -5 },
    { 33, 0 },   { 38, 2 },   { 33, 13 },    { 23, 35 },
    { 13, 58 },  { 29, -3 },  { 26, 0 },     { 22, 30 },
    { 31, -7 },  { 35, -15 }, { 34, -3 },    { 34, 3 },
    { 36, -1 },  { 34, 5 },   { 32, 11 },    { 35, 5 },
    { 34, 12 },  { 39, 11 },  { 30, 29 },    { 34, 26 },
    { 29, 39 },  { 19, 66 },

    /* 399 -> 435 */
    {  31,  21 }, {  31,  31 }, {  25,  50 },
    { -17, 120 }, { -20, 112 }, { -18, 114 }, { -11,  85 },
    { -15,  92 }, { -14,  89 }, { -26,  71 }, { -15,  81 },
    { -14,  80 }, {   0,  68 }, { -14,  70 }, { -24,  56 },
    { -23,  68 }, { -24,  50 }, { -11,  74 }, {  23, -13 },
    {  26, -13 }, {  40, -15 }, {  49, -14 }, {  44,   3 },
    {  45,   6 }, {  44,  34 }, {  33,  54 }, {  19,  82 },
    {  -3,  75 }, {  -1,  23 }, {   1,  34 }, {   1,  43 },
    {   0,  54 }, {  -2,  55 }, {   0,  61 }, {   1,  64 },
    {   0,  68 }, {  -9,  92 },

    /* 436 -> 459 */
    { -14, 106 }, { -13,  97 }, { -15,  90 }, { -12,  90 },
    { -18,  88 }, { -10,  73 }, {  -9,  79 }, { -14,  86 },
    { -10,  73 }, { -10,  70 }, { -10,  69 }, {  -5,  66 },
    {  -9,  64 }, {  -5,  58 }, {   2,  59 }, {  21, -10 },
    {  24, -11 }, {  28,  -8 }, {  28,  -1 }, {  29,   3 },
    {  29,   9 }, {  35,  20 }, {  29,  36 }, {  14,  67 }
};
struct f264Cabad_t
{
	int offset;
	int range;
};

template <int n>
struct CordTermFlag2D {
    uint8_t a[n]; // Left
    uint8_t b[n]; // Up
};

template <>
struct CordTermFlag2D<1> {
    uint8_t a; // Left
    uint8_t b; // Up
    CordTermFlag2D() {}
    CordTermFlag2D(uint8_t _a, uint8_t _b) : a(_a), b(_b) {}
};
struct CABACRefFlags {
    uint8_t mbQPDelta;
    CordTermFlag2D<1> nonIntraNxN;
    CordTermFlag2D<1> intraChromaPredMode;
    CordTermFlag2D<1> transform8x8;
    CordTermFlag2D<2> cbpLuma;
    CordTermFlag2D<1> cbpChroma;
    CordTermFlag2D<4> nonZero;
    CordTermFlag2D<1> nonZeroDC;
};
struct CABACCurFlags {
    uint8_t mbQPDelta;
    uint8_t nonIntraNxN;
    uint8_t intraChromaPredMode;
    uint8_t transform8x8;
    uint8_t cbpLuma;
    uint8_t cbpChroma;
    uint32_t nonZero;
};
struct CABACNearbyFlags {
    bool nonIntraNxN;
    bool intraChromaPredMode;
    bool transform8x8;
    bool cbpLuma[2];
    bool cbpChromaDC;
    bool cbpChromaAC;
};
struct CABACPrevFlags {
    bool mbQPDelta;
};
struct CodedBlockFlags {
    bool nonZero[4];
    bool nonZeroDC;
};
union CompCoefs {
    CoefType blk4x4[16][16];
    CoefType blk8x8[4][64];
    struct Blk16x16Type {
        CoefType dc[16];
        CoefType ac[16][15];
    } blk16x16;
};

struct EncodedMb {
    MacroblockPartition mbPart;
    uint8_t transformSize8x8Flag;

    // Intra
    uint8_t prevIntraPredModeFlag[16];
    uint8_t remIntraPredMode[16];
    uint8_t intra16x16PredMode;
    uint8_t intraChromaPredMode;

    // Common
    CompCoefs coefs;
    uint32_t nonZeroFlags;
    int8_t qp;
    bool qpUpdated;
};

struct ed_t {};

class CABAD {
private:
	int DEBUG_ED; // for three decoder engine debug

    struct ContextModel {
        uint8_t pStateIdx;
        uint8_t valMPS;
    };
    CodedBlockFlags* leftCBF[COLOUR_COMPONENT_COUNT];
    CodedBlockFlags* upCBF[COLOUR_COMPONENT_COUNT];
    CABACPrevFlags* prevFlags[COLOUR_COMPONENT_COUNT];
    CABACNearbyFlags* leftFlags[COLOUR_COMPONENT_COUNT];
    CABACNearbyFlags* upFlags[COLOUR_COMPONENT_COUNT];

    CABACRefFlags refFlags[COLOUR_COMPONENT_COUNT];
    ContextModel ctxModels[CABAC_CONTEXT_MODELS_COUNT];

    EncodedMb* mb;
	void init();
	void run();
	void dump();
    void resetMbBuffer(EncodedMb* mbEnc);
    unsigned int  CabadDecordRegular(f264Cabad_t* cb, uint8_t* state);
    unsigned int CabadDecordTerminate(f264Cabad_t* cb);
    unsigned int CabadDecordBypass(f264Cabad_t* cb);
	void update(ed_t& edout);
    void f264cabacDecoder(f264Cabad_t *m_cb);
	unsigned int ReadBitstream(unsigned int length);
    void DecodeSlicedata(int colourID, f264Cabad_t* m_cb);
    void readMbTypeInfos(int colourID, f264Cabad_t* cb);
    void getCtxIdxForMbType();
    int getNeighbourState(int mb_num);
    void getCABACRefFlags();
	void f264CabacContextInit(int qp, SliceType type);
	void CabadInit(f264Cabad_t* cb);
    uint16_t getCtxIdxForMbType(SliceType type, const CordTermFlag2D<1>& flag, uint8_t binIdx);
public:
	unsigned int data = 0;
	int bitcnt = 0;
	int bytecnt = 0;
    uint8_t cbpLumaDCFlag; // used for 16x16 partition
    uint8_t cbpLumaACFlag; // used for 16x16 partition
    uint32_t firstMbInSlice;
    int neighbour;
	int m_frame;
	int mb_x;
	int mb_y;
	int m_qp;
    uint8_t cbpChroma;
    uint16_t widthInMbs;
    uint16_t heightInMbs;
	SliceType type;
	f264Cabad_t m_cb;
    uint8_t cbpLuma[COLOUR_COMPONENT_COUNT];
	unsigned int frame_width, frame_height;
	uint64_t cnt_regular;
	uint64_t cnt_bypass;
	uint64_t cnt_term;

	FILE* m_fpCabadInput_I;
	FILE* m_fpCabadOutput_I;
	FILE* m_fpCabadInput_P;

	void cabad_proc(ed_t& edput);
};
