#ifndef MATRIX_LIB
#define MATRIX_LIB
#include "../include/matrix.h"
#endif

//// iff architecture...
#ifndef ARMCM4
#define ARMCM4
#include "../include/armcm4_helpers.h"
#endif


MOpStatus MConvert(
        Matrix* dst, unsigned int dst_row,
        Matrix* src, unsigned int src_row){
    
    if(dst->width != src->width){
        return MOP_WIDTH_ERROR;
    }
    
    // IF DEF ARCH CORTEX-M4
    // ELSE, x86 debug
    return armcm4_MConvert(dst,dst_row,src,src_row);
}

MOpStatus MAdd(
        Matrix* dst, unsigned int dst_row, 
        Matrix* m0,  unsigned int m0_row,
        Matrix* m1,  unsigned int m1_row){

    if(!((dst->dtype == m0->dtype) && (dst->dtype == m1->dtype))){
        return MOP_TYPE_ERROR;
    }
    if(!((dst->width == m0->width) && (dst->width == m1->width))){
        return MOP_WIDTH_ERROR;
    }

    // IF DEF ARCH CORTEX-M4
    // ELSE, x86 debug
    return armcm4_MAdd(dst, dst_row, m0, m0_row, m1, m1_row);
}

MOpStatus MSub(
        Matrix* dst, unsigned int dst_row,
        Matrix* m0,  unsigned int m0_row,
        Matrix* m1,  unsigned int m1_row){
    if(!((dst->dtype == m0->dtype) && (dst->dtype == m1->dtype))){
        return MOP_TYPE_ERROR;
    }
    if(!((dst->width == m0->width) && (dst->width == m1->width))){
        return MOP_WIDTH_ERROR;
    }

    // IF DEF ARCH CORTEX-M4
    // ELSE, x86 debug
    return armcm4_MSub(dst, dst_row, m0, m0_row, m1, m1_row);
}

MOpStatus MMult(
        Matrix* dst, unsigned int dst_row, 
        Matrix* m0,  unsigned int m0_row,
        Matrix* m1,  unsigned int m1_row){
    if(!((dst->dtype == m0->dtype) && (dst->dtype == m1->dtype))){
        return MOP_TYPE_ERROR;
    }
    if(!((dst->width == m0->width)&&(dst->width == m1->width))){
        return MOP_WIDTH_ERROR;
    }

    // IF DEF ARCH CORTEX-M4
    // ELSE, x86 debug
    return armcm4_MMult(dst, dst_row, m0, m0_row, m1, m1_row);
}

MOpStatus MHamDist(
        unsigned int* result,
        Matrix* m0, unsigned int m0_row,
        Matrix* m1, unsigned int m1_row){
    if(m0->dtype != m1->dtype){return MOP_TYPE_ERROR;}
    if(m0->width != m1->width){return MOP_WIDTH_ERROR;}

    // Check ARCH
    return armcm4_MHamDist(result, m0, m0_row, m1, m1_row);
}

MOpStatus MEqual(
        unsigned int* result,
        Matrix* m0, Matrix* m1){
    if(m0->dtype != m1->dtype){return MOP_TYPE_ERROR;}
    if(m0->width != m1->width){return MOP_WIDTH_ERROR;}
    if(m0->height != m1->height){return MOP_HEIGHT_ERROR;}

    // Check ARCH
    return armcm4_MEqual(result, m0, m1);
}

MOpStatus MSin(
        Matrix* dst, unsigned int dst_row,
        Matrix* src, unsigned int src_row){
    // Ignore type error for now
    if(src->width != dst->width){return MOP_WIDTH_ERROR;}

    // Check ARCH
    return armcm4_MSin(dst, dst_row, src, src_row);
}

MOpStatus MCos(
        Matrix* dst, unsigned int dst_row,
        Matrix* src, unsigned int src_row){
    if(src->width != dst->width){return MOP_WIDTH_ERROR;}

    // Check ARCH
    return armcm4_MCos(dst, dst_row, src, src_row);
}