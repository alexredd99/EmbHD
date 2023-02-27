#ifndef MATRIX_LIB
#define MATRIX_LIB
#include "../include/matrix.h"
#endif


inline MOpStatus armcm4_MConvert(
        Matrix* dst, unsigned int dst_row,
        Matrix* src, unsigned int src_row);

inline MOpStatus armcm4_MAdd(
        Matrix* dst, unsigned int dst_row, 
        Matrix* m0,  unsigned int m0_row,
        Matrix* m1,  unsigned int m1_row);

inline MOpStatus armcm4_MSub(
        Matrix* dst, unsigned int dst_row, 
        Matrix* m0,  unsigned int m0_row,
        Matrix* m1,  unsigned int m1_row);

inline MOpStatus armcm4_MMult(
        Matrix* dst, unsigned int dst_row, 
        Matrix* m0,  unsigned int m0_row,
        Matrix* m1,  unsigned int m1_row);

inline MOpStatus armcm4_MHamDist(
        unsigned int* result,
        Matrix* m0, unsigned int m0_row,
        Matrix* m1, unsigned int m1_row);

inline MOpStatus armcm4_MEqual(
        unsigned int* result,
        Matrix* m0, Matrix* m1);

inline MOpStatus armcm4_MSin(
        Matrix* dst, unsigned int dst_row,
        Matrix* src, unsigned int src_row);

inline MOpStatus armcm4_MCos(
        Matrix* dst, unsigned int dst_row,
        Matrix* src, unsigned int src_row);