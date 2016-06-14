#!/bin/bash
display_usage() { 
    echo -e "\nUsage:\n$0 [runNumber] [dataCampaign] [stream] \n" 
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
if [ -z "$2" ]
then
    echo "No stream, using physics_Main"
else
    stream=${3}
fi

echo $run $data $stream
source /afs/cern.ch/project/eos/installation/atlas/etc/setup.sh

# BASE=/eos/atlas/atlastier0/rucio/data15_cos/express_express/00259416
# DIR1=`eos ls ${BASE} | grep HIST.x`
# FILETOCOPY=`eos ls ${BASE}/${DIR1} | grep HIST`
# echo "TEST ${BASE}/${DIR1}/${FILETOCOPY}"
# eos cp ${BASE}/${DIR1}/${FILETOCOPY} .

# for file in `echo ${LIST}`
# do
BASE=/eos/atlas/atlastier0/rucio/${data}/${stream}/00${run}
echo $BASE    
DIR1=`eos ls ${BASE} | grep HIST.x`
DIR2=`eos ls ${BASE} | grep HIST.f`
echo $DIR1 $DIR2
if [ -n "$DIR1" ]; then
    FILETOCOPY=`eos ls ${BASE}/${DIR1} | grep HIST`
    echo "TEST ${BASE}/${DIR1}/${FILETOCOPY}"
    eos cp ${BASE}/${DIR1}/${FILETOCOPY} .
else
    FILETOCOPY=`eos ls ${BASE}/${DIR2} | grep HIST`
    echo "TEST ${BASE}/${DIR2}/${FILETOCOPY}"
    eos cp ${BASE}/${DIR2}/${FILETOCOPY} .
fi    
inputfile=`ls | grep HIST`
hcg.exe ${inputfile} -d HLT/Egamma -r Egamma TREG -ds "https://twiki.cern.ch/twiki/bin/view/Atlas/TrigEgammaDataQualityAndMonitoring" -a "HLT_Histogram_Not_Empty_with_Ref&GatherData" --deleteref > ${data}.${stream}.$run.config
trigEgammaDQ.py --file ${inputfile} --run ${run} --t0 True
rm *.bak
#tar -czf TrigEgammaDQArchive_Run_${run}.tar.gz ${data}.* *.C *.eps --remove-files


