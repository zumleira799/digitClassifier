#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/mman.h>

extern void* readF(const char* filename, int elemSize, long* nElWriteB);
extern void writeF(void* d1, int size, int elSize, const char* filepath);

#define NNDataPath "../neuralData/NNbinaries"

#define squareSize 28
#define ReLUalpha 0.1

int predictNumber(int* RGBPixelArr, int xSize, int ySize){
    int xRatio = xSize/squareSize;
    int yRatio = ySize/squareSize;
    long ignore;

    float* preProcessedI = (float*)malloc(squareSize*squareSize*sizeof(float));

    for(int i = 0; i < squareSize; i++){
        for(int k = 0; k < squareSize; k++){
            float* curP = &preProcessedI[i*squareSize + k];
            *curP = 0;
            for(int y = i*yRatio; y < (i+1)*yRatio; y++){
                for(int x = k*xRatio; x < (k+1)*xRatio; x++){
                    int v = RGBPixelArr[y*xSize + x];
                    if(v == 0xffffff){
                        *curP = *curP+1;
                    }
                }
            }
            *curP = *curP/(xRatio*yRatio);
        }
    }

    for(int i = 0; i < ySize; i++){
        for(int k = 0; k < xSize; k++){
            if(k >= 28 || i >= 28){
                RGBPixelArr[i*xSize + k] = 0x000000;
                continue;
            }
            int r = (int)(0xff*preProcessedI[i*28 + k]);
            RGBPixelArr[i*xSize + k] = (r|r<<8|r<<16);
        }
    }

    float* NNdata = (float*)readF(NNDataPath, sizeof(float), &ignore);

    float* NNPointer = NNdata;

    float curP[128];
    float tempP[128];
    for(int i = 0; i < 128; i++){
        curP[i] = 0;
        tempP[i] = 0;
    }

    for(int i = 0; i < 784; i++){
        for(int k = 0; k < 128; k++){
            curP[k] += preProcessedI[i]*NNPointer[i*128 + k];
        }
    }
    NNPointer += 784*128;

    for(int i = 0; i < 128; i++){
        curP[i] += NNPointer[i];
        if(curP[i] <= 0){
            curP[i] *= ReLUalpha;
        }
    }
    NNPointer += 128;

    for(int i = 0; i < 128; i++){
        for(int k = 0; k < 64; k++){
            tempP[k] += curP[i]*NNPointer[i*64 + k];
        }
    }
    NNPointer += 128*64;

    for(int i = 0; i < 64; i++){
        curP[i] = 0;
        tempP[i] += NNPointer[i];
        if(tempP[i] <= 0){
            tempP[i] *= ReLUalpha;
        }
    }
    NNPointer += 64;

    for(int i = 0; i < 64; i++){
        for(int k = 0; k < 10; k++){
            curP[k] += tempP[i]*NNPointer[i*10 + k];
        }
    }
    NNPointer += 64*10;

    float max = -999999999.0;
    for(int i = 0; i < 10; i++){
        curP[i] += NNPointer[i];
        if(curP[i] > max){
            max = curP[i];
        }
    }

    float sum = 0;

    for(int i = 0; i < 10; i++){
        sum += exp(curP[i] - max);
    }
    for(int i = 0; i < 10; i++){
        curP[i] = exp(curP[i] - max)/sum;
    }

    float otherMax = -1;
    int curM = -1;
    for(int i = 0; i < 10; i++){
        if(curP[i] > otherMax){
            curM = i;
            otherMax = curP[i];
        }
    }

    free(preProcessedI);
    munmap(NNdata, 109386*sizeof(float));

    return curM;
}
