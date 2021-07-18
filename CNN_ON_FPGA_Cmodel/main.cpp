#include<stdio.h>
#include"common.cpp"
#include"cfg.hpp"
#include <cstring>
int main(int argc, char **argv){

    cfg_t cfg;
    cfg_t *p_cfg = &cfg;
    // initial cfg
    cfgIni(p_cfg,argc, argv);
    
    pxl_t    datPic   [p_cfg->picHeight        ][p_cfg->picWidth       ]        = {0};  // 250 * 250
    weight_t weight_L1[p_cfg->L1_output_channel]                                = {0};  // 4
    bais_t   bias_L1  [p_cfg->L1_output_channel]                                = {0};  // 4
    weight_t weight_L2[p_cfg->L2_input_channel ][p_cfg->L2_output_channel]      = {0};  // 4 * 8
    bais_t   bias_L2  [p_cfg->L2_output_channel]                                = {0};  // 8
    weight_t weight_L3[p_cfg->L3_input_channel ][p_cfg->L3_output_channel]      = {0};  // 8 * 16
    bais_t   bias_L3  [p_cfg->L3_output_channel]                                = {0};  // 16
    weight_t weight_F1[p_cfg->F1_input_channel ][p_cfg->F1_output_channel]      = {0};  // 16 * 100
    bais_t   bias_F1  [p_cfg->F1_output_channel]                                = {0};  // 100
    weight_t weight_F2[p_cfg->F2_input_channel ][p_cfg->F2_output_channel]      = {0};  // 100 * 4
    bais_t   bias_F2  [p_cfg->F2_output_channel]                                = {0};  // 4

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
    // process
    for(int j = 0; j < p_cfg->picHeight; j++){
        for(int i = 0; i < p_cfg->picWidth; i++){

        }
    }

    return 0;
}