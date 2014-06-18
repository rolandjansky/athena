#!/bin/bash

# Sample commands to produce a complete sets of TSP or split-architecture banks

bankdir=/share/ftkdata/antonk/bases/v14_TEST
queue=pbs

# available samples:
sample=BEAMREG         # narrow beam sample (only run on region 0)
sample=V14PNFS         # default 300M muon sample (NOT region-splitted)
sample=V14PNFS1REG     # 300M gaus-z0 muon sample, split into per-region blocks
sample=V14PNFSREGALL   # 1.1B gaus-z0 muon sample, split into per-region blocks
sample=FLATZ0FIXREG    # 1.7B flat-z0 muon sample, split into per-region blocks

# action for default 11L/7L TSP banks
action=sectors,STOP,filter,STOP,corrgen,STOP,cleanup,STOP,pattgen,STOP,split

# action for default 11L/7L TSP banks (region 0 only)
#actionTSP0=sectors0,STOP,filter_0,STOP,corrgen0,STOP,cleanup0,STOP,pattgen0,STOP,split0

# action for split architecture banks (11L + 8L + 4L) - MUST BE USED WITH REGION-SPLITTED MUONS
# note that this variation decouples 4L training from 8L training by using truth hits in pseudolayer
# this is OK because we don't need 4L constants - just sectors and patterns
# NOTE 1: you can use 8L track fit results in pseudolayer by setting TRKLOOKUP_FROM_TRUTH=0,USE_TRKLOOKUP=1
# NOTE 2: you can use it with unsplitted muons, but you'll need to change MAKE_REGLOOKUP and USE_REGLOOKUP flags
#actionSPLIT=[-m=1:MAKE_REGLOOKUP=0],sectors,STOP,filter15,STOP,corrgen,STOP,cleanup,[MAKE_REGLOOKUP=0:MAKE_SECLOOKUP=1],STOP,pattgen,[MAKE_SECLOOKUP=0],STOP,split,STOP,STOP,[-m=16:-d=raw_8L_50x16x144_${sample}],sectors,STOP,filter10,STOP,corrgen,STOP,cleanup,[MAKE_SECLOOKUP=1],STOP,pattgen,[MAKE_SECLOOKUP=0],STOP,split,STOP,dumptracks,STOP,STOP,[-m=4:-d=raw_4L_15x150I150x20_${sample}:USE_TRKLOOKUP=0:TRKLOOKUP_FROM_TRUTH=1],sectors,STOP,[MAKE_SECLOOKUP=1],pattgen,STOP,[MAKE_SECLOOKUP=0],split,STOP,makelookup,[4:-7:-2]

# RUNNING WITH REGION-SPLITTED MUONS
#./ftk.py -q ${queue} -m4 -r ${bankdir} -t ${bankdir} -o FORCE_REGION,DATAFILE_LIST='${inputdir}/inputprod_raw_${dataset}${i}.list' -d raw_11L_50x64x144_${sample} -a ${action}

# RUNNING WITH UNSPLIT MUONS (deprecated - 10 times slower than the region-splitted version)
#./ftk.py -q ${queue} -m4 -r ${bankdir} -t ${bankdir} -d raw_11L_50x64x144_${sample} -a ${action}

# Running in the blayer-splitted mode
#./ftk.py -q ${queue} -m4 -o SPLIT_BLAYER_MODULES=1,PMAP_FILE='${mapdir}/raw_11L.pmap.blaysplit',RMAP_FILE='${mapdir}/raw_11L.rmap.blaysplit' -o DATAFILE_LIST='${inputdir}/inputprod_raw_${dataset}${i}.list' -r ${bankdir} -t ${bankdir} -d raw_11L_50x64x144_${sample} -a ${action}

# NOTE ON PRODUCING REGION-SPLITTED MUONS
# Once you have a list of ~300M wrapped muons, they should be split into per-region batches.
# First, you need to make a region lookup file by running 11L ftk.py with "-o MAKE_REGLOOKUP=1 -a allsectors"
# When the job finishes, it will save a region lookup file under ${bankdir}/lookup/.
# Convert that region lookup from .root to .dat format using FTKCacheLookup::dump_to_textfile
# Use this .dat region lookup file, along with unplit inputprod*.list, to run 
# TrigFTKSim/config/input/repackage.sh
