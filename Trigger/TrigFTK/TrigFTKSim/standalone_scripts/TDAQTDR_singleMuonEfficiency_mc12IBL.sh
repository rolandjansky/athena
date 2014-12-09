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
if [ "$FTKlibDS" != "" ]; then
    global_extra="${global_extra} -o libDS=$FTKlibDS";
fi
echo global_extra="${global_extra}";

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
#global_extra="${global_extra} -o N=64,RMAP_FILE=\${mapdir}/raw_11L.tmap,RFmode=roadfindertsp,TFmode=trackfitter711"
#global_extra="${global_extra} -o N=64,RMAP_FILE=\${mapdir}/raw_11L.tmap,RFmode=roadfindertsp,TFmode=trackfitter"

#official="trig-daq"
site=ANALY_MWT2_SL6
#site=CERN-PROD

regions="0..63"                          # which regions to generate
m=4   # number of subregions, this should be compatible with the bank!
baseDS=user.annovi.tdaqtdr_base_8LcIbl123_12LIbl_20130807v2/
baseDS=user.annovi.tomoya_phchang_multi_400MTracks_64reg.CONST_FTK.20140113cleanV3

inputs="0..12" #inputs="0..39"
MAXPATTERNS=4194304
bankDS=user.annovi.raw_8LcIbl123_15x16x36Ibl_mc12IBL_40Mx125loops_autotuneFix_4NSubs_AM30x32x72Ibl.2013_08_26_11_15_00/
bankDS=user.annovi.raw_8LcIbl123_15x16x36Ibl_singleMuons_pubnote400MTrkTraining.ftk.2013_12_12_16_31_24clean
extra="${extra} -o amlabel=30x32x72Ibl,RAW_MODE=2,USETSP_SIM=2,TRACKFITTER_MODE=3,USETSP_SETAMSIZE=2,SAVE_INCOMPLETE=1" #,SAVE_CLUSTER_CONTENT=1 " # First try of trackfitting

extra="$extra -o UNSPLIT_SECTORS,CACHEDBANK"

# 1stStage
#global_extra="${global_extra} -o N=64,RMAP_FILE=\${mapdir}/raw_12Libl.tmap,PMAP_FILE=\${mapdir}/raw_8LcIbl123.pmap,IBL_MODE=1,RFmode=roadfindertsp,TFmode=trackfitter,PMAP_FILE_COMPLETE=\${mapdir}/raw_8LcIbl123.pmap,HWSDEV_FILE=\${hwdir}/raw_8LcIbl123.hw,NCOORDS=11"
#runstring=raw_8LcIbl123_15x16x36Ibl_mc12fixMuons_130kmu_1stStage
#2nd stage
global_extra="${global_extra} -o TRACKFITTER_MODE=3,N=64,RMAP_FILE=\${mapdir}/raw_12Libl.tmap,PMAP_FILE=\${mapdir}/raw_8LcIbl123.pmap,IBL_MODE=1,RFmode=roadfindertsp,TFmode=trackfitter711,MULTI_CONN_MODE=1,NCONN=4,PMAP_FILE_COMPLETE=\${mapdir}/raw_12Libl.pmap,HWSDEV_FILE=\${hwdir}/raw_12L.hw,SS_FILE_UNUSED=\${mapdir}/../ss_file/raw_8LcIBL123_i8s4_unused.ss,SS_FILE_COMPLETE=\${mapdir}/../ss_file/raw_8LcIBL123_i8s4_unused.ss"
runstring=raw_8LcIbl123_15x16x36Ibl_mc12muTDAQTDR_130kmu_con4IblWin8_TFmode3

extra="${extra} -o PIXEL_CLUSTERING_MODE=1"
extra="${extra} -o MAX_MISSING_SCT_PAIRS=1,RESTRICT_SCT_PAIR_LAYER=1"
#extra="${extra} -o BMAP=badmodule_8Lc_V18_mc11,BMAP_FOR_HIT=badmodule_8Lc_V18_mc11 " # adding bad module list

INPUT="-n 100000 --nFilesPerJob 2 --inDS user.johnda.100kTestMuonsAfterFix_Aug_16_13 "'--match *.dat*'

# Call the submission function.
INPUT="${INPUT} --express"
runTSP

# Merge
unset regions
#INPUT="${INPUT} --express"
#extra="${extra} -o DO_NOT_MERGE_ROADS"
#extra="${extra} -o EXPLORE_FTK"
ftkDS=
#mergeTSP

global_extra="${global_extra} -o ANAmode=effstudy,ANA_IGNORE_ROADS=0"
ftkDS=
#analysisTSP

ftkDS=
#exploreFTK
