#include <string>
using namespace std;
struct cfg_t {
    string      strFilePic          ;
    string      strFileWeight_L1     ;
    string      strFileBias_L1      ;
    string      strFileWeight_L2     ;
    string      strFileBias_L2      ;
    string      strFileWeight_L3     ;
    string      strFileBias_L2      ;
    string      strFileWeight_F1     ;
    string      strFileWeight_F2     ;
};

typedef uint8_t     pxl_t;
typedef int16_t     weight_t;
typedef int16_t     bais_t;