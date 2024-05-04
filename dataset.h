#pragma once
#include <iostream>

using namespace std;

void readDataAndQueries(string basefile, uint numQuery, uint numBase,
                        int **sparse_data_indice, float **sparse_data_val,
                        int **sparse_data_marker, int **sparse_query_indice,
                        float **sparse_query_val, int **sparse_query_marker);

void readSparse(string fileName, int offset, int n, int *indices, float *values,
                int *markers, unsigned int bufferlen);

void readGroundTruthInt(const std::string &file, int numQueries,
                        int availableTopK, unsigned int *out);