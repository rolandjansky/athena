#!/bin/bash

#####################################################################
#
#
# Grid Job submission script for FTK simulation
#
#
#####################################################################

# SVN information that this script was used

# Path: .
# URL: svn+ssh://phchang@svn.cern.ch/reps/atlasoff/Trigger/TrigFTK/TrigFTKSim/branches/TrigFTKSim-00-09-09-branch
# Repository Root: svn+ssh://phchang@svn.cern.ch/reps/atlasoff
# Repository UUID: 4525493e-7705-40b1-a816-d608a930855b
# Revision: 569582
# Node Kind: directory
# Schedule: normal
# Last Changed Author: gvolpi
# Last Changed Rev: 569568
# Last Changed Date: 2013-11-10 04:08:03 -0600 (Sun, 10 Nov 2013)

# Basic environment setting.
source grid.sh # this script defines all the functions and the variable used but not the declared in this script
unset extra loops regions subregions inputs

# -------------------------------------------------------------------
#     runstring   - This current simulation run name.
#     INPUT       - Input data set that will run the simulation on.
#     inputs      - Sets how many files to run over in the INPUT
#     baseDS      - Base dataset contains constants and sectors which are 
#                   used in the trackfitting and many more.
#     bankDS      - Bank dataset contains the pattern banks, which will be 
#                   used to simulate "pattern matching" in the first stage.
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
# -------------------------------------------------------------------

#runstring=raw_8LcIbl123_15x16x36Ibl_singleMuons_pubnoteFC1deadModules
#runstring=raw_8LcIbl123_15x16x36Ibl_singleMuons_pubnote400MTrkTraining_2ndStage10kmu  #130kmu
#INPUT="-n 100000 --nFilesPerJob 2 --inDS user.johnda.100kTestMuonsAfterFix_Aug_16_13 "'--match *.dat*'
runstring=raw_8LcIbl123_15x16x36Ibl_singlePions_pubnote400MTrkTraining_test
INPUT="-n 100000 --nFilesPerJob 2 --inDS user.tompkins.ibl_singlePions_tdaqtdrconfig_20130913 "'--match *.dat*'
inputs="0" #..25"
#baseDS=user.tomoya.FTKBase_SingleMuon_64reg_8Lextfrom12L_multi_400MTracks_20131206_ANALY_MWT2_SL6/ #8L only
baseDS=user.annovi.tomoya_phchang_multi_400MTracks_64reg.CONST_FTK.20140113cleanV3/ # clean for production
#bankDS=user.tomoya.raw_8LcIbl123_15x16x36Ibl_mc12IBL_40Mx125loops_autotuneFixv2_Region0to63_4NSubs_400MTracks_201301218/ # before makeDCBank
#bankDS=user.tomoya.raw_8LcIbl123_15x16x36Ibl_mc12IBL_Region0to63_4NSubs_AM30x32x72Ibl_multimuons_400MTracks_20131209/ # not cached (lost as of July 2014)
#bankDS= TO BE FILLED WITH REPLACEMENT # not cached
#bankDS=user.annovi.raw_8LcIbl123_15x16x36Ibl_singleMuons_pubnote400MTrkTraining.ftk.2013_12_12_16_31_24/ #with cache
bankDS=user.annovi.raw_8LcIbl123_15x16x36Ibl_singleMuons_pubnote400MTrkTraining.ftk.2013_12_12_16_31_24clean #with cache and cleaned

site=ANALY_MWT2_SL6
#site=CERN-PROD
#site=ANALY_INFN-ROMA1
MAXPATTERNS=4194304
regions="0" #..63"
m=4

# global extra options
global_extra="${global_extra} -o N=64"
global_extra="${global_extra} -o RMAP_FILE=\${mapdir}/raw_12Libl.tmap"
global_extra="${global_extra} -o PMAP_FILE=\${mapdir}/raw_8LcIbl123.pmap"
global_extra="${global_extra} -o RFmode=roadfindertsp"

#1st stage
#global_extra="${global_extra} -o TFmode=trackfitter"
#global_extra="${global_extra} -o PMAP_FILE_COMPLETE=\${mapdir}/raw_8LcIbl123.pmap"
#global_extra="${global_extra} -o HWSDEV_FILE=\${hwdir}/raw_8LcIbl123.hw"
#extra="${extra} -o NCOORDS=11"

#2nd stage
global_extra="${global_extra} -o TFmode=trackfitter711"
global_extra="${global_extra} -o PMAP_FILE_COMPLETE=\${mapdir}/raw_12Libl.pmap"
global_extra="${global_extra} -o HWSDEV_FILE=\${hwdir}/raw_12L.hw"
global_extra="${global_extra} -o SS_FILE_UNUSED=\${mapdir}/../ss_file/raw_8LcIBL123_unusedmedium.ss"
global_extra="${global_extra} -o SS_FILE_COMPLETE=\${mapdir}/../ss_file/raw_8LcIBL123_unusedmedium.ss"
extra="${extra} -o NCOORDS=16"

# not used by Alberto for TDAQ TDR simulations:
extra="${extra} -o RESTRICT_SCT_PAIR_MODULE=1"                                            # RestrictSctPairModule 

# extra options
extra="${extra} -o amlabel=30x32x72Ibl"
extra="${extra} -o RAW_MODE=2"
extra="${extra} -o SAVE_INCOMPLETE=1"
extra="${extra} -o RESTRICT_SCT_PAIR_LAYER=1"                                             # RestrictSctPairLayer
extra="${extra} -o MAX_MISSING_SCT_PAIRS=1"                                               # MaxMissingSCTPairs
extra="${extra} -o USETSP_SIM=2"                                                          # TSPSimulationLevel
extra="${extra} -o NCONN=4"                                                               # SSFNConnections
extra="${extra} -o MULTI_CONN_MODE=1"                                                     # SSFMultiConnection
extra="${extra} -o TRACKFITTER_MODE=3"                                                    # TRACKFITTER_MODE
extra="${extra} -o USETSP_SETAMSIZE=2"                                                    # SetAMSize
extra="${extra} -o IBL_MODE=1"                                                            # IBLMode=1
extra="${extra} -o PIXEL_CLUSTERING_MODE=1"                                               # PixelClusteringMode=1
extra="${extra} -o SCT_CLUSTERING=0"                                                      # SctClustering=0
extra="${extra} -o ALLOW_EXTRA_MISS_IN_SUPEREXP=1"                                        # SSFAllowExtraMiss=1
#extra="${extra} -o USETSP_MINCOVERAGE=0"                                                  # TSPMinCoverage 
#extra="${extra} -o USETSP_DBLEVEL=1"                                                      # DBBankLevel
#extra="${extra} -o USETSP_BANK=1"                                                         # UseTSPBank
#extra="${extra} -o MAX_TRANSITION_ETA=1.4"                                                # SSFTRMaxEta
#extra="${extra} -o MIN_TRANSITION_ETA=1.0"                                                # SSFTRMinEta
#extra="${extra} -o CHECK_TRANSITION_BY=1"                                                 # SSFTRDefn
#extra="${extra} -o MAKECACHE=1,_PATTDB=patt.root"
extra="${extra} -o CACHEDBANK=1,_PATTDB=pcache.root"
#not to be used here extra="${extra} -o BMAP=badmodule_8LcIbl123_mc12Ibl "
#extra="${extra} -o BMAP_FOR_HIT=badmodule_8LcIbl123_mc12Ibl "
#extra="${extra} -o SECTORSASPATTERNS=1,UNSPLIT_SECTORS=1"
extra="${extra} -o UNSPLIT_SECTORS=1"

# Run simulation
runTSP

# Merge
unset regions
ftkDS=user.annovi.raw_8LcIbl123_15x16x36Ibl_singleMuons_pubnote400MTrkTraining_2ndStage10kmu.ftk.2014_01_18_21_02_49/
ftkDS=user.annovi.raw_8LcIbl123_15x16x36Ibl_singlePions_pubnote400MTrkTraining_2ndStage_130k.ftk.2014_01_21_13_13_38/
#mergeTSP

global_extra="${global_extra} -o ANAmode=effstudy,ANA_IGNORE_ROADS=0"
ftkDS=user.annovi.raw_8LcIbl123_15x16x36Ibl_singlePions_pubnote400MTrkTraining_2ndStage_130k.merge_ftk.2014_01_21_21_46_02/
#analysisTSP   

#ftkDS=
#exploreFTK
