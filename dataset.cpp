#include "dataset.h"
#include "benchmarking.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

void readSparse(string fileName, int offset, int n, int *indices, float *values,
                int *markers, unsigned int bufferlen) {
  std::cout << "[readSparse]" << std::endl;

  for (int i = 0; i <= n; i++) {
    markers[i] = bufferlen - 1;
  }

  std::ifstream file(fileName);
  std::string str;

  unsigned int ct = 0;            // Counting the input vectors.
  unsigned int totalLen = 0;      // Counting all the elements.
  while (std::getline(file, str)) // Get one vector (one vector per line).
  {
    if (ct < offset) { // If reading with an offset, skip < offset vectors.
      ct++;
      continue;
    }
    std::istringstream iss(str);
    std::string sub;
    iss >> sub;
    markers[ct - offset] = min(totalLen, bufferlen - 1);
    int pos;
    float val;
    int curLen = 0;
    do {
      std::string sub;
      iss >> sub;
      pos = sub.find_first_of(":");
      if (pos == string::npos) {
        continue;
      }
      val = stof(sub.substr(pos + 1, (str.length() - 1 - pos)));
      pos = stoi(sub.substr(0, pos));

      if (totalLen < bufferlen) {
        indices[totalLen] = pos;
        values[totalLen] = val;
      } else {
        std::cout << "[readSparse] Buffer is too small, data is truncated!\n";
        return;
      }
      curLen++;
      totalLen++;
    } while (iss);

    ct++;
    if (ct == (offset + n)) {
      break;
    }
  }
  markers[ct - offset] = totalLen;
  std::cout << "[readSparse] Read " << totalLen << " numbers, " << ct - offset
            << " vectors. " << std::endl;
}

void readGroundTruthInt(const std::string &file, int numQueries,
                        int availableTopK, unsigned int *out) {
  std::ifstream myFile(file, std::ios::in | std::ios::binary);

  if (!myFile) {
    printf("Error opening file ... \n");
    return;
  }

  char cNum[256];
  int ct = 0;
  while (myFile.good() && ct < availableTopK * numQueries) {
    myFile.good();
    myFile.getline(cNum, 256, ' ');
    out[ct] = atoi(cNum);
    ct++;
  }
  myFile.close();
}

bool load_vector_from_stream_int(std::istream &in, std::vector<size_t> &vec,
                                 size_t *first) {
  std::string line;
  std::getline(in, line);
  std::stringstream ss(line);
  std::string buff;
  vec.clear();

  size_t i = 0;
  bool ret = false;
  while (getline(ss, buff, ' ')) {
    ret = true;
    if (i != 0) {
      vec.push_back(stoul(buff));
    } else {
      *first = stoul(buff);
    }
    ++i;
  }
  return ret;
}

void readSet(std::istream &data_file, uint size, int **sparse_indice,
             float **sparse_dist, int **sparse_marker) {
  vector<vector<size_t>> *data = new vector<vector<size_t>>(size);
  size_t array_length = 0;
  vector<size_t> row = vector<size_t>(0);
  size_t index;
  size_t i = 0;
  while (load_vector_from_stream_int(data_file, row, &index) && i < size) {
    data->at(i) = row;
    i++;
    array_length += row.size();
  }
  *sparse_indice = new int[array_length];
  *sparse_dist = new float[array_length];
  *sparse_marker = new int[size + 1];
  size_t current_position = 0;
  for (size_t i = 0; i < size; ++i) {
    (*sparse_marker)[i] = current_position;
    for (size_t j = 0; j < data->at(i).size(); ++j) {
      (*sparse_dist)[current_position] = 1;
      (*sparse_indice)[current_position] =
          (data->at(i).at(j) % (((size_t)1) << 31));
      ++current_position;
    }
  }
  (*sparse_marker)[size] = current_position;
  delete data;
}

void readDataAndQueries(string baseFile, uint numQuery, uint numBase,
                        int **sparse_data_indice, float **sparse_data_val,
                        int **sparse_data_marker, int **sparse_query_indice,
                        float **sparse_query_val, int **sparse_query_marker) {
  ifstream data_file(baseFile);
#if defined(SETDATASET)
  readSet(data_file, numQuery, sparse_query_indice, sparse_query_val,
          sparse_query_marker);
  readSet(data_file, numBase, sparse_data_indice, sparse_data_val,
          sparse_data_marker);
#elif defined(SPARSEDATASET)
  *sparse_query_indice = new int[NUMQUERY * DIMENSION];
  *sparse_query_val = new float[NUMQUERY * DIMENSION];
  *sparse_query_marker = new int[NUMQUERY + 1];
  readSparse(baseFile, 0, numQuery, *sparse_query_indice, *sparse_query_val,
             *sparse_query_marker, NUMQUERY * DIMENSION);

  *sparse_data_indice = new int[NUMBASE * DIMENSION];
  *sparse_data_val = new float[NUMBASE * DIMENSION];
  *sparse_data_marker = new int[NUMBASE + 1];
  readSparse(baseFile, numQuery, numBase, *sparse_data_indice, *sparse_data_val,
             *sparse_data_marker, NUMBASE * DIMENSION);

#endif
  data_file.close();
}