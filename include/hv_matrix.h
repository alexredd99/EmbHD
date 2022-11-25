#include <stdint.h>

typedef enum {
	HvBinary = 0, // uint32_t
	HvInt8,
	HvInt16,
	HvInt32,
	HvUint8,
	HvUint16,
	HvUint32
} HvDataType;

typedef enum {
	HvRandom = 0,
	HvLevel,
	HvNone 
} HvType;

typedef struct {
	HvDataType   dtype;
	unsigned int height;
	unsigned int width;
	unsigned int size; 
	void*        data;
} HvMatrix;

// For debugging, maybe delete later?
void HvMatrixPrint(HvMatrix* m);
void HvMatrixPrintRow(HvMatrix* m, unsigned int row);

// Malloc new hypervector/hyperspace (matrix)
HvMatrix* HvMatrixNew(HvType type, HvDataType dtype, 
		unsigned int height, unsigned int width);

// Free malloc-ed hypervector/hyperspace (matrix)
void HvMatrixFree(HvMatrix* m);

// Add (multiset) 2 HVs in different hyperspaces 
void HvMatrixAdd(HvMatrix* dst, unsigned int dst_row,
		HvMatrix* m0, unsigned int row0,
		HvMatrix* m1, unsigned int row1);

// Subtract 2 HVs in different hyperspaces
void HvMatrixSub(HvMatrix* dst, unsigned int dst_row,
		HvMatrix*  m0, unsigned int row0,
		HvMatrix*  m1, unsigned int row1);

// Multiply/Bind 2 HVs in different hyperspaces
void HvMatrixMult(HvMatrix* dst, unsigned int dst_row,
		HvMatrix* m0, unsigned int row0,
		HvMatrix* m1, unsigned int row1);

void HvMatrixScale(HvMatrix* m, unsigned int row, float scale);

unsigned int HvMatrixHamDist(HvMatrix* m0, unsigned int row0,
		HvMatrix* m1, unsigned int row1);

void HvMatrixQuant(HvMatrix* dst, unsigned int dst_row, HvMatrix* src, unsigned int src_row);

float HvMatrixCosSim(HvMatrix* m0, unsigned int row0,
        HvMatrix* m1, unsigned int row1);

// For debugging, maybe delete later?
void HvMatrixComp(HvMatrix* m0, HvMatrix* m1);