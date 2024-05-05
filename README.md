# Practical Near Neighbour Search using Group Testing

## Data

Use the `download_data.py` script to download the dataset into the `raw` directory. The script will setup the following directory structure:

```
.
├── data
│   ├── genomes
│   │   ├── data
│   │   └── indices
│   ├── promethion
│   │   ├── data
│   │   └── indices
│   ├── proteomes
│   │   ├── data
│   │   └── indices
│   ├── url
│   │   ├── data
│   │   └── indices
│   ├── webspam
│   │   ├── data
│   │   └── indices
│   └── yfcc
│       ├── indices
│       └── queries
├── raw
│   ├── genomes-data.gz
│   ├── promethion-data.gz
│   ├── proteomes-data.gz
│   ├── url_combined.bz2
│   └── webspam_wc_normalized_trigram.svm.xz
```

## Paper

[Arxiv](https://arxiv.org/pdf/2106.11565)

## Running the code

Select the dataset you want to run the code on in the `benchmark.h` file. Then run `make`, and finally
```bash
./runme > flinng_proteomes.txt
```

> The Promethion and the WebSpam databases are too large, and will require either extensive harder or manually configured swap space to run.

## Generating the graphs

```bash
cd analysis && ./generate-graphs
```