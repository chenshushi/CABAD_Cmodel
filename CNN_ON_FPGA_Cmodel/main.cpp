#include<stdio.h>
#include"common.hpp"
#include"cfg.hpp"
#include <cstring>
int main(int argc, char **argv){

    cfg_t cfg;
    cfg_t *p_cfg = &cfg;
    // initial cfg
    cfgIni(p_cfg,argc, argv);
    
    weight_t datPic   [250 ][250]                        = {0};  // 250 * 250
    weight_t weight_L1[4][5][5]                          = {0};  // 4
    bais_t   bias_L1  [4]                                = {0};  // 4
    weight_t weight_L2[4][8][5][5]                       = {0};  // 4 * 8
    bais_t   bias_L2  [8]                                = {0};  // 8
    weight_t weight_L3[8][16]                            = {0};  // 8 * 16
    bais_t   bias_L3  [16]                               = {0};  // 16
    weight_t weight_F1[16][100]                          = {0};  // 16 * 100
    bais_t   bias_F1  [100]                              = {0};  // 100
    weight_t weight_F2[100][4]                           = {0};  // 100 * 4
    bais_t   bias_F2  [4]                                = {0};  // 4

    FILE *fp_pic         = fopen(p_cfg->strFilePic.c_str(),      "rb");
    FILE *fp_weight_L1   = fopen(p_cfg->strFileWeight_L1.c_str(),"rb");
    FILE *fp_bias_L1     = fopen(p_cfg->strFileBias_L1.c_str(),  "rb");
    FILE *fp_weight_L2   = fopen(p_cfg->strFileWeight_L2.c_str(),"rb");
    FILE *fp_bias_L2     = fopen(p_cfg->strFileBias_L2.c_str(),  "rb");
    FILE *fp_weight_L3   = fopen(p_cfg->strFileWeight_L3.c_str(),"rb");
    FILE *fp_bias_L3     = fopen(p_cfg->strFileBias_L2.c_str(),  "rb");
    FILE *fp_weight_F1   = fopen(p_cfg->strFileWeight_F1.c_str(),"rb");
    FILE *fp_bias_F1     = fopen(p_cfg->strFileBias_F1.c_str(),  "rb");
    FILE *fp_weight_F2   = fopen(p_cfg->strFileWeight_F2.c_str(),"rb");
    FILE *fp_bias_F2     = fopen(p_cfg->strFileBias_F2.c_str(),  "rb");

    //Read picture
    for(int j = 0; j < p_cfg->picHeight; j++){
        fread(datPic[j],sizeof(pxl_t),p_cfg->picWidth,fp_pic);
    }
    //Read weight_L1
    for(int j = 0; j <p_cfg->L1_output_channel; j++){
        fread(weight_L1,sizeof(weight_t),1,fp_weight_L1);
    }
    //Read bias_L1
    for(int j = 0; j <p_cfg->L1_output_channel; j++){
        fread(bias_L1,sizeof(bais_t),1,fp_bias_L1);
    }
    //Read weight_L2
    for(int j = 0; j <p_cfg->L2_output_channel; j++){
        fread(weight_L2,sizeof(weight_t),p_cfg->L2_input_channel,fp_weight_L2);
    }
    //Read bias_L2
    for(int j = 0; j <p_cfg->L2_output_channel; j++){
        fread(bias_L2,sizeof(weight_t),p_cfg->L2_input_channel,fp_bias_L2);
    }
    //Read weight_L3
    for(int j = 0; j <p_cfg->L3_output_channel; j++){
        fread(weight_L3,sizeof(weight_t),p_cfg->L3_input_channel,fp_weight_L3);
    }
    //Read bias_L3
    for(int j = 0; j <p_cfg->L3_output_channel; j++){
        fread(bias_L3,sizeof(weight_t),1,fp_bias_L3);
    }
    //Read weight_F1
    for(int j = 0; j <p_cfg->F1_output_channel; j++){
        fread(weight_F1,sizeof(weight_t),p_cfg->F1_input_channel,fp_weight_F1);
    }
    //Read bias_F1
    for(int j = 0; j <p_cfg->F1_output_channel; j++){
        fread(bias_F1,sizeof(weight_t),p_cfg->F1_input_channel,fp_bias_F1);
    }
    //Read weight_F2
    for(int j = 0; j <p_cfg->F2_output_channel; j++){
        fread(weight_F2,sizeof(weight_t),p_cfg->F2_input_channel,fp_weight_F2);
    }
    //Read bias_F2
    for(int j = 0; j <p_cfg->F2_output_channel; j++){
        fread(bias_F2,sizeof(weight_t),p_cfg->F2_input_channel,fp_bias_F2);
    }
    // process main
    // L1
    //                      chennel  pixel    pixel
    weight_t result_data_L1[4      ][246    ][246     ] = {0};
    for(int posYCur = 0; posYCur < 246; posYCur++){
        for(int posXCur = 0; posXCur < 246; posXCur++){
            weight_t *datCurBlock = &datPic[posYCur][posXCur];

            result_data_L1[0][posYCur][posXCur] = filter(datCurBlock, weight_L1[0],250);
            result_data_L1[1][posYCur][posXCur] = filter(datCurBlock, weight_L1[1],250);
            result_data_L1[2][posYCur][posXCur] = filter(datCurBlock, weight_L1[2],250);
            result_data_L1[3][posYCur][posXCur] = filter(datCurBlock, weight_L1[3],250);
        }
    }
    // L1_pool
    weight_t pic_data_L2   [4    ][123    ][123     ] = {0};
    for(int Y = 0; Y < 246 - 1; Y = Y + 2) {
        for(int X = 0; X < 246 - 1; X = X + 2) {
            pic_data_L2[0][Y/2][X/2] = (result_data_L1[0][Y + 0][X + 0] + result_data_L1[0][Y + 0][X + 1]
                                     +  result_data_L1[0][Y + 1][X + 0] + result_data_L1[0][Y + 1][X + 1] + 2) >> 2;
            pic_data_L2[1][Y/2][X/2] = (result_data_L1[1][Y + 0][X + 0] + result_data_L1[1][Y + 0][X + 1]
                                     +  result_data_L1[1][Y + 1][X + 0] + result_data_L1[1][Y + 1][X + 1] + 2) >> 2;
            pic_data_L2[2][Y/2][X/2] = (result_data_L1[2][Y + 0][X + 0] + result_data_L1[2][Y + 0][X + 1]
                                     +  result_data_L1[2][Y + 1][X + 0] + result_data_L1[2][Y + 1][X + 1] + 2) >> 2;
            pic_data_L2[3][Y/2][X/2] = (result_data_L1[3][Y + 0][X + 0] + result_data_L1[3][Y + 0][X + 1]
                                     +  result_data_L1[3][Y + 1][X + 0] + result_data_L1[3][Y + 1][X + 1] + 2) >> 2;
        }
    }
    // L2
    //                       output chennel  pixel    pixel
    weight_t result_data_L2 [8             ][119    ][119     ] = {0};
    for(int posYCur = 0; posYCur < 119; posYCur++){
        for(int posXCur = 0; posXCur < 119; posXCur++){
            weight_t *datCurBlock_0 = &pic_data_L2[0][posYCur][posXCur];
            weight_t *datCurBlock_1 = &pic_data_L2[1][posYCur][posXCur];
            weight_t *datCurBlock_2 = &pic_data_L2[2][posYCur][posXCur];
            weight_t *datCurBlock_3 = &pic_data_L2[3][posYCur][posXCur];
            for(int i = 0; i < 8; i++) {
                result_data_L2[i][posYCur][posXCur] = (filter(datCurBlock_0, weight_L2[0][i],123)
                                                    +  filter(datCurBlock_1, weight_L2[1][i],123)
                                                    +  filter(datCurBlock_2, weight_L2[2][i],123)
                                                    +  filter(datCurBlock_3, weight_L2[3][i],123) + 2) >> 2;
            }
        }
    }
    // L2_pool
    weight_t pic_data_L3     [8][60][60] = {0};   // have padding problem !!!
    for (int Y = 0; Y < 119 - 1; Y++) {
        for (int X = 0; X < 119 - 1; X++) {
            for (int i = 0; i < 8; i++) {
                pic_data_L3[i][Y/2][X/2] = (result_data_L2[i][Y + 0][X + 0] + result_data_L2[i][Y + 0][X + 1]
                                         +  result_data_L2[i][Y + 1][X + 0] + result_data_L2[i][Y + 1][X + 1] + 2) >> 2;
            }
        }
    }

    //L3
    //                      chennel  pixel    pixel
    weight_t result_data_L3 [16             ][119    ][119     ] = {0};
    for(int posYCur = 0; posYCur < 119; posYCur++){
        for(int posXCur = 0; posXCur < 119; posXCur++){
            weight_t *datCurBlock_0 = &pic_data_L2[0][posYCur][posXCur];
            weight_t *datCurBlock_1 = &pic_data_L2[1][posYCur][posXCur];
            weight_t *datCurBlock_2 = &pic_data_L2[2][posYCur][posXCur];
            weight_t *datCurBlock_3 = &pic_data_L2[3][posYCur][posXCur];
            for(int i = 0; i < 8; i++) {
                result_data_L2[i][posYCur][posXCur] = (filter(datCurBlock_0, weight_L2[0][i],123)
                                                    +  filter(datCurBlock_1, weight_L2[1][i],123)
                                                    +  filter(datCurBlock_2, weight_L2[2][i],123)
                                                    +  filter(datCurBlock_3, weight_L2[3][i],123) + 2) >> 2;
            }
        }
    }




    return 0;
}