#include <time.h>
#include <stdlib.h>
#include <math.h>  // REMOVE LATER, USE OPTIMIZED MATH

#ifndef MATRIX_LIB
#define MATRIX_LIB
#include "../include/matrix.h"
#endif

#include "../include/armcm4_helpers.h"
#include "../include/lookup_tables.h"

// Turn into x16 lookup table
/*
0001 0xFFFFFF01
0010 0xFFFF01FF
0011 0xFFFF0101
0100 0xFF01FFFF
0101 0xFF01FF01
0110 0xFF0101FF
0111 0xFF010101
1000 0x01FFFFFF
1001 0x01FFFF01
1010 0x01FF01FF
1011 0x01FF0101
1100 0x0101FFFF
1101 0x0101FF01
1110 0x010101FF
1111 0x01010101
*/

inline uint32_t bin_int8(uint8_t nibb){
    uint32_t result = 0;
    result |= (nibb & 0x01 ? 0x00000001 : 0x000000FF);
    result |= (nibb & 0x02 ? 0x00000100 : 0x0000FF00);
    result |= (nibb & 0x04 ? 0x00010000 : 0x00FF0000);
    result |= (nibb & 0x08 ? 0x01000000 : 0xFF000000);
    return result;
}

// Mask only sign bit, convert to lookup table, 0x80 sign bit, if set then negative
// if EQUAL TO ZERO MUST BE RANDOM!
inline uint8_t int8_bin(uint32_t *qword){
    uint8_t result = 0;
    int8_t* int8_data = (int8_t*) qword;
    for (unsigned int i = 0; i < 4; i++){
        if (int8_data[i] == 0){
            result |= (rand()%2) << i;
        } else if (int8_data[i] > 0){
            result |= 1 << i;
        }
    }

    return result;
}

// Quad 8-bit signed saturating addition
inline uint32_t __qadd8(uint32_t a, uint32_t b){
    uint32_t result;
    __asm volatile ("qadd8 %0, %1, %2" : "=r" (result) : "r" (a), "r" (b) );
    return result; 
}

// Dual 16-bit signed saturating addition
inline uint32_t __qadd16(uint32_t a, uint32_t b){
    uint32_t result;
    __asm volatile ("qadd16 %0, %1, %2" : "=r" (result) : "r" (a), "r" (b) );
    return result; 
}

// 32-bit signed saturating addition
inline uint32_t __qadd(uint32_t a, uint32_t b){
    uint32_t result;
    __asm volatile ("qadd %0, %1, %2" : "=r" (result) : "r" (a), "r" (b) );
    return result; 
}

// Quad 8-bit unsigned addition (overflowing)
inline uint32_t __sadd8(uint32_t a, uint32_t b){
    uint32_t result;
    __asm volatile ("sadd8 %0, %1, %2" : "=r" (result) : "r" (a), "r" (b) );
    return result; 
}

// Dual 16-bit unsigned addition (overflowing)
inline uint32_t __sadd16(uint32_t a, uint32_t b){
    uint32_t result;
    __asm volatile ("sadd16 %0, %1, %2" : "=r" (result) : "r" (a), "r" (b) );
    return result; 
}

// Quad 8-bit signed saturating subtraction, r = a-b 
inline uint32_t __qsub8(uint32_t a, uint32_t b){
    uint32_t result;
    __asm volatile("qsub8 %0, %1, %2" : "=r" (result) : "r" (a), "r" (b) );
    return result;
}

// Dual 16-bit signed saturating subtraction, r = a-b
inline uint32_t __qsub16(uint32_t a, uint32_t b){
    uint32_t result;
    __asm volatile ("qsub16 %0, %1, %2" : "=r" (result) : "r" (a), "r" (b) );
    return result; 
}

// 32-bit signed saturating subtraction, r = a-b
inline uint32_t __qsub(uint32_t a, uint32_t b){
    uint32_t result;
    __asm volatile ("qsub %0, %1, %2" : "=r" (result) : "r" (a), "r" (b) );
    return result; 
}

// Quad 8-bit unsigned subtraction (overflowing)
inline uint32_t __ssub8(uint32_t a, uint32_t b){
    uint32_t result;
    __asm volatile ("ssub8 %0, %1, %2" : "=r" (result) : "r" (a), "r" (b) );
    return result; 
}

// Dual 16-bit unsigned subtraction (overflowing)
inline uint32_t __ssub16(uint32_t a, uint32_t b){
    uint32_t result;
    __asm volatile ("ssub16 %0, %1, %2" : "=r" (result) : "r" (a), "r" (b) );
    return result; 
}

// Matrix Add
inline MOpStatus armcm4_MAdd(
        Matrix* dst, unsigned int dst_row, 
        Matrix* m0,  unsigned int m0_row,
        Matrix* m1,  unsigned int m1_row){
    unsigned int row_size = dst->size/dst->height;
    switch(dst->dtype){
        case MBin: {
            for (unsigned int x = 0; x < row_size; x++){
                dst->data[(dst_row*row_size)+x] =
                    m0->data[(m0_row*row_size)+x] | 
                    m1->data[(m1_row*row_size)+x];
            }
        }
            break;
        case MInt8: {
            for (unsigned int x = 0; x < row_size; x++){
                dst->data[(dst_row*row_size)+x] =
                    __qadd8(m0->data[(m0_row*row_size)+x], 
                            m1->data[(m1_row*row_size)+x]);
            }
        }
            break;
        case MInt16: {
            for (unsigned int x = 0; x < row_size; x++){
                dst->data[(dst_row*row_size)+x] =
                    __qadd16(m0->data[(m0_row*row_size)+x], 
                             m1->data[(m1_row*row_size)+x]);
            }
        }
            break;
        case MInt32: {
            for (unsigned int x = 0; x < row_size; x++){
                dst->data[(dst_row*row_size)+x] =
                    __qadd(m0->data[(m0_row*row_size)+x], 
                           m1->data[(m1_row*row_size)+x]);
            }
        }
            break;
        case MUint8: {  
            for (unsigned int x = 0; x < row_size; x++){
                dst->data[(dst_row*row_size)+x] =
                    __sadd8(m0->data[(m0_row*row_size)+x], 
                            m1->data[(m1_row*row_size)+x]);
            }
        }
            break;
        case MUint16: {
            for (unsigned int x = 0; x < row_size; x++){
                dst->data[(dst_row*row_size)+x] =
                    __sadd16(m0->data[(m0_row*row_size)+x], 
                             m1->data[(m1_row*row_size)+x]);
            }
        }
            break;
        case MUint32: {
            for (unsigned int x = 0; x < row_size; x++){
                dst->data[(dst_row*row_size)+x] =
                    m0->data[(m0_row*row_size)+x] + 
                    m1->data[(m1_row*row_size)+x];
            }
        }
            break;
        case MFloat32: {
            float* dst_data = (float*) dst->data + (dst_row*row_size);
            float* m0_data  = (float*) m0->data + (m0_row*row_size);
            float* m1_data  = (float*) m1->data + (m1_row*row_size);
            for (unsigned int x = 0; x < row_size; x++){
                dst_data[x] = m0_data[x] + m1_data[x];
            }
        }
            break;
        default: break;
    }
    return MOP_SUCCESS;
}

// Matrix Sub
inline MOpStatus armcm4_MSub(
        Matrix* dst, unsigned int dst_row, 
        Matrix* m0,  unsigned int m0_row,
        Matrix* m1,  unsigned int m1_row){
    unsigned int row_size = dst->size/dst->height;
    switch(dst->dtype){
        case MBin: {
            for (unsigned int x = 0; x < row_size; x++){
                //dst->data[x] = m0->data[x] | m1->data[x];
                return MOP_UNIMPLEMENTED;
            }
        }
            break;
        case MInt8: {
            for (unsigned int x = 0; x < row_size; x++){
                dst->data[(dst_row*row_size)+x] = 
                    __qsub8(m0->data[(m0_row*row_size)+x], 
                            m1->data[(m1_row*row_size)+x]);
            }
        }
            break;
        case MInt16: {
            for (unsigned int x = 0; x < row_size; x++){
                dst->data[(dst_row*row_size)+x] = 
                    __qsub16(m0->data[(m0_row*row_size)+x], 
                             m1->data[(m1_row*row_size)+x]);
            }
        }
            break;
        case MInt32: {
            for (unsigned int x = 0; x < row_size; x++){
                dst->data[(dst_row*row_size)+x] = 
                    __qsub(m0->data[(m0_row*row_size)+x], 
                           m1->data[(m1_row*row_size)+x]);
            }
        }
            break;
        case MUint8: {  
            for (unsigned int x = 0; x < row_size; x++){
                dst->data[(dst_row*row_size)+x] =
                    __ssub8(m0->data[(m0_row*row_size)+x], 
                            m1->data[(m1_row*row_size)+x]);
            }
        }
            break;
        case MUint16: {
            for (unsigned int x = 0; x < row_size; x++){
                dst->data[(dst_row*row_size)+x] = 
                    __ssub16(m0->data[(m0_row*row_size)+x], 
                             m1->data[(m1_row*row_size)+x]);
            }
        }
            break;
        case MUint32: {
            for (unsigned int x = 0; x < row_size; x++){
                dst->data[(dst_row*row_size)+x] = 
                    m0->data[(m0_row*row_size)+x] - 
                    m1->data[(m1_row*row_size)+x];
            }
        }
            break;
        case MFloat32: {
            float* dst_data = (float*) dst->data + (dst_row*row_size);
            float* m0_data  = (float*) m0->data + (m0_row*row_size);
            float* m1_data  = (float*) m1->data + (m1_row*row_size);
            for (unsigned int x = 0; x < row_size; x++){
                dst_data[x] = m0_data[x] - m1_data[x];
            }
        }
            break;
        default: break;
    }
    return MOP_SUCCESS;
}

// Matrix Mult
inline MOpStatus armcm4_MMult(
        Matrix* dst, unsigned int dst_row, 
        Matrix* m0,  unsigned int m0_row,
        Matrix* m1,  unsigned int m1_row){
    unsigned int row_size = dst->size/dst->height;
    switch(dst->dtype){
        case MBin: {
            for (unsigned int x = 0; x < row_size; x++){
                dst->data[(dst_row*row_size)+x] =
                    m0->data[(m0_row*row_size)+x] ^
                    m1->data[(m1_row*row_size)+x];
            }
        }
            break;
        case MInt8:   return MOP_UNIMPLEMENTED;
        case MInt16:  return MOP_UNIMPLEMENTED;
        case MInt32:  return MOP_UNIMPLEMENTED;
        case MUint8:  return MOP_UNIMPLEMENTED;
        case MUint16: return MOP_UNIMPLEMENTED;
        case MUint32: return MOP_UNIMPLEMENTED;
        case MFloat32: {
            float* dst_data = (float*) dst->data + (dst_row*row_size);
            float* m0_data  = (float*) m0->data + (m0_row*row_size);
            float* m1_data  = (float*) m1->data + (m1_row*row_size);
            for (unsigned int x = 0; x < row_size; x++){
                dst_data[x] = m0_data[x] * m1_data[x];
            }
        }
            break;
        default: break;
    }
    return MOP_SUCCESS;
}

static inline void set_bit(Matrix *m, unsigned int y, unsigned int x, uint8_t val){
    unsigned int row_size = m->size / m->height;
    unsigned int offset_byte = x / 32;
    unsigned int bit = x-(32*offset_byte);
    uint32_t* data = (uint32_t*) m->data;
    
    if (val) {
        data[(y*row_size)+offset_byte] |= (1 << (31-bit));
    } else {
        data[(y*row_size)+offset_byte] &= ~(1 << (31-bit));
    }
}
// 0 -> -1, 1 -> 1
static inline int get_bit(Matrix *m, unsigned int y, unsigned int x ){
    unsigned int row_size = m->size / m->height;
    unsigned int offset_size = x / 32;
    unsigned int bit = x-(32*offset_size);
    uint32_t* data = (uint32_t*) m->data;
    return ((data[(y*row_size)+offset_size] & (1 << (31-bit)))) ? 1 : -1;
}


// Matrix Convert
inline MOpStatus armcm4_MConvert(
        Matrix* dst, unsigned int dst_row,
        Matrix* src, unsigned int src_row){
    
    switch(dst->dtype){
        case MBin: {
            switch(src->dtype){ // DONT BREAK ACTUALLY
                case MBin: break; // MBin <- MBin
                case MInt8: { // MBin <- MInt8
                    unsigned int src_row_size = src->size/src->height;
                    unsigned int dst_row_size = (dst->size/dst->height); // Converting to int8 
                    uint8_t* dst_data = (uint8_t*) (dst->data + (dst_row*dst_row_size));
                    dst_row_size *= 4;
                    uint32_t* src_data = src->data + (src_row*src_row_size);
                    unsigned int y = 0;
                    for (unsigned int x = 0; (x < dst_row_size) && (y < src_row_size); x++){
                        if ((x % 2) == 0) {
                            dst_data[x] = int8_bin(src_data + y);
                        } else {
                            dst_data[x] |= (int8_bin(src_data + y) << 4);   
                            y++;
                        }
                    }
                }
                    break;
                case MInt16: { // MBin <- MInt16
                }
                    break;
                case MInt32: { // MBin <- MInt32
                }
                    break;
                case MUint8: { // MBin <- MUint8
                }  
                    break;
                case MUint16: { // MBin <- MUint16
                }
                    break;
                case MUint32: { // MBin <- MUint32
                }
                    break;
                default: break;
            }
        }
            break;
        case MInt8: {
            switch(src->dtype){
                case MBin: { // MInt8 <- MBin
                    unsigned int src_row_size = (src->size/src->height);
                    unsigned int dst_row_size = dst->size/dst->height;
                    uint8_t* src_data = (uint8_t*) (src->data + (src_row*src_row_size));
                    src_row_size *= 4;
                    uint32_t* dst_data = dst->data + (dst_row*dst_row_size);
                    unsigned int y = 0;
                    for (unsigned int x = 0; x < dst_row_size; x++){
                        if ((x % 2) == 0) {
                            dst_data[x] = bin_int8(src_data[y]);
                        } else {
                            dst_data[x] = bin_int8(src_data[y] >> 4);
                            y++;
                        }
                    }
                }
                    break;
                case MInt8: { // MInt8 <- MInt8 COPY DATA
                    unsigned int row_size = dst->size/dst->height;
                    for (unsigned int x = 0; x < row_size; x++){
                        dst->data[(dst_row*row_size)+x] =
                            (src->data[(src_row*row_size)+x]);
                    }
                }
                    break;
                case MInt16: {
                }
                    break;
                case MInt32: {
                }
                    break;
                case MUint8: {
                }  
                    break;
                case MUint16: {
                }
                    break;
                case MUint32: {
                }
                    break;
                default: break;
            }
        }
            break;
        case MInt16: {
            switch(src->dtype){
                case MBin: {
                }
                    break;
                case MInt8: {
                }
                    break;
                case MInt16: break;
                case MInt32: {
                }
                    break;
                case MUint8: {
                }  
                    break;
                case MUint16: {
                }
                    break;
                case MUint32: {
                }
                    break;
                default: break;
            }
        }
            break;
        case MInt32: {
            switch(src->dtype){
                case MBin: {
                }
                    break;
                case MInt8: {
                }
                    break;
                case MInt16: {
                }
                    break;
                case MInt32: break;
                case MUint8: {
                }  
                    break;
                case MUint16: {
                }
                    break;
                case MUint32: {
                }
                    break;
                default: break;
            }
        }
            break;
        case MUint8: {
            switch(src->dtype){
                case MBin: {
                }
                    break;
                case MInt8: {
                }
                    break;
                case MInt16: {
                }
                    break;
                case MInt32: {
                }
                    break;
                case MUint8: break;
                case MUint16: {
                }
                    break;
                case MUint32: {
                }
                    break;
                default: break;
            }
        }  
            break;
        case MUint16: {
            switch(src->dtype){
                case MBin: {
                }
                    break;
                case MInt8: {
                }
                    break;
                case MInt16: {
                }
                    break;
                case MInt32: {
                }
                    break;
                case MUint8: {
                }  
                    break;
                case MUint16: break;
                case MUint32: {
                }
                    break;
                default: break;
            }
        }
            break;
        case MUint32: {
            switch(src->dtype){
                case MBin: {
                }
                    break;
                case MInt8: {
                }
                    break;
                case MInt16: {
                }
                    break;
                case MInt32: {
                }
                    break;
                case MUint8: {
                }  
                    break;
                case MUint16: {
                }
                    break;
                case MUint32: break;
                default: break;
            }
        }
            break;
        default: break;
    }
    return MOP_SUCCESS;
}

inline MOpStatus armcm4_MHamDist(
        unsigned int* result,
        Matrix* m0, unsigned int m0_row,
        Matrix* m1, unsigned int m1_row){
    
    unsigned int row_size = m0->size/m0->height;
    uint32_t temp = 0;
    *result = 0;
    for (unsigned int x = 0; x < row_size; x++){
        temp = m0->data[(m0_row*row_size)+x] ^
               m1->data[(m1_row*row_size)+x];
        *result += ham_weight_lookup[temp>>24] +
                ham_weight_lookup[(temp>>16) & 0xFF] +
                ham_weight_lookup[(temp>>8) & 0xFF] +
                ham_weight_lookup[temp & 0xFF];
    }
    return MOP_SUCCESS;
}

inline MOpStatus armcm4_MEqual(
        unsigned int* result,
        Matrix* m0, Matrix* m1){
    
    *result = 1;
    for (unsigned int x = 0; x < m0->size; x++){
        if(m0->data[x] != m1->data[x]){
            *result = 0;
            break;
        }
    }
    return MOP_SUCCESS;
}

inline MOpStatus armcm4_MSin(
        Matrix* dst, unsigned int dst_row,
        Matrix* src, unsigned int src_row){
    // Should I check type here or in matrix.c instead?
    if (dst->dtype != MFloat32) {return MOP_TYPE_ERROR;}
    unsigned int row_size = src->size/src->height;

    switch(src->dtype){
        case MBin: { // Is this even possible?
            return MOP_UNIMPLEMENTED;
        }
            break;
        case MInt8: {
            int8_t* src_data = (int8_t*) (src->data + (src_row*row_size));
            float* dst_data = (float*) (dst->data + (dst_row*row_size));
            for (unsigned int x = 0; x < src->width; x++){
                dst_data[x] = (float) sinf((float)src_data[x]);
            }
        }
            break;
        case MInt16: {
            return MOP_UNIMPLEMENTED;
        }
            break;
        case MInt32: {
            return MOP_UNIMPLEMENTED;
        }
            break;
        case MUint8: {  
            return MOP_UNIMPLEMENTED;
        }
            break;
        case MUint16: {
            return MOP_UNIMPLEMENTED;
        }
            break;
        case MUint32: {
            return MOP_UNIMPLEMENTED;
        }
            break;
        case MFloat32: {
            float* dst_data = (float*) dst->data + (dst_row*row_size);
            float* src_data = (float*) src->data + (src_row*row_size);
            for (unsigned int x = 0; x < row_size; x++){
                dst_data[x] = sinf(src_data[x]);
            }
        }
            break;
        default: 
            return MOP_UNIMPLEMENTED;
            break;
    }
    return MOP_SUCCESS;
}

inline MOpStatus armcm4_MCos(
        Matrix* dst, unsigned int dst_row,
        Matrix* src, unsigned int src_row){
    // Should I check type here or in matrix.c instead?
    if (dst->dtype != MFloat32) {return MOP_TYPE_ERROR;}
    unsigned int row_size = src->size/src->height;

    switch(src->dtype){
        case MBin: { // Is this even possible?
            return MOP_UNIMPLEMENTED;
        }
            break;
        case MInt8: {
            int8_t* src_data = (int8_t*) (src->data + (src_row*row_size));
            float* dst_data = (float*) (dst->data + (dst_row*row_size));
            for (unsigned int x = 0; x < src->width; x++){
                dst_data[x] = (float) cosf((float)src_data[x]);
            }
        }
            break;
        case MInt16: {
            return MOP_UNIMPLEMENTED;
        }
            break;
        case MInt32: {
            return MOP_UNIMPLEMENTED;
        }
            break;
        case MUint8: {  
            return MOP_UNIMPLEMENTED;
        }
            break;
        case MUint16: {
            return MOP_UNIMPLEMENTED;
        }
            break;
        case MUint32: {
            return MOP_UNIMPLEMENTED;
        }
            break;
        case MFloat32: {
            float* dst_data = (float*) dst->data + (dst_row*row_size);
            float* src_data = (float*) src->data + (src_row*row_size);
            for (unsigned int x = 0; x < row_size; x++){
                dst_data[x] = cosf(src_data[x]);
            }
        }
            break;
        default: 
            return MOP_UNIMPLEMENTED;
            break;
    }
    return MOP_SUCCESS;
}

inline MOpStatus armcm4_MLinear(
        Matrix* dst,
        Matrix* m0,
        Matrix* m1){
   
    unsigned int dst_rsize = dst->size/dst->height;
    unsigned int m0_rsize = m0->size/m0->height; // should be equal?
    unsigned int m1_rsize = m1->size/m1->height;

    switch(dst->dtype){
        case MBin:    return MOP_UNIMPLEMENTED;
        case MInt8: {
            int8_t* dst_data = (int8_t*) dst->data;
            for (unsigned int x = 0; x < dst_rsize; x++){ // Col
                dst_data[x] = 0;
                int8_t* m0_data  = (int8_t*) m0->data + (x*m0_rsize);
                int8_t* m1_data  = (int8_t*) m1->data + (x*m1_rsize);
                for (unsigned int y = 0; y < m1_rsize; y++){ // row
                    dst_data[x] += (m0_data[x] * m1_data[(y*m1_rsize)+x]);
                }
            }
        }
            break;
        case MInt16:  return MOP_UNIMPLEMENTED;
        case MInt32:  return MOP_UNIMPLEMENTED;
        case MUint8:  return MOP_UNIMPLEMENTED;
        case MUint16: return MOP_UNIMPLEMENTED;
        case MUint32: return MOP_UNIMPLEMENTED;
        case MFloat32:{
            float* dst_data = (float*) dst->data;
            float* m0_data  = (float*) m0->data;
            float* m1_data  = (float*) m1->data;

            for (unsigned int y = 0; y < dst_rsize; y++){
                dst_data[y] = 0;
                for (unsigned int x = 0; x < m1_rsize; x++){
                    dst_data[y] += (m0_data[x] * m1_data[(y*m1_rsize)+x]);
                }
            }
        }
            break;
    }
    return MOP_SUCCESS;
}