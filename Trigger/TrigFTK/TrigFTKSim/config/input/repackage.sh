#!/bin/bash

# SAMPLE script to break a list of single muons into per-region files
# This allows to run pattgen/corrgen using only muons that belong to that region
# This script also effectively removes all events that fail filtering cuts

# NOTE: you need a region lookup file for the entire sample (region_${label}.dat)
# This file can be constructed by running "-a oldsectors" with PRINT_CYREG=1 using 11L architecture

label=$1  # BEAM
export LD_PRELOAD=/opt/d-cache/dcap/lib/libpdcap.so
(for f in `cat /home/antonk/jan/TrigFTKSim/config/input/inputprod_raw_${label}.list`; do cat $f; done) | ./repackage.py /share/ftkdata/antonk/bases/v14_${label}/lookup/region_${label}.dat single_muon_${label}_reg%d_file%.3d.dat /pnfs/uct3/data/users/antonk/MUONS/BEAM5GEV/PERREGION/ /scratch/antonk/
echo "DONE"
