#ifndef MATRIX_LIB
#define MATRIX_LIB
#include "matrix.h"
#endif

#define MIN_INT8   -128
#define MAX_INT8   127
#define MIN_INT16  -32768
#define MAX_INT16  32767

// Matrix Add
inline unsigned int debug_MAdd(
        Matrix* dst, unsigned int dst_row, 
        Matrix* m0,  unsigned int m0_row,
        Matrix* m1,  unsigned int m1_row){

    switch(dst->dtype){
        case MBin: {}
            break;
        case MInt8: {
            int8_t* dst_int8 = (int8_t*) dst->data;
            int8_t* m0_int8  = (int8_t*) m0->data;
            int8_t* m1_int8  = (int8_t*) m1->data;
            int16_t temp;

            for (unsigned int x = 0; x < m0->width; x++){
                temp = m0_int8[(m0->width*m0_row)+x] +
                        m1_int8[(m1->width*m1_row)+x];
                temp = temp > MAX_INT8 ? MAX_INT8 : 
                        temp < MIN_INT8 ? MIN_INT8 : temp;
                dst_int8[(dst->width*dst_row)+x] = (int8_t) temp;
            }}
            break;
        case MInt16: {
            int16_t* dst_int16 = (int16_t*) dst->data;
            int16_t* m0_int16  = (int16_t*) m0->data;
            int16_t* m1_int16  = (int16_t*) m1->data;
            int32_t temp;

            for (unsigned int x = 0; x < m0->width; x++){
                temp = m0_int16[(m0->width*m0_row)+x] +
                        m1_int16[(m1->width*m1_row)+x];
                temp = temp > MAX_INT16 ? MAX_INT16 : 
                        temp < MIN_INT16 ? MIN_INT16 : temp;
                dst_int16[(dst->width*dst_row)+x] = (int16_t) temp;
            }
        }
            break;
        case MInt32:{}
            break;
        case MUint8:{}
            break;
        case MUint16:{}
            break;
        case MUint32:{}
            break;
        default:{}
            break;
    }
    return MOP_SUCCESS;
}