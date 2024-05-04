
#ifndef EVALUATE_H
#define EVALUATE_H

#include <algorithm>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <unordered_set>

void evaluate(unsigned int *queryOutputs, int numQueries, int topk,
              unsigned int *groundTruthIdx, int availableTopk);

void rMetric(unsigned int *queryOutputs, int numQueries, int topk,
             unsigned int *groundTruthIdx, int availableTopk, int numerator);

#endif /* EVALUATE_H */
