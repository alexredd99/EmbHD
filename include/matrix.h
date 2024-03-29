#include <stdint.h>

typedef enum {
	MBin = 0, // uint32_t
	MInt8,
	MInt16,
	MInt32,
	MUint8,
	MUint16,
	MUint32,
        MFloat32,
} MDataType;

typedef struct {
    MDataType    dtype;
    unsigned int height;
    unsigned int width;
    unsigned int size;
    uint32_t*    data;
} Matrix;

typedef enum {  
    MOP_SUCCESS = 0,
    MOP_TYPE_ERROR,
    MOP_WIDTH_ERROR,
    MOP_HEIGHT_ERROR,
    MOP_UNIMPLEMENTED,
} MOpStatus;

// convert between matrix types
// converting to binary = QUANT from old library
MOpStatus MConvert(
        Matrix* dst, unsigned int dst_row,
        Matrix* src, unsigned int src_row);

// add matrices
MOpStatus MAdd(
        Matrix* dst, unsigned int dst_row, 
        Matrix* m0,  unsigned int m0_row,
        Matrix* m1,  unsigned int m1_row);

MOpStatus MSub(
        Matrix* dst, unsigned int dst_row, 
        Matrix* m0,  unsigned int m0_row,
        Matrix* m1,  unsigned int m1_row);

MOpStatus MMult(
        Matrix* dst, unsigned int dst_row, 
        Matrix* m0,  unsigned int m0_row,
        Matrix* m1,  unsigned int m1_row);

MOpStatus MHamDist(
        unsigned int* result,
        Matrix* m0, unsigned int m0_row,
        Matrix* m1, unsigned int m1_row);

// result 0 if no, 1 if yes
MOpStatus MEqual(
        unsigned int* result,
        Matrix* m0, Matrix* m1);

MOpStatus MDot(
        Matrix* dst, unsigned int dst_row, 
        Matrix* m0,  unsigned int m0_row,
        Matrix* m1,  unsigned int m1_row);

// Dst MUST be float32?
// Maybe offer software emulation of tiny floats in future
MOpStatus MSin(
        Matrix* dst, unsigned int dst_row,
        Matrix* src, unsigned int src_row);

// Dst MUST be float32?
MOpStatus MCos(
        Matrix* dst, unsigned int dst_row,
        Matrix* src, unsigned int src_row);

// Linear transformation, maybe add optional bias term?
// Computes dst = m0 * m1^T
// m0 shape of AxB, m1 shape of CxB
// dst shape of AxC
MOpStatus MLinear(
        Matrix* dst,
        Matrix* m0,
        Matrix* m1);