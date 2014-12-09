#!/bin/bash

# EXAMPLE SCRIPT TO RUN RESUB COMMAND
source grid.sh
unset extra loops regions subregions inputs
MAKE_BANK_TEST=""
if [ "$1" == "--test" ]; then
    MAKE_BANK_TEST="yes";
    shift;
fi

if [ "$1" = "" ]; then
    echo "usage: makeXXXbanks.sh [--test] command [previous_bank_DS] [runTSP_output]";
    echo "       --test option runs anly a few test jobs";
    echo "       commands are: makeBank, mergeBank, resubBank, makeDCBank, runTSP, mergeTSP, effchainTSP";
    echo "       previousDS is needed for all commands except makeBank."
    echo "                  Please set it to the resulting dataset from previous step."
    echo "       runTSP will prepare the pattern bank cache for a given configuration"
    echo "              and calculate the efficiency for that configuration."
    echo ""
    echo "Help information:"
    echo "  makeBank takes as input a baseDS dataset and all FTK config paramters and produced patterns"
    echo "  makeBank,mergeBank: Use patmergeroot=1 as extra option to use new patmergroot program"
    exit -1;
fi

COMMAND=$1;
PREVIOUS_DS=$2;
runTSP_OUTPUT=$3;
if [ "$COMMAND" != makeBank -a "$PREVIOUS_DS" = "" ]; then
    echo        previousDS is needed for all commands except makeBank.
    echo                   it is the resulting BANK dataset from previous step.
    exit -1;
fi

if [ "$COMMAND" == margeTSP -a "$runTSP_OUTPUT" = "" ]; then
    echo        runTSP_output is needed for mergeTSP.
    echo                   it is the output dataset from runTSP or effchainTSP.
    exit -1;
fi



#site=ANALY_MWT2_SL6
#site=ANALY_INFN-ROMA1
site=CERN-PROD
#site=ANALY_BNL_LONG
#site=ANALY_DESY-HH
#site=LOCAL

if [ "$FTKlibDS" != "" ]; then
    echo Running: rucio-ls -r $FTKlibDS 
    libDS_SITE=$( rucio-ls -r $FTKlibDS | grep "COMPLETE: " )
    libDS_SITEshort=`echo $libDS_SITE | sed -e "s,.*COMPLETE: ,," | sed -e "s,_.*,,"` 
    isOnSite=`echo $site | grep $libDS_SITEshort`
    if [ "$isOnSite" != "" ]; then
	echo $FTKlibDS is $libDS_SITE
	read -p "Do you want to use libDS=$FTKlibDS ? " -n 1 -r  
	echo
	if [[ $REPLY =~ ^[Yy]$ ]]; then
	    global_extra="${global_extra} -o libDS=$FTKlibDS";
	fi;
    fi;
fi;
echo global_extra="${global_extra}";

# read -p "Do you want to skip the tarball generation? " -n 1 -r
# echo
# if [[ $REPLY =~ ^[Yy]$ ]]; then
#   global_extra="${global_extra} -o skip_tarball"
#   echo Skipping tarball
# else
#   echo Remake tarball
# fi

#mc11 genaration TP equivalent
#geometry mc11 11layers (no IBL)
#NTracks="40" # in millions
#NLoops=40
#mMakeBank=16
#mFinal=4
#BASE_runstring=raw_8Lc_22x16x36_${NTracks}Mx${NLoops}loops
#baseDS=user.tompkins.ftkgeo18_base_8Lc_20130307
#AMLABEL=""
#SINGLE_MUONS="user.annovi.cmelachrFTKnoIBL.muIndepLauren.set3MWT2.20130219rewrapTrigMC_17.2.10.1.3_pixelLocalPos_20130717_094125"
#global_extra="${global_extra} -o N=64,RMAP_FILE=\${mapdir}/raw_11L.tmap"
#global_extra="${global_extra} -o UNSPLIT_SECTORS=1"

##mc11 genaration for FTK TDR (by Lauren) Lauren used 30M * 150.
##geometry mc11 11layers (no IBL)
#NTracks="50" # in millions
#NLoops=100
#mMakeBank=32
#mFinal=4
#BASE_runstring=raw_8Lc_15x16x36_${NTracks}Mx${NLoops}loops
#baseDS=user.tompkins.ftkgeo18_base_8Lc_20130307 #FTK TDR baseDS
##baseDS=user.tomoya.FTKBase_SingleMuon_64reg_8Lc_noTruthFilter_400M_20131003_ANALY_MWT2_SL6/
#AMLABEL=30x32x72
##AMLABEL=60x128x144 # For Gradon's AMsplitting test
##SINGLE_MUONS="user.cmelachr.FTKnoIBL.muIndepLauren.set3MWT2.20130219/"
#SINGLE_MUONS="user.annovi.cmelachrFTKnoIBL.muIndepLauren.set3MWT2.20130219rewrapTrigMC_17.2.10.1.3_pixelLocalPos_20130717_094125"
#global_extra="${global_extra} -o N=64,RMAP_FILE=\${mapdir}/raw_11L.tmap,PMAP_FILE=\${mapdir}/raw_8Lc.pmap,IBL_MODE=0,TFmode=trackfitter,PMAP_FILE_COMPLETE=\${mapdir}/raw_8Lc.pmap,HWSDEV_FILE=\${hwdir}/raw_8Lc.hw,NCOORDS=11" #1stStage

##mc11 11 layers (11 layer patterns) test
#NTracks="50" # in millions
#NLoops=10 #200
#mMakeBank=1 #32
#mFinal=4
#BASE_runstring=raw_11L_15x16x36_${NTracks}Mx${NLoops}loops
#baseDS=user.tompkins.ftkgeo18_base_11L_20130317
#AMLABEL=60x64x144
#global_extra="${global_extra} -o N=64,RMAP_FILE=\${mapdir}/raw_11L.tmap"
#global_extra="${global_extra} -o UNSPLIT_SECTORS=1"

###########################################################
#mc12 12 layers (with IBL) TDAQ TDR
###NTracks="50"  # in millions
###NLoops=100    # number of jobs per tower (or region)
# STS test 25.8.2014
NTracks="25"  # in millions
mSubMerge=10  # number of repetitions of patgen job (only if patmergeroot=1)
NLoops=20    # number of patgen jobs per tower (or region)
mMakeBank=32  # number of subregions at makeBank level # new patmergeroot algo can 'digest' large pattern files (was: 32)
mFinal=4      # number of subregions after resubBank
patmergeroot=1 # enable new merging procedure using root files
BASE_runstring=raw_8LcIbl123_15x16x36Ibl_${NTracks}Mx${NLoops}loops 
#AMLABEL=60x128x144Ibl
#AMLABEL=60x128x72Ibl
#AMLABEL=60x64x144Ibl 
#AMLABEL=60x64x72Ibl
#AMLABEL=30x32x368LbIbl
AMLABEL=30x32x72Ibl # default

#baseDS=user.annovi.tdaqtdr_base_8LcIbl123_12LIbl_20130807v2/ #TDAQ TDR baseDS
baseDS=user.tomoya.FTKBase_SingleMuon_64reg_8Lextfrom12L_multi_400MTracks_20131206_ANALY_MWT2_SL6/ #FTK note baseDS
##baseDS=user.annovi.tomoya_phchang_multi_400MTracks_64reg.CONST_FTK.20140113cleanV3/ # FTK note baseDS: clean for production
##bankDS=user.tomoya.raw_8LcIbl123_15x16x36Ibl_mc12IBL_40Mx125loops_autotuneFixv2_Region0to63_4NSubs_400MTracks_201301218/ # before makeDCBank
##bankDS=user.tompkins.raw_8LcIbl123_15x16x36Ibl_50Mx100loops_Region0to63_4NSubs_AM30x32x72Ibl.2014_07_14/ # not cached remade (here just for reference)
SINGLE_MUONS="user.johnda.100kTestMuonsAfterFix_Aug_16_13"
global_extra="${global_extra} -o UNSPLIT_SECTORS=1"

global_extra="${global_extra} -o N=64,RMAP_FILE=\${mapdir}/raw_12Libl.tmap,PMAP_FILE=\${mapdir}/raw_8LcIbl123.pmap,IBL_MODE=1"

# 1stStage:
global_extra="${global_extra} -o TFmode=trackfitter,PMAP_FILE_COMPLETE=\${mapdir}/raw_8LcIbl123.pmap,HWSDEV_FILE=\${hwdir}/raw_8LcIbl123.hw,NCOORDS=11"
# 2nd stage:
#global_extra="${global_extra} -o RFmode=roadfindertsp,TFmode=trackfitter711,MULTI_CONN_MODE=1,NCONN=4,PMAP_FILE_COMPLETE=\${mapdir}/raw_12Libl.pmap,HWSDEV_FILE=\${hwdir}/raw_12L.hw,SS_FILE_UNUSED=\${mapdir}/../ss_file/raw_8LcIBL123_i8s4_unused.ss,SS_FILE_COMPLETE=\${mapdir}/../ss_file/raw_8LcIBL123_i8s4_unused.ss"

#TestVector options July 2014
##baseDS=user.tomoya.FTKBase_1M_SingleMuon_64reg_8L_20140612_v1_ANALY_MWT2_SL6/ #TestVector test July2014 OBSOLETE
#baseDS=user.tomoya.FTKBase_SingleMuon_8L_25MTracks_20140717_v2_ANALY_MWT2_SL6/ #TestVector test July2014
#global_extra="${global_extra} -o HWMODEID=2,PCONST_OVERLAP=0,MODULE_LUT=\${mapdir}/raw_8Lc123test.moduleidmap"
##BASE_runstring=raw_8LcIbl123_15x16x36Ibl_${NTracks}Mx${NLoops}loops_DESYmerge
##BASE_runstring=raw_8LcIbl123_15x16x36IblAntonio_${NTracks}Mx${NLoops}loops_DESYmerge
#global_extra="${global_extra} -o RMAP_FILE=\${mapdir}/raw_12Libl_HWMODEID2.tmap"

global_extra="${global_extra} --express "

global_extra="${global_extra} -o UNSPLIT_SECTORS=1"
if [ ${patmergeroot} == 1 ] ; then
    global_extra="${global_extra} -o PATMERGEROOT=1"
fi
###########################################################


if [  "$username" == "sschmitt" ]; then
#run test at DESY
    site=ANALY_DESY-HH
    N=1
    regs=`echo {0..63}`
    regsEffchain=0..63
    official="user";
#full bank generation
elif [ "$MAKE_BANK_TEST" == "" ]; then
    echo Running full production;
    N=16
    regs=`echo {0..3}`
    regsEffchain=0..63
    #official="trig-daq"; # to be uncommented by "power users" that want to use the official role
else
    echo Running test job;
    N=1
    NLoops=5   # number of jobs per tower (or region)
    mMakeBank=1 # number of subregions at makeBank level
    #regs for bank production
    regs="45" #"0 16 45"
#    regs=`echo {0..3}`
    #regs for simulation
    regsEffchain="0..63"
    #regsEffchain="0,32"
    official="user";
fi

for i in ${regs}; do
#### Decreasing the number of subregions used in a bank
   unset extra loops regions subregions inputs
   if [ ${patmergeroot} == 1 ] ; then
# number of repetitions of patgen, merged to a single root file
       m=$mSubMerge
   else
# splitting in subregions, old procedure
       m=$mMakeBank
   fi
   runstring=${BASE_runstring}_Region$((i*N))to$((i*N+N-1))
   regions="$((i*N))..$((i*N+N-1))"
   n=${NTracks}000000      # number of iterations per per-region loop 
   loops="1..${NLoops}"    # number of per-region submission loops
   #extra="-o PTMIN=8000,SPOTR=11"
   if [ "$COMMAND" = makeBank ]; then
       echo makeBank: $runstring
       makeBank
   fi
done

# Now create a DS container that contains all the output datasets from makeBank using the command below.
# It will be the input for the next step.
#
# ../standalone_scripts/makeBankContainer.py common_part_of_the_dataset_name
#
# The makeBankContainer.py will ask for confirmation.
# Please check that all and only the needed dataset are being used.

for i in ${regs}; do
#### Decreasing the number of subregions used in a bank
   unset extra loops regions subregions inputs factor
   if [ ${patmergeroot} == 1 ] ; then
# new root file procedure requires only one merge job per region
       m=1
# use $factor to transmit information about the final number of subregions
       factor=${mFinal}
   else
# number of subregions
       m=$mMakeBank                     # nsubregions in final bank
   fi
   runstring=${BASE_runstring}_Region$((i*N))to$((i*N+N-1))
   regions="$((i*N))..$((i*N+N-1))"

   # put you DS name here
   #ftkDS=user.annovi.raw_8Lc_15x16x36_100Mx50loops_Region0to1_50NLoops_unmerged.2013_06_11_10_11_02/
   #ftkDS=user.annovi.raw_8LcIblEXPtoIBL_15x16x36Ibl_50Mx100loops_100NLoops_unmerged/
   #ftkDS=user.annovi.raw_11L_15x16x36_50Mx200loops_200NLoops/
   ftkDS=$PREVIOUS_DS;

   extra="-o MINCOVERAGE=2"
   if [ "$COMMAND" = "mergeBank" ]; then
       if [ ${patmergeroot} == 1 ] ; then
           unset baseDS
           echo mergeBankRoot: $regions \( $runstring \) 
               mergeBankRoot;
       else
           echo mergeBank: $regions \( $runstring \) 
               mergeBank;
       fi
   fi
done

# Again, now you need to create a DS container as described above.

for i in ${regs}; do
#### Decreasing the number of subregions used in a bank
   unset extra loops regions subregions inputs
   m=$mFinal                     # nsubregions in final bank
   factor=$(( $mMakeBank / $mFinal ))
   MAXPATTERNS=99999999999
   runstring=${BASE_runstring}_Region$((i*N))to$((i*N+N-1))
   regions="$((i*N))..$((i*N+N-1))"
   subregions="0..`expr ${m} - 1`"

   #put your dataset name here
   bankDS=user.annovi.raw_8LcIblEXPtoIBL_15x16x36Ibl_50Mx100loops_32NSubs/
   bankDS=user.annovi.raw_11L_15x16x36_50Mx200loops_Region8to11and24to27_64NSubs_20130821/
   bankDS=user.annovi.raw_8Lc_22x16x36_40Mx40loops_Region0to63_16NSubs.2013_09_06_23_34_53/
   bankDS=$PREVIOUS_DS;

   extra=""
   if [ "$COMMAND" = resubBank ]; then
       echo resubBank: $regions \( $runstring \)
       resubBank
   fi
done

# Again, now you need to create a DS container as described above.

for i in ${regs}; do
#### Decreasing the number of subregions used in a bank
   unset extra loops regions subregions inputs
   m=$mFinal                     # nsubregions in final bank
   MAXPATTERNS=99999999999
   runstring=${BASE_runstring}_Region$((i*N))to$((i*N+N-1))
   regions="$((i*N))..$((i*N+N-1))"
   subregions="0..`expr ${m} - 1`"

   #put your dataset here
   bankDS=user.annovi.raw_8Lc_15x16x36_100Mx50loops_Region0to1_4NSubs.2013_06_12_12_30_41/
   bankDS=user.annovi.raw_8LcIblEXPtoIBL_15x16x36Ibl_50Mx100loops_4NSubs/
   bankDS=user.annovi.raw_8Lc_15x16x36_Waseda_4NSubs_20130801/ #from Waseda
   bankDS=user.annovi.raw_11L_15x16x36_50Mx200loops_Region8to11_4NSubs.2013_08_21_16_06_44/
   # TDAQ TDR
   bankDS=user.annovi.raw_8LcIbl123_15x16x36Ibl_mc12IBL_40Mx125loops_autotuneFix_4NSubs.2013_08_26_08_27_00/ 
   bankDS=$PREVIOUS_DS;
   extra="-o amlabel=${AMLABEL},MINCOVERAGE=2"
   if [ "$COMMAND" = makeDCBank ]; then
       echo makeDCBank: $regions \( $runstring \)
       makeDCBank
   fi
done

# Again, now you need to create a DS container as described above.

############################ Effchain TSP run ################################
unset extra loops regions subregions inputs
bankDS=$PREVIOUS_DS;
m=$mFinal                     # nsubregions in final bank
MAXPATTERNS=99999999999
inputs="0..19"
INPUT="-n 100000 --nFilesPerJob 1 --nJobs 1 --inDS $SINGLE_MUONS --express "'--match *.dat*' # input dataset, file range to process, and nevents:
regions=$regsEffchain

#after resubBank
#extra="-o amlabel=30x32x72,RFmode=roadfinder,N=64,RMAP_FILE=\${mapdir}/raw_11L.tmap"
extra="-o RFmode=roadfinder"
extra="${extra} -o MAX_MISSING_SCT_PAIRS=1,RESTRICT_SCT_PAIR_LAYER=1"
runstring=${BASE_runstring}_indepmuon
#bankDS=user.annovi.raw_8Lc_22x16x36_40Mx40loops_Region0to63_4NSubs.2013_09_07_09_02_39/
#echo effchainTSP AM only: $regions \( $runstring \)
#effchainTSP

#### effcurve run
ftkDS=user.annovi.raw_8Lc_11x12x18_indepmuon.ftk_eff.2013_04_26_18_10_05/
extra=""
#echo effcurveTSP: $regions \( $runstring \)
#effcurveTSP

#after makeDCBank
unset runstring bankDS extra
bankDS=$PREVIOUS_DS;
runstring=${BASE_runstring}_AM${AMLABEL}_effchainAMDC
extra="-o amlabel=${AMLABEL},RFmode=roadfindertsp,USETSP_SIM=2,USETSP_SETAMSIZE=2,RFmode=roadfindertsp"
#",TFmode=trackfitter711"

#extra="${extra} -o UNSPLIT_SECTORS=1"
if [ "$COMMAND" = effchainTSP ]; then
    echo effchainTSP with DC: $regions \( $runstring \)
    effchainTSP
fi

#### effcurve run
ftkDS=$PREVIOUS_DS;
extra=""
#echo effcurveTSP: $regions \( $runstring \)
#effcurveTSP

#after makeDCBank:
#  prepare the pattern bank cache for a given # of patterns and AM size (aka max # of DC bits/layer)
#  and calculate the efficiency for the configuration associated to the cache
unset runstring bankDS extra
bankDS=$PREVIOUS_DS;
runstring=${BASE_runstring}_AM${AMLABEL}_effCache
extra="-o amlabel=${AMLABEL},RFmode=roadfindertsp,USETSP_SIM=2,USETSP_SETAMSIZE=2" #",TFmode=trackfitter711"
extra="${extra} -o MAKECACHE=1"
MAXPATTERNS=4194304
inputs="0"
if [ "$COMMAND" = runTSP ]; then
    echo runTSP with DC for efficiency and cache: $regions \( $runstring \)
   runTSP
fi

#### effcurve run
bankDS=$PREVIOUS_DS;
ftkDS=$runTSP_OUTPUT;
extra=""
if [ "$COMMAND" = mergeTSP ]; then
    echo mergeTSP for efficiency: $regions \( $runstring \)
   mergeTSP
fi


