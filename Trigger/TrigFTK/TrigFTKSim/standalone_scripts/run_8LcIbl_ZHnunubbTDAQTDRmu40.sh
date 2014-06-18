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
    read -p "Do you want to use libDS=$FTKlibDS ? " -n 1 -r  
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        global_extra="${global_extra} -o libDS=$FTKlibDS";
    fi;
fi;
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
# 1stStage
global_extra="${global_extra} -o N=64,RMAP_FILE=\${mapdir}/raw_12Libl.tmap,PMAP_FILE=\${mapdir}/raw_8LcIbl123.pmap,IBL_MODE=1,RFmode=roadfindertsp,TFmode=trackfitter,PMAP_FILE_COMPLETE=\${mapdir}/raw_8LcIbl123.pmap,HWSDEV_FILE=\${hwdir}/raw_8LcIbl123.hw,NCOORDS=11"
runstring=raw_8LcIbl123_15x16x36Ibl_mc12fixMuons_130kmu_1stStage
#2nd stage
#global_extra="${global_extra} -o TRACKFITTER_MODE=3,N=64,RMAP_FILE=\${mapdir}/raw_12Libl.tmap,PMAP_FILE=\${mapdir}/raw_8LcIbl123.pmap,IBL_MODE=1,RFmode=roadfindertsp,TFmode=trackfitter711,MULTI_CONN_MODE=1,NCONN=20,PMAP_FILE_COMPLETE=\${mapdir}/raw_12Libl.pmap,HWSDEV_FILE=\${hwdir}/raw_12L.hw,SS_FILE_UNUSED=\${mapdir}/../ss_file/raw_raw_8LcIBL123_unused24x24.ss,SS_FILE_COMPLETE=\${mapdir}/../ss_file/raw_raw_8LcIBL123_unused24x24.ss" #raw_8LcIBL123_i8s4_unused.ss"

#official="trig-daq"
site=ANALY_MWT2_SL6
#site=CERN-PROD
#site=ANALY_INFN-FRASCATI
#site=ANALY_BNL_LONG

INPUT="-n 5000 --nFilesPerJob 1 --inDS=user.annovi.mc12_14TeV.161849.Pythia8_AU2CTEQ6L1_ZH125_nunubb.recon.TXT_FTKIP.e2103_s1682_s1691_r4710_IBLwrap/ "'--match *.dat*'
#INPUT="${INPUT} --maxCpuCount 86400"

#runstring=raw_8LcIbl123_15x16x36Ibl_ZHnunubbTDAQTDRmu40
regions="0..63"                          # which regions to generate
#regions="10"
m=4   # number of subregions, this should be compatible with the bank!
baseDS=user.annovi.tdaqtdr_base_8LcIbl123_12LIbl_20130807v2/
bankDS=user.annovi.raw_8LcIbl123_15x16x36Ibl_mc12IBL_40Mx125loops_autotuneFix_4NSubs_AM30x32x72Ibl.2013_08_26_11_15_00/
inputs="0..5" #inputs="0..39"
inputs="0..9"
MAXPATTERNS=4194304
extra="${extra} -o amlabel=30x32x72Ibl,RAW_MODE=2,USETSP_SIM=2,TRACKFITTER_MODE=3,USETSP_SETAMSIZE=2,SAVE_INCOMPLETE=1 "
extra="${extra} -o MAX_MISSING_SCT_PAIRS=1,RESTRICT_SCT_PAIR_LAYER=1"
extra="${extra} -o PIXEL_CLUSTERING_MODE=1"
extra="${extra} -o DO_NOT_MERGE_ROADS"
#extra="${extra} -o BMAP=badmodule_8Lc_V18_mc11,BMAP_FOR_HIT=badmodule_8Lc_V18_mc11 " # adding bad module list

# Call the submission function.
runTSP

# Merge
#unset regions
#INPUT="${INPUT} --express"
#extra="${extra} -o DO_NOT_MERGE_ROADS"
ftkDS=user.annovi.raw_8LcIbl123_15x16x36Ibl_ZHnunubbTDAQTDRmu40.ftk.2013_11_12_18_57_16/ #1 file
ftkDS=user.annovi.raw_8LcIbl123_15x16x36Ibl_ZHnunubbTDAQTDRmu40.ftk.2013_11_13_00_59_52/ #10 files
#extra="${extra} -o EXPLORE_FTK"
#mergeTSP

#ftkDS=user.annovi.raw_8LcIbl123_15x16x36Ibl_ZHnunubbTDAQTDRmu40.merge_ftk.2013_11_14_15_27_12/
#exploreFTK
