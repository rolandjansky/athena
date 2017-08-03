#!/bin/bash
display_usage() { 
    echo -e "\nUsage:\n$0 [runNumber] [dataCampaign] [stream] [refNumber] \n" 
} 
if [  $# -le 0 ] 
then 
    display_usage
    exit 1
fi 
if [ -z "$1" ]
then
    echo "No Run Number supplied"
    exit 1
fi

run=${1}
data="data16_13TeV"
stream="physics_Main"
if [ -z "$2" ]
then
    echo "No data campaign, using data16_13TeV"
else
    data=${2}
fi

if [ -z "$3" ]
then
    echo "No stream, using physics_Main"
else
    stream=${3}
fi

if [ -z "$4" ]
then
    echo "No reference run number supplied, no reference is used"
else
    refrun=${4}
fi


echo $run $data $stream $refrun
source /afs/cern.ch/project/eos/installation/atlas/etc/setup.sh

BASE=/eos/atlas/atlastier0/rucio/${data}/${stream}/00${run}
echo $BASE 
#echo eos ls ${BASE} | grep HIST.x
#echo eos ls ${BASE} | grep HIST.f

DIR1=`eos ls ${BASE} | grep HIST.x`
DIR2=`eos ls ${BASE} | grep HIST.f`
#DIR1=`eos ls ${BASE} | grep HIST.x`
#DIR2=`eos ls ${BASE} | grep HIST.f`
echo "Found $DIR1 $DIR2"

if [ -n "$DIR1" ]; then
    echo "TRY $BASE $DIR1"
    echo "${BASE}/${DIR1}" 
    FILETOCOPY1=`eos ls ${BASE}/${DIR1} | grep HIST`
    echo $FILETOCOPY1
    echo "TEST ${BASE}/${DIR1}/${FILETOCOPY1}"
    xrdcp ${BASE}/${DIR1}/${FILETOCOPY1} .
else
    echo "TRY $BASE $DIR2"
    echo "${BASE}/${DIR2}" 
    FILETOCOPY1=`eos ls ${BASE}/${DIR2} | grep HIST`
    echo $FILETOCOPY1
    echo "TEST ${BASE}/${DIR2}/${FILETOCOPY1}"
    xrdcp ${BASE}/${DIR2}/${FILETOCOPY1} .
fi    

if [ -n "$refrun" ]; then
    BASEREF=/eos/atlas/atlastier0/rucio/${data}/${stream}/00${refrun}
    echo $BASEREF 
    echo eos ls ${BASEREF} | grep HIST.x
    echo eos ls ${BASEREF} | grep HIST.f
    DIR3=`eos ls ${BASEREF} | grep HIST.x`
    DIR4=`eos ls ${BASEREF} | grep HIST.f`
    echo $DIR3 $DIR4

    if [ -n "$DIR3" ]; then
	FILETOCOPY2=`eos ls ${BASEREF}/${DIR3} | grep HIST`
	echo "TEST ${BASEREF}/${DIR3}/${FILETOCOPY2}"
	eos cp ${BASEREF}/${DIR3}/${FILETOCOPY2} .
    else
	FILETOCOPY2=`eos ls ${BASEREF}/${DIR4} | grep HIST`
	echo "TEST ${BASEREF}/${DIR4}/${FILETOCOPY2}"
	eos cp ${BASEREF}/${DIR4}/${FILETOCOPY2} .
    fi    
fi




hcg.exe ${FILETOCOPY1} -d HLT/Egamma -r Egamma TREG -ds "https://twiki.cern.ch/twiki/bin/view/Atlas/TrigEgammaDataQualityAndMonitoring" -a "HLT_Histogram_Not_Empty_with_Ref&GatherData" --deleteref > ${data}.${stream}.$run.config

if [ -n "$refrun" ]; then
    trigEgammaDQ.py --file ${FILETOCOPY1} --run ${run} --t0 True --ref ${FILETOCOPY2} --refrun ${refrun} --lumi True
else 
    trigEgammaDQ.py --file ${FILETOCOPY1} --run ${run} --t0 True --lumi True
fi

rm *.bak
#tar -czf TrigEgammaDQArchive_Run_${run}.tar.gz ${data}.* *.C *.eps --remove-files


