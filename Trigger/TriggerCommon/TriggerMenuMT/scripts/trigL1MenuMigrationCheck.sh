#!/bin/env sh

testdir=tmp
rm -rf ${testdir}
mkdir ${testdir}
cd ${testdir}
mkdir log

echo "Test directory: ${testdir}/"
echo "Log files directory: ${testdir}/log/"
echo "AtlasVersion = ${AtlasVersion}"

echo "Generating L1 xml menus: LVL1config_LS2_v1_${AtlasVersion}.xml"
cmd="generateL1MenuMT.sh LS2_v1"
echo "Command:" > ./log/l1menuGeneration.log
echo "${cmd}" >> ./log/l1menuGeneration.log
echo "Output:" >> ./log/l1menuGeneration.log
${cmd} >> ./log/l1menuGeneration.log 2>&1

echo "Generating HLT and L1 json menu: HLTMenu_LS2_v1_${AtlasVersion}.json"
DS=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data18_13TeV.00360026.physics_EnhancedBias.merge.RAW._lb0151._SFO-1._0001.1
echo "Command:" > ./log/hltmenuGeneration.log
echo "DS=${DS}" >> ./log/hltmenuGeneration.log
echo "Output:" >> ./log/hltmenuGeneration.log

echo "athena.py --config-only=config.pkl -c \"setMenu='LS2_v1'\" --imf --perfmon --threads=1 --evtMax=1 --filesInput=${DS}  TriggerJobOpts/runHLT_standalone.py " >> ./log/hltmenuGeneration.log
athena.py --config-only=config.pkl -c "setMenu='LS2_v1'" --imf --perfmon --threads=1 --evtMax=1 --filesInput=${DS}  TriggerJobOpts/runHLT_standalone.py >> ./log/hltmenuGeneration.log 2>&1

# generate EB run 360026 files: LVL1 menu LVL1config_SMK_2749.xml, HLTconfig_SMK_2749.xml, L1TopoConfig_SMK_2749.xml
echo "Downloading L1Topo menu from EB run 360026: L1TopoConfig_SMK_2749.xml"
cmd="TrigConfReadWrite -i TRIGGERDB 2749 -o xml SMK_2749"
echo "Command:" > ./log/run2L1TopoDownload.log
echo "${cmd}" >> ./log/run2L1TopoDownload.log
echo "Output:" >> ./log/run2L1TopoDownload.log
${cmd} >> ./log/run2L1TopoDownload.log 2>&1

cmd="trigCompareOldandNewL1Menus.py \
L1Menu_LS2_v1_${AtlasVersion}.json \
LVL1config_LS2_v1_${AtlasVersion}.xml \
HLTMenu_LS2_v1_${AtlasVersion}.json \
L1TopoConfig_SMK_2749.xml"

echo "Running menu comparison"
echo "Command:"
echo "${cmd}"
echo "Output:"
${cmd}
