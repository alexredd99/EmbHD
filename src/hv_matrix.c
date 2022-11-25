#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "hv_matrix.h"
#include "ham_lookup.h"

extern unsigned char ham_weight_lookup[256];

#define BYTE_TO_BINARY(byte) \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

// Move to external log function, have user define printing/logging function given input
// Print entire HV Matrix
void HvMatrixPrint(HvMatrix* m){
    // Add other types 
    switch(m->dtype){
        case HvBinary:
            uint32_t* data_bin = (uint32_t*) m->data;
            for (int i = 0; i < m->size; i++) {
                printf("%c%c%c%c%c%c%c%c", BYTE_TO_BINARY(data_bin[i] >> 24));
                printf("%c%c%c%c%c%c%c%c", BYTE_TO_BINARY((data_bin[i] >> 16)) & 0xFF);
                printf("%c%c%c%c%c%c%c%c", BYTE_TO_BINARY((data_bin[i] >> 8)) & 0xFF);
                printf("%c%c%c%c%c%c%c%c", BYTE_TO_BINARY(data_bin[i] & 0xFF));
                if ((i+1) % (m->size/m->height) == 0){
                    printf("\n");
                }
            }
            break;
        case HvInt32:
            int32_t* data_int32 = (int32_t*) m->data;
            for (int y = 0; y < m->height; y++){
                for (int x = 0; x < m->width; x++){
                    printf("%d ", data_int32[(m->width*y)+x]);
                }
                printf("\n");
            }
            break;
        default:
            break;
    }
}

// Print single HV from HV Matrix/Hyperspace
void HvMatrixPrintRow(HvMatrix* m, unsigned int row){
    // Add other types
    switch(m->dtype){
        case HvBinary:
            uint32_t* data_bin = (uint32_t*) m->data;
            unsigned int row_size = m->size / m->height;
            for (int i = 0; i < row_size; i++) {
                printf("%c%c%c%c%c%c%c%c", BYTE_TO_BINARY(data_bin[(row_size*row)+i] >> 24));
                printf("%c%c%c%c%c%c%c%c", BYTE_TO_BINARY((data_bin[(row_size*row)+i] >> 16)) & 0xFF);
                printf("%c%c%c%c%c%c%c%c", BYTE_TO_BINARY((data_bin[(row_size*row)+i] >> 8)) & 0xFF);
                printf("%c%c%c%c%c%c%c%c", BYTE_TO_BINARY(data_bin[(row_size*row)+i] & 0xFF));
                if ((i+1) % (m->size/m->height) == 0){
                    printf("\n");
                }
            }
            break;
        case HvInt16: {
            for (unsigned int x = 0; x < m->width; x++){
                int16_t* data_int16 = (int16_t*) m->data;
                printf("%d ",data_int16[(m->width*row)+x]);
            }
            printf("\n");
        }
        default:
            break;
    }
}


HvMatrix* HvMatrixNew(HvType type, HvDataType dtype, 
        unsigned int height, unsigned int width){
    HvMatrix* new = malloc(sizeof(HvMatrix));
    if (!new){
        // Need to define error function, similar to log/print function
    }
    /* NotImplemented */
}   

void HvMatrixFree(HvMatrix* m){
    // Need to check if m is valid first
    free(m->data); 
    free(m);
}

// -1 if 0 , 1 if 1
static inline int get_val(HvMatrix *m, unsigned int y, unsigned int x ){
    // Need to do some sort of bounds checking...
    //if ((y > matrix->height) || (x > matrix->width)){ 
    //    printf("OUT OF BOUNDS, Y: %d X: %d\n", y, x);
    //    exit(1);
    //}

    unsigned int row_size = m->size / m->height;
    unsigned int offset_size = x / 32;
    unsigned int bit = x-(32*offset_size);
    uint32_t* data = (uint32_t*) m->data;
    return ((data[(y*row_size)+offset_size] & (1 << (31-bit)))) ? 1 : -1;
}

static inline void set_val(HvMatrix *m, unsigned int y, unsigned int x, uint8_t val){
    // Need to do some sort of bounds checking...
    //if ((y > matrix->height) || (x > matrix->width)){ 
    //    printf("OUT OF BOUNDS, Y: %d X: %d\n", y, x);
    //    exit(1);
    //}
    
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

void HvMatrixAdd(HvMatrix* dst, unsigned int dst_row,
		         HvMatrix*  m0, unsigned int row0,
		         HvMatrix*  m1, unsigned int row1){
    
    //if((dst->dtype != src->dtype) ||
    //    (dst->height != src->height) ||
    //    (dst->width != src->width)){
    //    //  ERROR
    //}
    // ERROR CHECK HERE
   
    // REALLY hacky, need to do bounds and type checking...

    switch(m0->dtype) {
        case HvInt8: {
            int8_t* dst_int8 = (int8_t*) dst->data;
            int8_t* m0_int8  = (int8_t*) m0->data;
            int8_t* m1_int8  = (int8_t*) m1->data;
            int16_t temp;
            /*
                Implicit type conversions bad...
                    Options:
                        User must explicitly call type conversion function first
                        Pass special condition code? (eh...)
                        All conversions between most types, give best effort
                            Could be ALOT of code and type checking
            */
            if (dst->dtype == HvInt8 && m0->dtype == HvInt8 && m1->dtype == HvBinary){
                for (unsigned int x = 0; x < m0->width; x++){
                    temp = m0_int8[(m0->width*row0)+x] + get_val(m1,row1,x);
                    if (temp > 127) {
                        temp = 127;
                    } else if (temp < -128) {
                        temp = -128;
                    }
                    dst_int8[(dst->width*dst_row)+x] = (int8_t) temp;
                }

            } else {
                for (unsigned int x = 0; x < m0->width; x++){
                    temp = m0_int8[(m0->width*row0)+x] +
                           m1_int8[(m1->width*row1)+x];
                    if (temp > 127) {
                        temp = 127;
                    } else if (temp < -128) {
                        temp = -128;
                    }
                    dst_int8[(dst->width*dst_row)+x] = (int8_t) temp;
                }
            }

        }
            break;
        case HvBinary:
        case HvUint32: {
            uint32_t* dst_data_uint32  = (uint32_t*) dst->data;
            uint32_t* m0_data_uint32   = (uint32_t*) m0->data;
            uint32_t* m1_data_uint32   = (uint32_t*) m1->data;
            unsigned int row_size = dst->size/dst->height;

            for (unsigned int i = 0; i < row_size; i++){
                dst_data_uint32[(dst_row*row_size)+i] = 
                    m0_data_uint32[(row0*row_size)+i] | 
                    m1_data_uint32[(row1*row_size)+i];
            }
        }
            break;
        case HvInt16: {
            int16_t* dst_int16 = (int16_t*) dst->data;
            int16_t* m0_int16  = (int16_t*) m0->data;
            int32_t temp;
            if (dst->dtype == HvInt16 && m0->dtype == HvInt16 && m1->dtype == HvBinary){
                for (unsigned int x = 0; x < m0->width; x++){
                    temp = m0_int16[(m0->width*row0)+x] + get_val(m1,row1,x); 
                    if (temp > 32767) {
                        dst_int16[(dst->width*dst_row)+x] = 32767;
                    } else if (temp < -32768) {
                        dst_int16[(dst->width*dst_row)+x] = -32767;
                    } else {
                        dst_int16[(dst->width*dst_row)+x] = temp;
                    }
                }
            } else {
                int16_t* m1_int16 = (int16_t*) m1->data;
                for (unsigned int x = 0; x < m0->width; x++){
                    temp = m0_int16[(m0->width*row0)+x] +
                           m1_int16[(m1->width*row1)+x];
                    if (temp > 32767) {
                        dst_int16[(dst->width*dst_row)+x] = 32767;
                    } else if (temp < -32768) {
                        dst_int16[(dst->width*dst_row)+x] = -32767;
                    } else {
                        dst_int16[(dst->width*dst_row)+x] = temp;
                    }
                }
            }
        }
            break;
        default:
            break;
    }
}

// Need to add code from HvMatrixAdd
void HvMatrixSub(HvMatrix* dst, unsigned int dst_row,
		         HvMatrix*  m0, unsigned int row0,
		         HvMatrix*  m1, unsigned int row1){
    // Type and bounds checking...
    switch(m0->dtype){
        case HvInt8: {
            int8_t* dst_int8 = (int8_t*) dst->data;
            int8_t* m0_int8  = (int8_t*) m0->data;
            int8_t* m1_int8  = (int8_t*) m1->data;
            int16_t temp;

            for (unsigned int x = 0; x < m0->width; x++){
                temp = m0_int8[(m0->width*row0)+x] -
                       m1_int8[(m1->width*row1)+x];
                if (temp > 127) {
                    temp = 127;
                } else if (temp < -128) {
                    temp = -128;
                }
                dst_int8[(dst->width*dst_row)+x] = (int8_t) temp;
            }
        }
            break;
        default:
            break;
    }
}

void HvMatrixScale(HvMatrix* m, unsigned int row, float scale){
    // Type and bounds checking
    int8_t* m_int8 = (int8_t*) m->data;

    for(unsigned int x = 0; x < m->width; x++){
        m_int8[(m->width*row)+x] *= scale;
    }
}

void HvMatrixMult(HvMatrix* dst, unsigned int dst_row,
		HvMatrix* m0, unsigned int row0,
		HvMatrix* m1, unsigned int row1){
    // Type and bounds checking
    switch(m0->dtype){
        case HvBinary:
        case HvUint32: {
            uint32_t* dst_data_uint32  = (uint32_t*) dst->data;
            uint32_t* m0_data_uint32   = (uint32_t*) m0->data;
            uint32_t* m1_data_uint32   = (uint32_t*) m1->data;
            unsigned int row_size = dst->size/dst->height;

            for (unsigned int i = 0; i < row_size; i++){
                dst_data_uint32[(dst_row*row_size)+i] = 
                    m0_data_uint32[(row0*row_size)+i] ^
                    m1_data_uint32[(row1*row_size)+i];
            }
        }
            break;
        default:
            break;
    }
}

// Make inline?
static unsigned int count_ones(uint32_t curr_int){
    unsigned int count;
    count = ham_weight_lookup[curr_int>>24]          +
            ham_weight_lookup[(curr_int>>16) & 0xFF] +
            ham_weight_lookup[(curr_int>>8) & 0xFF]  + 
            ham_weight_lookup[curr_int & 0xFF];
    return count;
}

unsigned int HvMatrixHamDist(HvMatrix* m0, unsigned int row0,
		HvMatrix* m1, unsigned int row1) {
    if((m0->dtype != m1->dtype) ||
        (m0->height != m1->height) ||
        (m0->width !=  m1->width)){
        //  ERROR
    }
    unsigned int dist = 0;
    switch(m0->dtype){
        case HvBinary: {
            unsigned int row_size = m0->size / m0->height;
            uint32_t* m0_data = (uint32_t*) m0->data;
            uint32_t* m1_data = (uint32_t*) m1->data;
            
            for (int i = 0; i < row_size; i++){
                dist += count_ones(m0_data[(row0*row_size)+i] ^
                                   m1_data[(row1*row_size)+i]);
            }
        }
            break;
        default:
            break;
    }
    return dist;
}

// MUST HAVE SAME WIDTH, CHECK LATER
// maybe set dst data to zeroes, then wont have to use set_val 0?
// Make dst ONLY HV BINARY ? Still hacky...
void HvMatrixQuant(HvMatrix* dst, unsigned int dst_row, HvMatrix* src, unsigned int src_row){
    if (dst->dtype == HvBinary && src->dtype == HvInt16){
        int16_t* src_int16 = (int16_t*)src->data;
            for (unsigned int x = 0; x < src->width; x++){
                if (src_int16[(src_row*src->width)+x] < 0){
                    set_val(dst,dst_row,x,0);
                } else if (src_int16[(src_row*src->width)+x] == 0){
                    set_val(dst,dst_row,x,rand()%2); // random 0/1
                } else {
                    set_val(dst,dst_row,x,1);
                }
            }
    } else if (dst->dtype == HvInt16 && src->dtype == HvInt16) {
        int16_t* src_int16 = (int16_t*)src->data;
        int16_t* dst_int16 = (int16_t*)dst->data;
        int16_t temp = 0;
        for (unsigned int x = 0; x < src->width; x++){
            if (src_int16[(src->width*src_row)+x] < 0){
                temp = -1;
            } else if (src_int16[(src->width*src_row)+x] == 0){
                temp = rand()%2 == 0 ? -1 : 1;
            } else {
                temp = 1;
            }
            dst_int16[(dst->width*dst_row)+x] = temp;
        }
    } else if (dst->dtype == HvBinary && src->dtype == HvInt8){
        int8_t* src_int8 = (int8_t*) src->data;
        int8_t* dst_int8 = (int8_t*) dst->data;
        int8_t temp = 0;
        for (unsigned int x = 0; x < src->width; x++){
            temp = src_int8[(src->width*src_row)+x];
            if (temp < 0){
                set_val(dst,dst_row,x,0);
            } else if (temp == 0){
                set_val(dst,dst_row,x,rand()%2);
            } else {
                set_val(dst,dst_row,x,1);
            }
        }

    } else {
        int count;    
        for (unsigned int x = 0; x < src->width; x++){
            count = 0;
            for (unsigned int y = 0; y < src->height; y++){
                count += get_val(src, y, x);
            }
            if (count > 0){
                set_val(dst, dst_row, x, 1);
            } else {
                set_val(dst, dst_row, x, 0);
            }
        }
    }
}

// CHECK TYPE
float HvMatrixCosSim(HvMatrix* m0, unsigned int row0,
        HvMatrix* m1, unsigned int row1){
    
    int dot = 0;
    int mag0 = 0;
    int mag1 = 0;
    
    int8_t* m0_int8 = (int8_t*) m0->data;
    int8_t* m1_int8 = (int8_t*) m1->data;

    for (unsigned int i = 0; i < m0->width; i++){
        int temp0 = (int) m0_int8[(m0->width*row0)+i];
        int temp1 = (int) m1_int8[(m1->width*row1)+i];

        dot  += temp0*temp1;
        mag0 += temp0*temp0;
        mag1 += temp1*temp1;
    }

    return ((float)dot)/(sqrt((float)mag0)*sqrt((float)mag1));
}

// Delete later? Really only used for debugging at the moment
// MUST HAVE SAME SIZE
void HvMatrixComp(HvMatrix* m0, HvMatrix* m1){
    switch(m0->dtype){
    case HvBinary:
    case HvUint8: {
        unsigned int row_bytes = m0->size / m0->height; // Might be wrong
        uint8_t* m0_uint8 = (uint8_t*) m0->data;
        uint8_t* m1_uint8 = (uint8_t*) m1->data;
        for (int y = 0; y < m0->height; y++){
            for (int x = 0; x < row_bytes; x++){
                if (m0_uint8[(y*row_bytes)+x] !=m1_uint8[(y*row_bytes)+x]){
                    printf("ERROR\n");
                    exit(1);
                }
            }
        }
    }
        break;
    default:
        break;
    }
}