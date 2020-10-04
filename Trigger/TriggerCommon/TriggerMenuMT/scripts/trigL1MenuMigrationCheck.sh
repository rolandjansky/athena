#!/bin/env sh

testdir=tmp
rm -rf ${testdir}
mkdir ${testdir}
cd ${testdir}
mkdir log

echo "Test directory: ${testdir}/"
echo "Log files directory: ${testdir}/log/"
echo "AtlasVersion = ${AtlasVersion}"

echo "Generating L1 menus: L1Topoconfig_LS2_v1_${AtlasVersion}.xml LVL1config_LS2_v1_${AtlasVersion}.xml L1Menu_LS2_v1_${AtlasVersion}.json"
cmd="generateL1MenuMT.sh LS2_v1"
echo "Command:" > ./log/l1menuGeneration.log
echo "${cmd}" >> ./log/l1menuGeneration.log
echo "Output:" >> ./log/l1menuGeneration.log
${cmd} >> ./log/l1menuGeneration.log 2>&1

echo "Generating HLT menu: HLTMenu_LS2_v1_${AtlasVersion}.json"
DS=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.RDO.e4993_s3214_r11315/RDO.17533168._000001.pool.root.1
echo "Command:" > ./log/hltmenuGeneration.log
echo "DS=${DS}" >> ./log/hltmenuGeneration.log
echo "Output:" >> ./log/hltmenuGeneration.log
echo "athena.py -b -c \"endJobAfterGenerate=True;setMenu='LS2_v1'\" --threads=1 --filesInput=${DS} TriggerJobOpts/runHLT_standalone.py" >> ./log/hltmenuGeneration.log 
athena.py -b -c "endJobAfterGenerate=True;setMenu='LS2_v1'" --threads=1 --filesInput=${DS} TriggerJobOpts/runHLT_standalone.py >> ./log/hltmenuGeneration.log 2>&1

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
L1Topoconfig_LS2_v1_${AtlasVersion}.xml \
L1TopoConfig_SMK_2749.xml"

echo "Running menu comparison"
echo "Command:"
echo "${cmd}"
echo "Output:"
${cmd}
