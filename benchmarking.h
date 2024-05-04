// Select a dataset below by uncommenting it.

// #define URL
// #define WEBSPAM_TRI
// #define DNA_FULL_GENOME
// #define PROMETHION_SHORT
#define DNA_FULL_PROTEOME

// --------------------------------

/* Parameters. */
#if defined URL

#define SPARSEDATASET

#define NUMHASHBATCH 200

#define K 4
#define RANGE 17

#define DIMENSION 120
#define FULL_DIMENSION 3231961

#define NUMBASE 2386130
#define NUMQUERY 10000
#define AVAILABLE_TOPK 1024
#define TOPK 128

#define BASEFILE "data/url/data"
#define GTRUTHINDICE "data/url/indices"

#elif defined WEBSPAM_TRI

#define SPARSEDATASET

#define NUMHASHBATCH 50

#define K 4
#define RANGE 18

#define DIMENSION 4000
#define FULL_DIMENSION 16609143

#define NUMBASE 340000
#define NUMQUERY 10000
#define AVAILABLE_TOPK 1024
#define TOPK 128

#define BASEFILE "data/webspam/data"
#define GTRUTHINDICE "data/webspam/indices"

#elif defined DNA_FULL_GENOME

#define SETDATASET

#define NUMHASHBATCH 50

#define K 1
#define RANGE 17

#define NUMBASE 117219
#define NUMQUERY 10000
#define TOPK 128
#define AVAILABLE_TOPK 128

#define BASEFILE "data/genomes/data"
#define GTRUTHINDICE "data/genomes/indices"

#elif defined DNA_FULL_PROTEOME

#define SETDATASET

#define NUMHASHBATCH 50

#define K 1
#define RANGE 17

#define NUMBASE 116373
#define NUMQUERY 10000
#define TOPK 128
#define AVAILABLE_TOPK 128

#define BASEFILE "data/proteomes/data"
#define GTRUTHINDICE "data/proteomes/indices"

#elif defined PROMETHION_SHORT

#define SETDATASET

#define NUMHASHBATCH 200

#define K 1
#define RANGE 17

#define NUMBASE 3696341
#define NUMQUERY 10000
#define TOPK 128
#define AVAILABLE_TOPK 128

#define BASEFILE "data/promethion/data"
#define GTRUTHINDICE "data/promethion/indices"

#endif

#define STARTREPS (1 << 2)
#define MAXREPS (1 << 11)
#define REPRATIO 2
#define BRATIO 2

#define STARTB (1 << 11)
#define ENDB (1 << 15)
#define STARTR 2
#define ENDR 4

// To avoid annoying compile time errors
#ifndef DIMENSION
#define DIMENSION 0
#endif
