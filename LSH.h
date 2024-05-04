#include "omp.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <stdio.h>
#include <stdlib.h>

#define hashIndicesOutputIdx(numProbes, numInputs, dataIdx, probeIdx, tb)      \
  (unsigned long long)(numInputs * numProbes * tb + dataIdx * numProbes +      \
                       probeIdx)
#define hashesOutputIdx(numHashPerFamily, numInputs, dataIdx, tb,              \
                        hashInFamIdx)                                          \
  (unsigned long long)(tb * (numInputs * numHashPerFamily) +                   \
                       dataIdx * numHashPerFamily + hashInFamIdx)

class LSH {
private:
  int _rangePow;
  int _numTables;

  /* Optimal Densified Minhash. */
  int *_randHash, _randa, _numhashes, _lognumhash, _K;
  int _L;
  int *rand1;

  unsigned int getRandDoubleHash(int binid, int count);
  void optimalMinHash(unsigned int *hashArray, unsigned int *nonZeros,
                      int sizenonzeros);
  void getOptimalMinhash(unsigned int *hashIndices, unsigned int *probeDataIdx,
                         int *dataIdx, int *dataMarker, int numInputEntries,
                         int numProbes);

public:
  void getHash(unsigned int *hashIndices, unsigned int *identity, int *dataIdx,
               float *dataVal, int *dataMarker, int numInputEntries,
               int numProbes, size_t maxNumEntries, size_t offset);

  LSH(int _K_in, int _L_in, int _rangePow_in);
  ~LSH();
};
