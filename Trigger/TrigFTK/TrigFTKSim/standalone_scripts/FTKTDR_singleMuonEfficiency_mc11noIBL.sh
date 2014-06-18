#!/bin/bash

#####################################################################
#
#
# Grid Job submission script for FTK simulation
#
#
#####################################################################

# Basic environment setting.
source grid.sh # this script defines all the functions and the variable used but not the declared in this script
unset extra loops regions subregions inputs

# -------------------------------------------------------------------
#   This script sets basic environment variables. and runs a single function
#   called. "runTSP". Things that are being set in this script are,
#     runstring   - This current simulation run name.
#     INPUT       - Input data set that will run the simulation on.
#     baseDS      - Base dataset contains constants and sectors which are 
#                   used in the trackfitting and many more.
#     bankDS      - Bank dataset contains the pattern banks, which will be 
#                   used to simulate "pattern matching" in the first stage.
#     inputs      - Sets how many files to run over in the INPUT
#     site        - Grid site to submit to.
#     MAXPATTERNS - Sets the max number of patterns to use during the
#                   pattern matching stage.
#     regions     - Defines which regions Out of 64 regions the simulation 
#                   will run.
#     m           - For each regions, there are 'm' number of sub-regions.
#                   This variable defines the number of sub-regions.
#                   This value has to be compatiable with the data sets one
#                   is using.
#     extra       - This is the most complicated part where it defines 
#                   various options. If unsure, contact one of the FTK experts.

#global_extra="${global_extra} -o skip_tarball"
#2nd stage only
global_extra="${global_extra} -o N=64,RMAP_FILE=\${mapdir}/raw_11L.tmap,RFmode=roadfindertsp,TFmode=trackfitter711,NCOORDS=14,PMAP_FILE_COMPLETE=\${mapdir}/raw_11L.pmap,HWSDEV_FILE=\${hwdir}/raw_11L.hw"
#1st stage only
#global_extra="${global_extra} -o N=64,RMAP_FILE=\${mapdir}/raw_11L.tmap,RFmode=roadfindertsp,TFmode=trackfitter,NCOORDS=11,PMAP_FILE_COMPLETE=\${mapdir}/raw_8Lc.pmap,HWSDEV_FILE=\${hwdir}/raw_8Lc.hw"

#official="trig-daq"
site=ANALY_MWT2_SL6
#site=CERN-PROD

#regions="0..1"                          # which regions to generate
regions="0..63"                          # which regions to generate
m=4   # number of subregions, this should be compatible with the bank!
baseDS=user.phchang.FTK_geo18_2nd_stage_baseDS_8Lc_20130318
bankDS=user.tompkins.trig-daq.raw_8Lc_15x16x36_geo18_FullDet_4NSubs_AM30x32x72.2013_03_11/
inputs="0..9"

MAXPATTERNS=4194304
extra="${extra} -o amlabel=30x32x72,RAW_MODE=2,USETSP_SIM=2,TRACKFITTER_MODE=1,USETSP_SETAMSIZE=2,SAVE_INCOMPLETE=1,SAVE_CLUSTER_CONTENT=1 " 
#extra="${extra} -o BMAP=badmodule_8Lc_V18_mc11,BMAP_FOR_HIT=badmodule_8Lc_V18_mc11 " # adding bad module list
extra="${extra} -o MAX_MISSING_SCT_PAIRS=1,RESTRICT_SCT_PAIR_LAYER=1"
runstring=raw_8Lc_15x16x36_mc11indepMuonsLocalPos_100k_2ndStage
#extra="${extra} -o PIXEL_CLUSTERING_RENUMBERING=5"

#INPUT="-n 100000 --nFilesPerJob 1 --inDS user.annovi.cmelachrFTKnoIBL.muIndepLauren.set3MWT2.20130219rewrap "'--match *.dat*'
#INPUT="-n 100000 --nFilesPerJob 1 --inDS user.annovi.cmelachrFTKnoIBL.muIndepLauren.set3MWT2.20130219rewrapTrigMC_17.2.10.1.3_20130717_094800 "'--match *.dat*'
INPUT="-n 100000 --nFilesPerJob 1 --inDS user.annovi.cmelachrFTKnoIBL.muIndepLauren.set3MWT2.20130219rewrapTrigMC_17.2.10.1.3_pixelLocalPos_20130717_094125 "'--match *.dat*'

# Call the submission function.
INPUT="${INPUT} --express"
runTSP

# Merge
unset regions
#INPUT="${INPUT} --express"
#extra="${extra} -o DO_NOT_MERGE_ROADS"
#extra="${extra} -o EXPLORE_FTK"

#last tries 10kmu
ftkDS=
#mergeTSP

ftkDS=
#exploreFTK
