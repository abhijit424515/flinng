#include "LSH.h"
#include <assert.h>

#ifndef INT_MAX
#define INT_MAX 0xffffffff
#endif

#ifndef UINT_MAX
#define UINT_MAX 0xffffffff
#endif

LSH::LSH(int _K_in, int _L_in, int _rangePow_in) {
  _K = _K_in;
  _L = _L_in;
  _numTables = _L_in;
  _rangePow = _rangePow_in;

  printf("<<< LSH Parameters >>>\n");
  std::cout << "_K " << _K << std::endl;
  std::cout << "_L " << _L << std::endl;
  std::cout << "_rangePow " << _rangePow_in << std::endl;

  rand1 = new int[_K * _L];

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<unsigned int> dis(1, UINT_MAX);

  // Generate rand1 - odd random numbers.
  for (int i = 0; i < _K * _L; i++) {
    rand1[i] = dis(gen);
    if (rand1[i] % 2 == 0)
      rand1[i]++;
  }

  _numhashes = _K * _L;
  _lognumhash = log2(_numhashes);
  std::cout << "_lognumhash " << _lognumhash << std::endl;

  // _randa and _randHash* are random odd numbers.
  _randa = dis(gen);
  if (_randa % 2 == 0)
    _randa++;
  _randHash = new int[2];
  _randHash[0] = dis(gen);
  if (_randHash[0] % 2 == 0)
    _randHash[0]++;
  _randHash[1] = dis(gen);
  if (_randHash[1] % 2 == 0)
    _randHash[1]++;
  std::cout << "Optimal Densified Hashing intialized ...  \n";
}

LSH::~LSH() {
  delete[] _randHash;
  delete[] rand1;
}

/* OpenMP - Sparse. */
void LSH::getHash(unsigned int *hashIndices, unsigned int *probeDataIdx,
                  int *dataIdx, float *dataVal, int *dataMarker,
                  int numInputEntries, int numProbes, size_t maxNumEntries,
                  size_t offset) {
  getOptimalMinhash(hashIndices, probeDataIdx, dataIdx, dataMarker,
                    numInputEntries, numProbes);
}

void LSH::getOptimalMinhash(unsigned int *hashIndices,
                            unsigned int *probeDataIdx, int *dataIdx,
                            int *dataMarker, int numInputEntries,
                            int numProbes) {
  for (int inputIdx = 0; inputIdx < numInputEntries; inputIdx++) {
    unsigned int *hashes = new unsigned int[_numhashes];

    int sizenonzeros = dataMarker[inputIdx + 1] - dataMarker[inputIdx];
    optimalMinHash(hashes, (unsigned int *)(dataIdx + dataMarker[inputIdx]),
                   sizenonzeros);

    for (int tb = 0; tb < _L; tb++) {
      unsigned int index = 0;
      for (int k = 0; k < _K; k++) {
        unsigned int h = hashes[_K * tb + k];
        h *= rand1[_K * tb + k];
        h ^= h >> 13;
        h ^= rand1[_K * tb + k];
        index += h * hashes[_K * tb + k];
      }
      index = (index << 2) >> (32 - _rangePow);

      hashIndices[hashIndicesOutputIdx(numProbes, numInputEntries, inputIdx, 0,
                                       tb)] = index;
      probeDataIdx[hashIndicesOutputIdx(numProbes, numInputEntries, inputIdx, 0,
                                        tb)] = inputIdx;
      for (int k = 1; k < numProbes; k++) {
        hashIndices[hashIndicesOutputIdx(numProbes, numInputEntries, inputIdx,
                                         k, tb)] = index ^ (1 << (k - 1));
        probeDataIdx[hashIndicesOutputIdx(numProbes, numInputEntries, inputIdx,
                                          k, tb)] = inputIdx;
      }
    }
    delete[] hashes;
  }
}

// The range of hashes returned by getRandDoubleHash is 2^_lognumhash = numhash.
unsigned int LSH::getRandDoubleHash(int binid, int count) {
  unsigned int tohash = ((binid + 1) << 10) + count;
  return ((unsigned int)_randHash[0] * tohash << 3) >>
         (32 - _lognumhash); // _lognumhash needs to be ceiled.
}

void LSH::optimalMinHash(unsigned int *hashArray, unsigned int *nonZeros,
                         int sizenonzeros) {
  /* This function computes the minhash and perform densification. */
  unsigned int *hashes = new unsigned int[_numhashes];

  unsigned int range = 1 << _rangePow;
  // binsize is the number of times the range is larger than the total number of
  // hashes we need.
  unsigned int binsize = ceil(range / _numhashes);

  for (size_t i = 0; i < _numhashes; i++) {
    hashes[i] = INT_MAX;
  }

  for (size_t i = 0; i < sizenonzeros; i++) {
    unsigned int h = nonZeros[i];
    h *= _randa;
    h ^= h >> 13;
    h *= 0x85ebca6b;
    unsigned int curhash =
        ((unsigned int)(((unsigned int)h * nonZeros[i]) << 5) >>
         (32 - _rangePow));
    unsigned int binid = std::min((unsigned int)floor(curhash / binsize),
                                  (unsigned int)(_numhashes - 1));
    if (hashes[binid] > curhash)
      hashes[binid] = curhash;
  }

  /* Densification of the hash. */
  for (size_t i = 0; i < _numhashes; i++) {
    unsigned int next = hashes[i];
    if (next != INT_MAX) {
      hashArray[i] = hashes[i];
      continue;
    }
    unsigned int count = 0;
    while (next == INT_MAX) {
      count++;
      unsigned int index =
          std::min((unsigned)getRandDoubleHash((unsigned int)i, count),
                   (unsigned)_numhashes);
      next = hashes[index]; // Kills GPU.
      if (count > 100)      // Densification failure.
        break;
    }
    hashArray[i] = next;
  }

  delete[] hashes;
}