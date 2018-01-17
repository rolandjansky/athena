##############################################################################

# Testing script to be run via:
# . ./HLT/Trigger/TrigTransforms/TrigTransform/share/test_run.sh | tee test_log.txt
# or by specifying a particular test:
# . ./HLT/Trigger/TrigTransforms/TrigTransform/share/test_run.sh XX | tee test_log.txt

##############################################################################

echo "***Start time***"
date

# test categories
# X1 = BS - BS
# X2 = BS - BS and reco/cost
# X3 = debug style jobs
# X4 = use dumpOptions to compare

# list of tests - current status (set runTestXX argument to false)
# 01 athenaHLT BSRDO->BS - works for all files
# 11 as 01 but with release swap for first step - works correctly uses runtranslate JO
# 21 as 01 but DBlvl1pskey not DBextra - works
runTest01=false
runTest11=false
runTest21=false
# 02 adds reco steps (otherwise as 01) - works for all files
# 12 adds costmon (otherwise as 01, although runs not from DB) - works makes costmon files
# 22 costmon not from DB and checks tmp.BS - works
runTest02=false
runTest12=false
runTest22=false
# 03 run via a JSON file (otherwise as 01) - works
# 13 as 03 but with debug stream=True - works (BS file renaming to be finalised)
# 23 like 13 but with copy of file to test input ending with .data
runTest03=false
runTest13=false
runTest23=false
# 04 uses dumpOptions to show that athenaopts gives the same as DB args as in 01 - works
runTest04=false
# 05 run using testPhysicsV5 instead of from DB - works
runTest05=false

runTest00J=false # run all Jira tests (also needed to recreate json files for below)
runTest01J=false # ATR-11463 --postcommand
runTest02J=false # ATR-11555 --eventmodifier
runTest03J=false # PRODSYS-468 --prodSysBSRDO


if [ "$*" == "" ]; then
    echo "***No argument provided will run all tests***"
    runTest01=true
    runTest11=true
    runTest21=true
    runTest02=true
    runTest12=true
    runTest22=true
    runTest03=true
    runTest13=true
    runTest23=true
    runTest04=true
    runTest05=true
    runTest00J=true
    runTest01J=true
    runTest02J=true
    runTest03J=true
fi

if [ "$1" == "01" ]; then
    echo "***Selected: runTest01***"
    runTest01=true
fi

if [ "$1" == "11" ]; then
    echo "***Selected: runTest11***"
    runTest11=true
fi

if [ "$1" == "21" ]; then
    echo "***Selected: runTest21***"
    runTest21=true
fi

if [ "$1" == "02" ]; then
    echo "***Selected: runTest02***"
    runTest02=true
fi

if [ "$1" == "12" ]; then
    echo "***Selected: runTest12***"
    runTest12=true
fi

if [ "$1" == "22" ]; then
    echo "***Selected: runTest22***"
    runTest22=true
fi

if [ "$1" == "03" ]; then
    echo "***Selected: runTest03***"
    runTest03=true
fi

if [ "$1" == "13" ]; then
    echo "***Selected: runTest13***"
    runTest13=true
fi

if [ "$1" == "23" ]; then
    echo "***Selected: runTest23***"
    runTest23=true
fi

if [ "$1" == "04" ]; then
    echo "***Selected: runTest04***"
    runTest04=true
fi

if [ "$1" == "05" ]; then
    echo "***Selected: runTest05***"
    runTest05=true
fi

if [ "$1" == "00J" ]; then
    echo "***Selected 00J run all Jira tests***"
    runTest00J=true
    runTest01J=true
    runTest02J=true
    runTest03J=true
fi

if [ "$1" == "01J" ]; then
    echo "***Selected: runTest01J***"
    runTest00J=true
    runTest01J=true
fi

if [ "$1" == "02J" ]; then
    echo "***Selected: runTest02J***"
    runTest00J=true
    runTest02J=true
fi

if [ "$1" == "03J" ]; then
    echo "***Selected: runTest03J***"
    runTest00J=true
    runTest03J=true
fi

##############################################################################

if $runTest01 ; then

echo -e "\n******runTest01: BSRDO->BS******"

rm -rf runTest01_BSRDO_BS
mkdir runTest01_BSRDO_BS
cd runTest01_BSRDO_BS

Trig_reco_tf.py \
--inputBS_RDOFile=root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1 \
--jobOptionSvcType TrigConf::HLTJobOptionsSvc \
--useDB TRUE --DBtype Coral --DBserver TRIGGERDBREPR \
--DBsmkey 46 --DBhltpskey 33 --DBextra "{'lvl1key': 14}" \
--precommand "markTest=True" \
--ignoreErrors="True" --runNumber="253010" \
--maxEvents=1 --skipEvents=2 \
--outputHIST_HLTMONFile="HIST_HLTMON.04854087._000852.pool.root.1" \
--outputBSFile="RAW.05530098._000001.pool.root.1" \
> log.txt 2>&1

#may also want to make use of dumpoptions or testing a real precommand
#--dumpOptions=True \
#--precommand "rerunLVL1=True;markTest=True" \

echo "***trf command***"
grep "command line was" log.txt
echo "***trf end status***"
tail log.txt -n 1
echo "***runTest01: BSRDO->BS completed***"

echo "---test runNumber (253010)---$ grep \"Using run number\" log.BSRDOtoRAW"
grep "Using run number" log.BSRDOtoRAW
echo "---test skipEvents (2)---$ grep Skipped.*event log.BSRDOtoRAW"
grep Skipped.*event log.BSRDOtoRAW 
echo "---test maxEvents (1)---$ grep Processed log.BSRDOtoRAW"
grep Processed log.BSRDOtoRAW
echo "---test smkey (46) hltpskey (33) lvlkey (14) server (TRIGGERDBREPR)---$ grep \"TrigDBConnectionConfig after\" log.BSRDOtoRAW"
grep "TrigDBConnectionConfig after" log.BSRDOtoRAW
echo "---test BS file created---$ ls RAW.05530098._000001.pool.root.1"
ls RAW.05530098._000001.pool.root.1
echo "---test MON file created---$ ls HIST_HLTMON.04854087._000852.pool.root.1"
ls HIST_HLTMON.04854087._000852.pool.root.1
echo "---test metadata---$ grep lfn metadata.xml"
grep lfn metadata.xml

cd ..

echo "************"

fi

##############################################################################

if $runTest11 ; then

echo -e "\n******runTest11: BSRDO->BS after RelSwap******"

rm -rf runTest11_BSRDO_BS_RelSwap
mkdir runTest11_BSRDO_BS_RelSwap
cd runTest11_BSRDO_BS_RelSwap

Trig_reco_tf.py \
--inputBS_RDOFile=root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1 \
--jobOptionSvcType TrigConf::HLTJobOptionsSvc \
--useDB TRUE --DBtype Coral --DBserver TRIGGERDBREPR \
--DBsmkey 46 --DBhltpskey 33 --DBextra "{'lvl1key': 14}" \
--precommand "markTest=True" \
--ignoreErrors="True" --runNumber="253010" \
--maxEvents=1 --skipEvents=2 \
--outputHIST_HLTMONFile="HIST_HLTMON.04854087._000852.pool.root.1" \
--outputBSFile="RAW.05530098._000001.pool.root.1" \
--asetup b2r:AtlasP1HLT,20.1.3.1 \
> log.txt 2>&1

echo "***trf command***"
grep "command line was" log.txt
echo "***trf end status***"
tail log.txt -n 1
echo "***runTest11: BSRDO->BS completed***"

echo "---compare to runTest01---$ diff -s ../runTest01_BSRDO_BS/runargs.BSRDOtoRAW.py runargs.BSRDOtoRAW.py --ignore-matching-lines=\"Run arguments file auto-generated\""
diff -s ../runTest01_BSRDO_BS/runargs.BSRDOtoRAW.py runargs.BSRDOtoRAW.py --ignore-matching-lines="Run arguments file auto-generated"
echo "---compare to runTest01---$ diff -s ../runTest01_BSRDO_BS/runwrapper.BSRDOtoRAW.sh runwrapper.BSRDOtoRAW.sh"
diff -s ../runTest01_BSRDO_BS/runwrapper.BSRDOtoRAW.sh runwrapper.BSRDOtoRAW.sh 
echo "---compare to runTest01---$ diff -s ../runTest01_BSRDO_BS/runtranslate.BSRDOtoRAW.py runtranslate.BSRDOtoRAW.py"
diff -s ../runTest01_BSRDO_BS/runtranslate.BSRDOtoRAW.py runtranslate.BSRDOtoRAW.py
echo "---test runNumber (253010)---$ grep \"Using run number\" log.BSRDOtoRAW"
grep "Using run number" log.BSRDOtoRAW
echo "---test skipEvents (2)---$ grep Skipped.*event log.BSRDOtoRAW"
grep Skipped.*event log.BSRDOtoRAW 
echo "---test maxEvents (1)---$ grep Processed log.BSRDOtoRAW"
grep Processed log.BSRDOtoRAW
echo "---test smkey (46) hltpskey (33) lvlkey (14) server (TRIGGERDBREPR)---$ grep \"TrigDBConnectionConfig after\" log.BSRDOtoRAW"
grep "TrigDBConnectionConfig after" log.BSRDOtoRAW
echo "---test BS file created---$ ls RAW.05530098._000001.pool.root.1"
ls RAW.05530098._000001.pool.root.1
echo "---test MON file created---$ ls HIST_HLTMON.04854087._000852.pool.root.1"
ls HIST_HLTMON.04854087._000852.pool.root.1
echo "---test metadata---$ grep lfn metadata.xml"
grep lfn metadata.xml

cd ..

echo "************"

fi

##############################################################################

if $runTest21 ; then

echo -e "\n******runTest21: BSRDO->BS with DBlvl1pskey******"

rm -rf runTest21_BSRDO_BS_l1key
mkdir runTest21_BSRDO_BS_l1key
cd runTest21_BSRDO_BS_l1key

Trig_reco_tf.py \
--inputBS_RDOFile=root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1 \
--jobOptionSvcType TrigConf::HLTJobOptionsSvc \
--useDB TRUE --DBtype Coral --DBserver TRIGGERDBREPR \
--DBsmkey 46 --DBhltpskey 33 \
--DBlvl1pskey 14 \
--precommand "markTest=True" \
--ignoreErrors="True" --runNumber="253010" \
--maxEvents=1 --skipEvents=2 \
--outputHIST_HLTMONFile="HIST_HLTMON.04854087._000852.pool.root.1" \
--outputBSFile="RAW.05530098._000001.pool.root.1" \
> log.txt 2>&1

#for additional testing can also add a different DB extra to check lvl1key gets added
#--DBextra "{'blah': 14}" \

echo "***trf command***"
grep "command line was" log.txt
echo "***trf end status***"
tail log.txt -n 1
echo "***runTest21: BSRDO->BS completed***"

echo "---compare to runTest01---$ diff -s ../runTest01_BSRDO_BS/runargs.BSRDOtoRAW.py runargs.BSRDOtoRAW.py --ignore-matching-lines=\"Run arguments file auto-generated\""
diff -s ../runTest01_BSRDO_BS/runargs.BSRDOtoRAW.py runargs.BSRDOtoRAW.py --ignore-matching-lines="Run arguments file auto-generated"
echo "---compare to runTest01---$ diff -s ../runTest01_BSRDO_BS/runwrapper.BSRDOtoRAW.sh runwrapper.BSRDOtoRAW.sh"
diff -s ../runTest01_BSRDO_BS/runwrapper.BSRDOtoRAW.sh runwrapper.BSRDOtoRAW.sh 
echo "---compare to runTest01---$ diff -s ../runTest01_BSRDO_BS/runtranslate.BSRDOtoRAW.py runtranslate.BSRDOtoRAW.py"
diff -s ../runTest01_BSRDO_BS/runtranslate.BSRDOtoRAW.py runtranslate.BSRDOtoRAW.py
echo "---test runNumber (253010)---$ grep \"Using run number\" log.BSRDOtoRAW"
grep "Using run number" log.BSRDOtoRAW
echo "---test skipEvents (2)---$ grep Skipped.*event log.BSRDOtoRAW"
grep Skipped.*event log.BSRDOtoRAW 
echo "---test maxEvents (1)---$ grep Processed log.BSRDOtoRAW"
grep Processed log.BSRDOtoRAW
echo "---test smkey (46) hltpskey (33) lvlkey (14) server (TRIGGERDBREPR)---$ grep \"TrigDBConnectionConfig after\" log.BSRDOtoRAW"
grep "TrigDBConnectionConfig after" log.BSRDOtoRAW
echo "---test BS file created---$ ls RAW.05530098._000001.pool.root.1"
ls RAW.05530098._000001.pool.root.1
echo "---test MON file created---$ ls HIST_HLTMON.04854087._000852.pool.root.1"
ls HIST_HLTMON.04854087._000852.pool.root.1
echo "---test metadata---$ grep lfn metadata.xml"
grep lfn metadata.xml

cd ..

echo "************"

fi

##############################################################################

if $runTest02 ; then

echo -e "\n******runTest02: BSRDO->BS->ESD->AOD->HIST******"

rm -rf runTest02_BSRDO_RECO
mkdir runTest02_BSRDO_RECO
cd runTest02_BSRDO_RECO

Trig_reco_tf.py \
--inputBS_RDOFile=root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1 \
--jobOptionSvcType TrigConf::HLTJobOptionsSvc \
--useDB TRUE --DBtype Coral --DBserver TRIGGERDBREPR \
--DBsmkey 46 --DBhltpskey 33 --DBextra "{'lvl1key': 14}" \
--precommand "markTest=True" \
--ignoreErrors="True" --runNumber="253010" \
--maxEvents=1 --skipEvents=2 \
--outputHIST_HLTMONFile="HIST_HLTMON.04854087._000852.pool.root.1" \
--asetup r2e:AtlasProduction,20.1.4.2 e2a:AtlasProduction,20.1.4.2  \
--geometryVersion 'ATLAS-R2-2015-02-00-00' --conditionsTag 'CONDBR2-ES1PA-2014-01' --beamType 'cosmics' --autoConfiguration 'everything' \
--preExec r2e:'from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArHVCorr=False;jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr.set_Value_and_Lock(False);from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.doInnerDetectorCommissioning.set_Value_and_Lock(True);InDetFlags.useBroadClusterErrors.set_Value_and_Lock(False);DQMonFlags.doStreamAwareMon=False;DQMonFlags.enableLumiAccess=False;from JetRec.JetRecFlags import jetFlags;jetFlags.useTracks=False;DQMonFlags.doLVL1CaloMon=False;DQMonFlags.doCTPMon=False;' \
--outputBSFile="RAW.05530098._000001.pool.root.1" \
--outputESDFile='thetestESD.pool.root' \
--outputAODFile='thetestAOD.pool.root'  \
--outputHISTFile='thetestHIST.root' \
--triggerConfig=MCRECO:DB:TRIGGERDBREPR:46,14,33 \
> log.txt 2>&1

#should also test sending triggerConfig via substep
#--triggerConfig r2e=MCRECO:DB:TRIGGERDBREPR:46,14,33 \

echo "***trf command***"
grep "command line was" log.txt
echo "***trf end status***"
tail log.txt -n 1
echo "***runTest02: BSRDO->BS->ESD->AOD->HIST completed***"

echo "---test triggerConfig---$ grep TriggerFlags log.RAWtoESD | grep triggerConfig: log.RAWtoESD"
grep TriggerFlags log.RAWtoESD | grep triggerConfig: log.RAWtoESD
echo "---test metadata---$ grep lfn metadata.xml"
grep lfn metadata.xml
echo "---test BS file created---$ ls RAW.05530098._000001.pool.root.1"
ls RAW.05530098._000001.pool.root.1
echo "---test MON file created---$ ls HIST_HLTMON.04854087._000852.pool.root.1"
ls HIST_HLTMON.04854087._000852.pool.root.1
echo "---test ESD file created---$ ls thetestESD.pool.root"
ls thetestESD.pool.root
echo "---test AOD file created---$ ls thetestAOD.pool.root"
ls thetestAOD.pool.root
echo "---test HIST file created---$ ls thetestHIST.root"
ls thetestHIST.root

cd ..

echo "************"

fi

##############################################################################

if $runTest12 ; then

echo -e "\n******runTest12: BSRDO->BS->COST->RECO with costmon in DB******"

rm -rf runTest12_BSRDO_COST
mkdir runTest12_BSRDO_COST
cd runTest12_BSRDO_COST

#version from DB (note test uses data12!)
Trig_reco_tf.py \
--inputBS_RDOFile=root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/data12_8TeV.00212967.physics_EnhancedBias.merge_eb_zee_zmumu_cc.RAW._lb0291._SFO-5._0001.data \
--jobOptionSvcType TrigConf::HLTJobOptionsSvc \
--useDB TRUE --DBtype Coral --DBserver TRIGGERDBREPR \
--DBsmkey 64 --DBhltpskey 44 --DBextra "{'lvl1key': 21}" \
--ignoreErrors="True" --runNumber="212967" \
--maxEvents=1 --skipEvents=2 \
--outputHIST_HLTMONFile="HIST_HLTMON.04854087._000852.pool.root.1" \
--asetup r2c:AtlasProduction,20.1.4.8 r2e:AtlasProduction,20.1.4.8 e2a:AtlasProduction,20.1.4.8 \
--outputNTUP_TRIGCOSTFile="NTUP_TRIGCOST.04854087._000852.root.1" \
--outputNTUP_TRIGRATEFile="NTUP_TRIGRATE.04854087._000852.root.1" \
--outputNTUP_TRIGEBWGHTFile="NTUP_TRIGEBWGHT.04854087._000852.root.1" \
--geometryVersion 'ATLAS-R1-2012-02-00-00' --conditionsTag 'COMCOND-BLKPA-RUN1-07' --beamType 'collisions' --autoConfiguration 'everything' \
--preExec "RAWtoESD:from TriggerJobOpts.TriggerFlags import TriggerFlags; TriggerFlags.EDMDecodingVersion.set_Value_and_Lock(2);DQMonFlags.doStreamAwareMon=False;DQMonFlags.enableLumiAccess=False;from JetRec.JetRecFlags import jetFlags;jetFlags.useTracks=False;DQMonFlags.doLVL1CaloMon=False;DQMonFlags.doCTPMon=False" "ESDtoAOD:DQMonFlags.doStreamAwareMon=False;DQMonFlags.enableLumiAccess=False;from JetRec.JetRecFlags import jetFlags;jetFlags.useTracks=False;DQMonFlags.doLVL1CaloMon=False;DQMonFlags.doCTPMon=False;from TrigHLTMonitoring.HLTMonFlags import HLTMonFlags;HLTMonFlags.doTau=False" \
--outputBSFile="RAW.05530098._000001.pool.root.1" \
--outputESDFile='thetestESD.pool.root' \
--outputAODFile='thetestAOD.pool.root'  \
--outputHISTFile='thetestHIST.root' \
--triggerConfig=MCRECO:DB:TRIGGERDBREPR:64,21,44 \
> log.txt 2>&1

echo "***trf command***"
grep "command line was" log.txt
echo "***trf end status***"
tail log.txt -n 1
echo "***runTest12: BSRDO->BS->COST->RECO completed***"

echo "---check costForCAF enabled in BS->BS---$ grep costForCAF log.BSRDOtoRAW"
grep costForCAF log.BSRDOtoRAW
echo "---costmon summary---$ grep TrigCostRun log.RAWtoCOST | grep POST_HLT"
grep TrigCostRun log.RAWtoCOST | grep POST_HLT
echo "---test costmon ntups created---$ ls NTUP_TRIG*"
ls NTUP_TRIG*
echo "---test metadata---$ grep lfn metadata.xml"
grep lfn metadata.xml

cd ..

echo "************"

fi

##############################################################################

if $runTest22 ; then

echo -e "\n******runTest22: BSRDO->BS->COST costmon not DB + tmp.BS******"

rm -rf runTest22_BSRDO_COST
mkdir runTest22_BSRDO_COST
cd runTest22_BSRDO_COST

#version not from DB
Trig_reco_tf.py \
--inputBS_RDOFile=root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1 \
--precommand "testPhysicsV5=True;enableCostForCAF=True;enableCostMonitoring=True" \
--ignoreErrors="True" --runNumber="253010" \
--maxEvents=1 --skipEvents=2 \
--outputHIST_HLTMONFile="HIST_HLTMON.04854087._000852.pool.root.1" \
--asetup r2c:AtlasProduction,20.1.4.2  \
--outputNTUP_TRIGCOSTFile="NTUP_TRIGCOST.04854087._000852.root.1" \
--outputNTUP_TRIGRATEFile="NTUP_TRIGRATE.04854087._000852.root.1" \
--outputNTUP_TRIGEBWGHTFile="NTUP_TRIGEBWGHT.04854087._000852.root.1" \
> log.txt 2>&1

# For more reco steps would need
#   --triggerConfig=DATARECO:REPR \
# along with all the other reco info...
#   --asetup r2c:AtlasProduction,20.1.4.2 r2e:AtlasProduction,20.1.4.2 e2a:AtlasProduction,20.1.4.2 \
#   --geometryVersion ....

echo "***trf command***"
grep "command line was" log.txt
echo "***trf end status***"
tail log.txt -n 1
echo "***runTest22: BSRDO->BS->COST completed***"

echo "---check costForCAF enabled in BS->BS---$ grep costForCAF log.BSRDOtoRAW"
grep costForCAF log.BSRDOtoRAW
echo "---costmon summary---$ grep TrigCostRun log.RAWtoCOST | grep POST_HLT"
grep TrigCostRun log.RAWtoCOST | grep POST_HLT
echo "---test tmp.BS created---$ ls tmp.BS"
ls tmp.BS
echo "---test costmon ntups created---$ ls NTUP_TRIG*"
ls NTUP_TRIG*
echo "---test metadata---$ grep lfn metadata.xml"
grep lfn metadata.xml

cd ..

echo "************"

fi

##############################################################################

if $runTest03 ; then

echo -e "\n******runTest03: BSRDO->BS via JSON******"

rm -rf runTest03_BSRDO_BS_JSON
mkdir runTest03_BSRDO_BS_JSON
cd runTest03_BSRDO_BS_JSON

echo "{\
\"outputBSFile\": \"RAW.05530098._000001.pool.root.1\", \
\"inputBS_RDOFile\": [\"root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1\"], \
\"jobOptionSvcType\": \"TrigConf::HLTJobOptionsSvc\", \
\"useDB\": \"TRUE\", \"DBtype\": \"Coral\", \"DBserver\": \"TRIGGERDBREPR\", \
\"DBsmkey\": \"46\", \"DBhltpskey\": \"33\", \"DBextra\": \"{'lvl1key': 14}\", \
\"precommand\": \"markTest=True\", \
\"ignoreErrors\": \"True\", \"runNumber\": \""253010"\", \
\"maxEvents\": \"1\", \"skipEvents\": \"2\", \
\"outputHIST_HLTMONFile\": \"HIST_HLTMON.04854087._000852.pool.root.1\" \
}" \
> data15_cos.00253010.debug_all.daq.RAW.g17.dbgrec.task._0011.job.argdict.json 

Trig_reco_tf.py --argJSON=data15_cos.00253010.debug_all.daq.RAW.g17.dbgrec.task._0011.job.argdict.json \
> log.txt 2>&1

#--dumpOptions=True \
#--precommand "rerunLVL1=True;markTest=True" \

echo "***trf command***"
grep "command line was" log.txt
echo "***trf end status***"
tail log.txt -n 1
echo "***runTest03: BSRDO->BS completed***"

echo "---compare to runTest01---$ diff -s ../runTest01_BSRDO_BS/runargs.BSRDOtoRAW.py runargs.BSRDOtoRAW.py --ignore-matching-lines=\"Run arguments file auto-generated\""
diff -s ../runTest01_BSRDO_BS/runargs.BSRDOtoRAW.py runargs.BSRDOtoRAW.py --ignore-matching-lines="Run arguments file auto-generated"
echo "---compare to runTest01---$ diff -s ../runTest01_BSRDO_BS/runwrapper.BSRDOtoRAW.sh runwrapper.BSRDOtoRAW.sh"
diff -s ../runTest01_BSRDO_BS/runwrapper.BSRDOtoRAW.sh runwrapper.BSRDOtoRAW.sh 
echo "---compare to runTest01---$ diff -s ../runTest01_BSRDO_BS/runtranslate.BSRDOtoRAW.py runtranslate.BSRDOtoRAW.py"
diff -s ../runTest01_BSRDO_BS/runtranslate.BSRDOtoRAW.py runtranslate.BSRDOtoRAW.py
#extra diffs that could be useful
#echo "---compare jobReport.json to runTest01"
#diff ../runTest01_BSRDO_BS/jobReport.json jobReport.json
#echo "---compare jobReport.txt to runTest01"
#diff ../runTest01_BSRDO_BS/jobReport.txt jobReport.txt

cd ..

echo "************"

fi

##############################################################################

if $runTest13 ; then

echo -e "\n******runTest13: BSRDO->BS with DebugStream=True ******"

rm -rf runTest13_BSRDO_BS_Debug
mkdir runTest13_BSRDO_BS_Debug
cd runTest13_BSRDO_BS_Debug

echo "{\
\"outputHIST_DEBUGSTREAMMONFile\": \"data15_cos.00263146.debugrec_hlt.reproc.HIST_DEBUGSTREAMMON.g17#data15_cos.00263146.debugrec_hlt.reproc.HIST_DEBUGSTREAMMON.g17._0005\", \
\"outputBSFile\": \"data15_cos.00263146.debugrec_hlt.reproc.RAW.g17#data15_cos.00263146.debugrec_hlt.reproc.RAW.g17._0005\", \
\"inputBS_RDOFile\": [\"root://eosatlas.cern.ch//eos/atlas/atlastier0/rucio/data15_cos/debug_HLTSVForceAccept/00263146/data15_cos.00263146.debug_HLTSVForceAccept.daq.RAW/data15_cos.00263146.debug_HLTSVForceAccept.daq.RAW._lb0000._SFO-2._0001.data\"], \
\"ignoreErrors\": \"True\", \"runNumber\": \""263146"\", \
\"outputHIST_HLTMONFile\": \"HIST_HLTMON.04854087._000852.pool.root.1\", \
\"debug_stream\": \"True\", \
\"athenaopts\": \"-c testPhysicsV5=True\" \
}" \
> data15_cos.00263146.debug_all.daq.RAW.g17.dbgrec.task._0005.job.argdict.json 

Trig_reco_tf.py --argJSON=data15_cos.00263146.debug_all.daq.RAW.g17.dbgrec.task._0005.job.argdict.json \
> log.txt 2>&1

#--dumpOptions=True \
#--precommand "rerunLVL1=True;markTest=True" \

#\"jobOptionSvcType\": \"TrigConf::HLTJobOptionsSvc\", \
#\"useDB\": \"TRUE\", \"DBtype\": \"Coral\", \"DBserver\": \"TRIGGERDBREPR\", \
#\"DBsmkey\": \"46\", \"DBhltpskey\": \"33\", \"DBextra\": \"{'lvl1key': 14}\", \
#\"precommand\": \"markTest=True\", \
#\"maxEvents\": \"1\", \"skipEvents\": \"2\", \

echo "***trf command***"
grep "command line was" log.txt
echo "***trf end status***"
tail log.txt -n 1
echo "***runTest13: BSRDO->BS completed***"

echo "---test BS file created---$ ls data15_cos.00263146.debugrec_hlt.reproc.RAW.g17._0005"
ls data15_cos.00263146.debugrec_hlt.reproc.RAW.g17._0005
echo "---test DEBUGSTREAMMON file created---$ ls data15_cos.00263146.debugrec_hlt.reproc.HIST_DEBUGSTREAMMON.g17._0005"
ls data15_cos.00263146.debugrec_hlt.reproc.HIST_DEBUGSTREAMMON.g17._0005
echo "---test MON file created---$ ls HIST_HLTMON.04854087._000852.pool.root.1"
ls HIST_HLTMON.04854087._000852.pool.root.1
echo "---test metadata---$ grep lfn metadata.xml"
grep lfn metadata.xml

#echo "---compare to runTest01---$ diff -s ../runTest01_BSRDO_BS/runargs.BSRDOtoRAW.py runargs.BSRDOtoRAW.py --ignore-matching-lines=\"Run arguments file auto-generated\""
#diff -s ../runTest01_BSRDO_BS/runargs.BSRDOtoRAW.py runargs.BSRDOtoRAW.py --ignore-matching-lines="Run arguments file auto-generated"
#echo "---compare to runTest01---$ diff -s ../runTest01_BSRDO_BS/runwrapper.BSRDOtoRAW.sh runwrapper.BSRDOtoRAW.sh"
#diff -s ../runTest01_BSRDO_BS/runwrapper.BSRDOtoRAW.sh runwrapper.BSRDOtoRAW.sh 
#echo "---compare to runTest01---$ diff -s ../runTest01_BSRDO_BS/runtranslate.BSRDOtoRAW.py runtranslate.BSRDOtoRAW.py"
#diff -s ../runTest01_BSRDO_BS/runtranslate.BSRDOtoRAW.py runtranslate.BSRDOtoRAW.py
#extra diffs that could be useful
#echo "---compare jobReport.json to runTest01"
#diff ../runTest01_BSRDO_BS/jobReport.json jobReport.json
#echo "---compare jobReport.txt to runTest01"
#diff ../runTest01_BSRDO_BS/jobReport.txt jobReport.txt

cd ..

echo "************"

fi

##############################################################################

if $runTest23 ; then

echo -e "\n******runTest23: BSRDO->BS with copy of file first ******"

rm -rf runTest23_BSRDO_BS
mkdir runTest23_BSRDO_BS
cd runTest23_BSRDO_BS

xrdcp -f root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1 .

mv data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1 data15_13TeV.00271595.debug_PUTimeout.daq.RAW._lb0000._SFO-5._0004.data

echo "{\
\"outputBSFile\": \"data15_13TeV.00271595.debugrec_hlt.reproc.RAW.g17#data15_13TeV.00271595.debugrec_hlt.reproc.RAW.g17._0073\", \
\"inputBS_RDOFile\": [\"data15_13TeV.00271595.debug_all.daq.RAW#data15_13TeV.00271595.debug_PUTimeout.daq.RAW._lb0000._SFO-5._0004.data\"], \
\"ignoreErrors\": \"True\", \
\"athenaopts\": \"-c testPhysicsV5=True\" \
}" \
> data15_cos.00271595.debug_all.daq.RAW.g17.dbgrec.task._0005.job.argdict.json 

Trig_reco_tf.py --argJSON=data15_cos.00271595.debug_all.daq.RAW.g17.dbgrec.task._0005.job.argdict.json \
> log.txt 2>&1

echo "***trf command***"
grep "command line was" log.txt
echo "***trf end status***"
tail log.txt -n 1
echo "***runTest23: BSRDO->BS completed***"

echo "---test BS file created---$ ls data15_13TeV.00271595.debugrec_hlt.reproc.RAW.g17._0073"
ls data15_13TeV.00271595.debugrec_hlt.reproc.RAW.g17._0073
echo "---test metadata---$ grep lfn metadata.xml"
grep lfn metadata.xml

cd ..

echo "************"

fi

##############################################################################

if $runTest04 ; then

echo -e "\n******runTest04: BSRDO->BS with athenaopt******"

rm -rf runTest04_BSRDO_BS_athOpt
mkdir runTest04_BSRDO_BS_athOpt
cd runTest04_BSRDO_BS_athOpt

Trig_reco_tf.py \
--inputBS_RDOFile=root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1 \
--jobOptionSvcType TrigConf::HLTJobOptionsSvc \
--useDB TRUE --DBtype Coral --DBserver TRIGGERDBREPR \
--DBsmkey 46 --DBhltpskey 33 --DBextra "{'lvl1key': 14}" \
--precommand "markTest=True" \
--ignoreErrors="True" --runNumber="253010" \
--maxEvents=1 --skipEvents=2 \
--outputHIST_HLTMONFile="HIST_HLTMON.04854087._000852.pool.root.1" \
--outputBSFile="RAW.05530098._000001.pool.root.1" \
--dumpOptions=True \
> logDBargs.txt 2>&1

echo "***trf command using DB args***"
grep "command line was" logDBargs.txt
echo "***now rerun athenaHLT to get options dump***"
cat runwrapper.BSRDOtoRAW.sh

. ./runwrapper.BSRDOtoRAW.sh > dumpDBargs.txt 2>&1

Trig_reco_tf.py \
--inputBS_RDOFile=root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1 \
--athenaopts=" -J TrigConf::HLTJobOptionsSvc --db-type Coral --db-server TRIGGERDBREPR \
--use-database \
--db-smkey 46 --db-hltpskey 33 --db-extra \"{'lvl1key': 14}\" " \
--precommand "markTest=True" \
--ignoreErrors="True" --runNumber="253010" \
--maxEvents=1 --skipEvents=2 \
--outputHIST_HLTMONFile="HIST_HLTMON.04854087._000852.pool.root.1" \
--outputBSFile="RAW.05530098._000001.pool.root.1" \
--dumpOptions=True \
> logAthenaOpts.txt 2>&1

echo "***trf command using athenaopts***"
grep "command line was" logAthenaOpts.txt
echo "***now rerun athenaHLT to get options dump***"
cat runwrapper.BSRDOtoRAW.sh

. ./runwrapper.BSRDOtoRAW.sh > dumpAthenaOpts.txt 2>&1

echo "***runTest04: BSRDO->BS completed***"

echo "---compare dump with/without athenaopts---$ diff -s dumpDBargs.txt dumpAthenaOpts.txt --ignore-matching-lines=\"# User \""
diff -s dumpDBargs.txt dumpAthenaOpts.txt --ignore-matching-lines="# User "

cd ..

echo "************"

fi

##############################################################################

if $runTest05 ; then

echo -e "\n******runTest05: BSRDO->BS not from DB******"

rm -rf runTest05_BSRDO_BS_noDB
mkdir runTest05_BSRDO_BS_noDB
cd runTest05_BSRDO_BS_noDB

Trig_reco_tf.py \
--inputBS_RDOFile=root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1 \
--precommand "testPhysicsV5=True" \
--ignoreErrors="True" --runNumber="253010" \
--maxEvents=1 --skipEvents=2 \
--outputHIST_HLTMONFile="HIST_HLTMON.04854087._000852.pool.root.1" \
--outputBSFile="RAW.05530098._000001.pool.root.1" \
> log.txt 2>&1

echo "***trf command***"
grep "command line was" log.txt
echo "***trf end status***"
tail log.txt -n 1
echo "***runTest05: BSRDO->BS completed***"

echo "---test runNumber (253010)---$ grep \"Using run number\" log.BSRDOtoRAW"
grep "Using run number" log.BSRDOtoRAW
echo "---test skipEvents (2)---$ grep Skipped.*event log.BSRDOtoRAW"
grep Skipped.*event log.BSRDOtoRAW 
echo "---test maxEvents (1)---$ grep Processed log.BSRDOtoRAW"
grep Processed log.BSRDOtoRAW
echo "---test menu (PhysicsV5)---$ grep setMenu log.BSRDOtoRAW"
grep setMenu log.BSRDOtoRAW
echo "---test BS file created---$ ls RAW.05530098._000001.pool.root.1"
ls RAW.05530098._000001.pool.root.1
echo "---test MON file created---$ ls HIST_HLTMON.04854087._000852.pool.root.1"
ls HIST_HLTMON.04854087._000852.pool.root.1
echo "---test metadata---$ grep lfn metadata.xml"
grep lfn metadata.xml

cd ..

echo "************"

fi

##############################################################################

if $runTest00J ; then

echo -e "\n******runTest00J: create JSON files******"

rm -rf runTest00J
mkdir runTest00J
cd runTest00J

Trig_reco_tf.py \
--inputBS_RDOFile=root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1 \
--AMITag r6839 \
--DBhltpskey 76 --DBlvl1pskey 33 --DBserver TRIGGERDBREPR --DBsmkey 110 --DBtype Coral \
--autoConfiguration everything --beamType collisions \
--ignoreErrors True --jobOptionSvcType TrigConf::HLTJobOptionsSvc --maxEvents 1 \
--outputBSFile=RAW.05731105._005112.pool.root.1 \
--outputHIST_HLTMONFile=HIST_HLTMON.05731105._005112.pool.root.1 \
--runNumber 253010 \
--useDB True \
--dumpJSON hltonly.json

Trig_reco_tf.py \
--inputBS_RDOFile=root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1 \
--AMITag r6839 \
--DBhltpskey 76 --DBlvl1pskey 33 --DBserver TRIGGERDBREPR --DBsmkey 110 --DBtype Coral \
--asetup RAWtoESD:AtlasProduction,20.1.5.8 ESDtoAOD:AtlasProduction,20.1.5.8 RAWtoCOST:AtlasProduction,20.1.5.8 \
--autoConfiguration everything --beamType collisions \
--conditionsTag RAWtoESD:CONDBR2-ES1PA-2015-05 ESDtoAOD:CONDBR2-ES1PA-2015-05 \
--geometryVersion RAWtoESD:ATLAS-R2-2015-03-01-00 ESDtoAOD:ATLAS-R2-2015-03-01-00 \
--ignoreErrors True --jobOptionSvcType TrigConf::HLTJobOptionsSvc --maxEvents 1 \
--outputAODFile=AOD.05731105._005112.pool.root.1 \
--outputBSFile=RAW.05731105._005112.pool.root.1 \
--outputESDFile=ESD.05731105._005112.pool.root.1 \
--outputHISTFile=HIST.05731105._005112.pool.root.1 \
--outputHIST_HLTMONFile=HIST_HLTMON.05731105._005112.pool.root.1 \
--outputNTUP_TRIGCOSTFile=NTUP_TRIGCOST.05731105._005112.pool.root.1 \
--outputNTUP_TRIGRATEFile=NTUP_TRIGRATE.05731105._005112.pool.root.1 \
--postExec r2e:pass --preExec 'RAWtoESD:from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.useBeamConstraint.set_Value_and_Lock(False);DQMonFlags.doStreamAwareMon=False;DQMonFlags.enableLumiAccess=False;DQMonFlags.doLVL1CaloMon=True;DQMonFlags.doCTPMon=False;' 'ESDtoAOD:DQMonFlags.doStreamAwareMon=False;DQMonFlags.enableLumiAccess=False;DQMonFlags.doCTPMon=False' \
--runNumber 253010 \
--triggerConfig RAWtoESD=MCRECO:DB:TRIGGERDBREPR:110,33,76 --useDB True \
--dumpJSON hlt+reco.json

echo "---diff json files--$ diff hltonly.json hlt+reco.json"
diff hltonly.json hlt+reco.json

echo "***runTest00J completed***"

cd ..

echo "************"

fi

##############################################################################

if $runTest01J ; then

echo -e "\n******runTest01J: ATR-11463******"

rm -rf runTest01J
mkdir runTest01J
cd runTest01J

mkdir HLTONLY
cd HLTONLY

echo -e "\n******run HLTONLY******"

Trig_reco_tf.py \
--argJSON="../../runTest00J/hltonly.json" \
--postcommand="include(\"TriggerRelease/dbmod_BFieldAutoConfig.py\")" \
> log.txt 2>&1

echo "***trf command***"
grep "command line was" log.txt
echo "***trf end status***"
tail log.txt -n 1
echo "***HLTONLY completed***"

echo "---test runNumber (253010)---$ grep \"Using run number\" log.BSRDOtoRAW"
grep "Using run number" log.BSRDOtoRAW
echo "---test maxEvents (1)---$ grep Processed log.BSRDOtoRAW"
grep Processed log.BSRDOtoRAW
echo "---test smkey (110) hltpskey (76) lvlkey (33) server (TRIGGERDBREPR)---$ grep \"TrigDBConnectionConfig after\" log.BSRDOtoRAW"
grep "TrigDBConnectionConfig after" log.BSRDOtoRAW
echo "---test BS file created---$ ls RAW.05731105._005112.pool.root.1"
ls RAW.05731105._005112.pool.root.1
echo "---test MON file created---$ ls HIST_HLTMON.05731105._005112.pool.root.1"
ls HIST_HLTMON.05731105._005112.pool.root.1
echo "---test metadata---$ grep lfn metadata.xml"
grep lfn metadata.xml

cd ..

mkdir HLT+RECO
cd HLT+RECO

echo -e "\n******run HLT+RECO******"

Trig_reco_tf.py \
--argJSON=../../runTest00J/hlt+reco.json \
--postcommand="include(\"TriggerRelease/dbmod_BFieldAutoConfig.py\")" \
> log.txt 2>&1

echo "***trf command***"
grep "command line was" log.txt
echo "***trf end status***"
tail log.txt -n 1
echo "***HLT+RECO completed***"

echo "---test runNumber (253010)---$ grep \"Using run number\" log.BSRDOtoRAW"
grep "Using run number" log.BSRDOtoRAW
echo "---test maxEvents (1)---$ grep Processed log.BSRDOtoRAW"
grep Processed log.BSRDOtoRAW
echo "---test smkey (110) hltpskey (76) lvlkey (33) server (TRIGGERDBREPR)---$ grep \"TrigDBConnectionConfig after\" log.BSRDOtoRAW"
grep "TrigDBConnectionConfig after" log.BSRDOtoRAW
echo "---test BS file created---$ ls RAW.05731105._005112.pool.root.1"
ls RAW.05731105._005112.pool.root.1
echo "---test MON file created---$ ls HIST_HLTMON.05731105._005112.pool.root.1"
ls HIST_HLTMON.05731105._005112.pool.root.1
echo "---test metadata---$ grep lfn metadata.xml"
grep lfn metadata.xml

cd ..

echo "---test postcommand---$ grep BFieldAutoConfig */log.BSRDOtoRAW"
grep BFieldAutoConfig */log.BSRDOtoRAW

echo "***runTest01J completed***"

cd ..

echo "************"

fi

##############################################################################

if $runTest02J ; then

echo -e "\n******runTest02J: ATR-11555******"

rm -rf runTest02J
mkdir runTest02J
cd runTest02J

mkdir HLTONLY
cd HLTONLY

echo -e "\n******run HLTONLY******"

Trig_reco_tf.py \
--argJSON=../../runTest00J/hltonly.json \
--eventmodifier="TrigByteStreamTools.trigbs_prescaleL1" \
> log.txt 2>&1

echo "***trf command***"
grep "command line was" log.txt
echo "***trf end status***"
tail log.txt -n 1
echo "***HLTONLY completed***"

echo "---test runNumber (253010)---$ grep \"Using run number\" log.BSRDOtoRAW"
grep "Using run number" log.BSRDOtoRAW
echo "---test maxEvents (1)---$ grep Processed log.BSRDOtoRAW"
grep Processed log.BSRDOtoRAW
echo "---test smkey (110) hltpskey (76) lvlkey (33) server (TRIGGERDBREPR)---$ grep \"TrigDBConnectionConfig after\" log.BSRDOtoRAW"
grep "TrigDBConnectionConfig after" log.BSRDOtoRAW
echo "---test BS file created---$ ls RAW.05731105._005112.pool.root.1"
ls RAW.05731105._005112.pool.root.1
echo "---test MON file created---$ ls HIST_HLTMON.05731105._005112.pool.root.1"
ls HIST_HLTMON.05731105._005112.pool.root.1
echo "---test metadata---$ grep lfn metadata.xml"
grep lfn metadata.xml

cd ..

mkdir HLT+RECO
cd HLT+RECO

echo -e "\n******run HLT+RECO******"

Trig_reco_tf.py \
--argJSON=../../runTest00J/hlt+reco.json \
--eventmodifier="TrigByteStreamTools.trigbs_prescaleL1" \
> log.txt 2>&1

echo "***trf command***"
grep "command line was" log.txt
echo "***trf end status***"
tail log.txt -n 1
echo "***HLT+RECO completed***"

echo "---test runNumber (253010)---$ grep \"Using run number\" log.BSRDOtoRAW"
grep "Using run number" log.BSRDOtoRAW
echo "---test maxEvents (1)---$ grep Processed log.BSRDOtoRAW"
grep Processed log.BSRDOtoRAW
echo "---test smkey (110) hltpskey (76) lvlkey (33) server (TRIGGERDBREPR)---$ grep \"TrigDBConnectionConfig after\" log.BSRDOtoRAW"
grep "TrigDBConnectionConfig after" log.BSRDOtoRAW
echo "---test BS file created---$ ls RAW.05731105._005112.pool.root.1"
ls RAW.05731105._005112.pool.root.1
echo "---test MON file created---$ ls HIST_HLTMON.05731105._005112.pool.root.1"
ls HIST_HLTMON.05731105._005112.pool.root.1
echo "---test metadata---$ grep lfn metadata.xml"
grep lfn metadata.xml

cd ..

echo "---test eventmodifier---$ grep trigbs_prescaleL1 */log.BSRDOtoRAW"
grep trigbs_prescaleL1 */log.BSRDOtoRAW

echo "***runTest02J completed***"

cd ..

echo "************"

fi

##############################################################################

if $runTest03J ; then

echo -e "\n******runTest03J: PRODSYS-468******"

rm -rf runTest03J
mkdir runTest03J
cd runTest03J

mkdir HLTONLY
cd HLTONLY

echo -e "\n******run HLTONLY******"

Trig_reco_tf.py \
--argJSON=../../runTest00J/hltonly.json \
--prodSysBSRDO True \
> log.txt 2>&1

echo "***trf command***"
grep "command line was" log.txt
echo "***trf end status***"
tail log.txt -n 1
echo "***HLTONLY completed***"

echo "---test runNumber (253010)---$ grep \"Using run number\" log.BSRDOtoRAW"
grep "Using run number" log.BSRDOtoRAW
echo "---test maxEvents (1)---$ grep Processed log.BSRDOtoRAW"
grep Processed log.BSRDOtoRAW
echo "---test smkey (110) hltpskey (76) lvlkey (33) server (TRIGGERDBREPR)---$ grep \"TrigDBConnectionConfig after\" log.BSRDOtoRAW"
grep "TrigDBConnectionConfig after" log.BSRDOtoRAW
echo "---test BS file created---$ ls RAW.05731105._005112.pool.root.1"
ls RAW.05731105._005112.pool.root.1
echo "---test MON file created---$ ls HIST_HLTMON.05731105._005112.pool.root.1"
ls HIST_HLTMON.05731105._005112.pool.root.1
echo "---test metadata---$ grep lfn metadata.xml"
grep lfn metadata.xml

cd ..

mkdir HLT+RECO
cd HLT+RECO

echo -e "\n******run HLT+RECO******"

Trig_reco_tf.py \
--argJSON=../../runTest00J/hlt+reco.json \
--prodSysBSRDO True \
> log.txt 2>&1

echo "***trf command***"
grep "command line was" log.txt
echo "***trf end status***"
tail log.txt -n 1
echo "***HLT+RECO completed***"

echo "---test runNumber (253010)---$ grep \"Using run number\" log.BSRDOtoRAW"
grep "Using run number" log.BSRDOtoRAW
echo "---test maxEvents (1)---$ grep Processed log.BSRDOtoRAW"
grep Processed log.BSRDOtoRAW
echo "---test smkey (110) hltpskey (76) lvlkey (33) server (TRIGGERDBREPR)---$ grep \"TrigDBConnectionConfig after\" log.BSRDOtoRAW"
grep "TrigDBConnectionConfig after" log.BSRDOtoRAW
echo "---test BS file created---$ ls RAW.05731105._005112.pool.root.1"
ls RAW.05731105._005112.pool.root.1
echo "---test MON file created---$ ls HIST_HLTMON.05731105._005112.pool.root.1"
ls HIST_HLTMON.05731105._005112.pool.root.1
echo "---test metadata---$ grep lfn metadata.xml"
grep lfn metadata.xml

cd ..

echo "---test eventmodifier---$ grep prodSysBSRDO */log.BSRDOtoRAW"
grep prodSysBSRDO */jobReport.json

echo "***runTest03J completed***"

cd ..

echo "************"

fi

##############################################################################
    
echo -e "\n***End time***"
date
