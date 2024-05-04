#include "benchmarking.h"
#include "FLINNG.h"
#include "dataset.h"
#include "evaluate.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

typedef chrono::high_resolution_clock Clock;

void runTrials(size_t b, size_t r, size_t reps, size_t range, uint *hashes,
               uint max_reps, unsigned int *gtruth_indice,
               int *query_sparse_indice, float *query_sparse_val,
               int *query_sparse_marker, LSH *hash_family) {

  unsigned int *queryOutputs = new unsigned int[NUMQUERY * TOPK]();

  auto begin = Clock::now();
  float etime_0;

  // Create index
  FLINNG *flinng =
      new FLINNG(r, b, hashes, max_reps, hash_family, range, reps, NUMBASE);
  flinng->finalize_construction();

  auto end = Clock::now();
  etime_0 = (end - begin).count() / 1000000;
  cout << "Indexing took " << etime_0 << "ms." << endl;

  cout << "Querying..." << endl;
  omp_set_num_threads(1);
  begin = Clock::now();
  for (uint i = 0; i < NUMQUERY; i++) {
    uint32_t *recall_buffer = new uint32_t[TOPK];
    flinng->query(query_sparse_indice, query_sparse_val,
                  query_sparse_marker + i, TOPK, recall_buffer);
    for (size_t j = 0; j < TOPK; j++) {
      queryOutputs[TOPK * i + j] = recall_buffer[j];
    }
    delete[] recall_buffer;
  }
  end = Clock::now();
  omp_set_num_threads(80);

  etime_0 = (end - begin).count() / 1000000;
  std::cout << "Queried " << NUMQUERY << " datapoints, used " << etime_0
            << "ms. \n";

  evaluate(queryOutputs, NUMQUERY, TOPK, gtruth_indice, AVAILABLE_TOPK);

  delete flinng;
  delete[] queryOutputs;
}

void runBenchmark() {
  omp_set_num_threads(80);
  std::cout << "Reading groundtruth and data ... " << std::endl;

  int *sparse_data_indice;
  float *sparse_data_val;
  int *sparse_data_marker;
  int *sparse_query_indice;
  float *sparse_query_val;
  int *sparse_query_marker;
  readDataAndQueries(BASEFILE, NUMQUERY, NUMBASE, &sparse_data_indice,
                     &sparse_data_val, &sparse_data_marker,
                     &sparse_query_indice, &sparse_query_val,
                     &sparse_query_marker);

  unsigned int *gtruth_indice = new unsigned int[NUMQUERY * AVAILABLE_TOPK];
  readGroundTruthInt(GTRUTHINDICE, NUMQUERY, AVAILABLE_TOPK, gtruth_indice);

  cout << "Starting total hash generation" << endl;
  cout << "Starting index building and query grid parameter test" << endl;
  unsigned int *queryOutputs = new unsigned int[NUMQUERY * TOPK]();

  for (size_t reps = STARTREPS; reps <= MAXREPS; reps *= REPRATIO) {
    std::cout << "Initializing data hashes, array size " << reps * NUMBASE
              << endl;
    unsigned int *hashes;
    unsigned int *indices;

    LSH *hashFamily = new LSH(K, reps, RANGE);
    hashes = new unsigned int[reps * NUMBASE];
    indices = new unsigned int[reps * NUMBASE];
    hashFamily->getHash(hashes, indices, sparse_data_indice, sparse_data_val,
                        sparse_data_marker, NUMBASE, 1, NUMBASE, 0);

    for (size_t r = STARTR; r < ENDR; r++) {
      for (size_t b = STARTB; b < ENDB; b *= BRATIO) {
        if (b * r > 1 << 16) {
          continue;
        }
        std::cout << "STATS_GROUPS: " << r << " " << b << " " << RANGE << " "
                  << reps << std::endl;
        runTrials(b, r, reps, RANGE, hashes, reps, gtruth_indice,
                  sparse_query_indice, sparse_query_val, sparse_query_marker,
                  hashFamily);
      }
    }
  }

  delete[] sparse_data_indice;
  delete[] sparse_data_marker;
  delete[] sparse_query_indice;
  delete[] sparse_query_val;
  delete[] sparse_query_marker;
  delete[] gtruth_indice;
  delete[] queryOutputs;
}

int main() { runBenchmark(); }