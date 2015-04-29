##############################################################################

# Testing script to be run via:
# . ./HLT/Trigger/TrigTransforms/TrigTransform/share/test_run.sh | tee test_log.txt
# or by specifying a particular test:
# . ./HLT/Trigger/TrigTransforms/TrigTransform/share/test_run.sh XX | tee test_log.txt

##############################################################################

echo "***Start time***"
date

# list of tests - current status (set runTestXX argument to false)
# 01 athenaHLT BSRDO->BS - works for all files
# 11 as 01 but with release swap for first step - works correctly uses runtranslate JO
# 21 as 01 but DBlvl1pskey not DBextra - works
runTest01=false
runTest11=false
runTest21=false
# 02 adds reco steps (otherwise as 01) - works for all files
# 12 adds costmon (otherwise as 01, although runs not from DB) - works makes costmon files
runTest02=false
runTest12=false
# 03 run via a JSON file (otherwise as 01) - works
# 13 as 03 but with debug stream=True - works (BS file renaming to be finalised)
runTest03=false
runTest13=false
# 04 uses dumpOptions to show that athenaopts gives the same as DB args as in 01 - works
runTest04=false
# 05 run using testPhysicsV5 instead of from DB - works
runTest05=false

if [ "$*" == "" ]; then
    echo "***No argument provided will run all tests***"
    runTest01=true
    runTest11=true
    runTest21=true
    runTest02=true
    runTest12=true
    runTest03=true
    runTest13=true
    runTest04=true
    runTest05=true
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

if [ "$1" == "03" ]; then
    echo "***Selected: runTest03***"
    runTest03=true
fi

if [ "$1" == "13" ]; then
    echo "***Selected: runTest13***"
    runTest13=true
fi

if [ "$1" == "04" ]; then
    echo "***Selected: runTest04***"
    runTest04=true
fi

if [ "$1" == "05" ]; then
    echo "***Selected: runTest05***"
    runTest05=true
fi

##############################################################################

if $runTest01 ; then

echo -e "\n******runTest01: BSRDO->BS******"

rm -rf runTest01_BSRDO_BS
mkdir runTest01_BSRDO_BS
cd runTest01_BSRDO_BS

Trig_reco_tf.py \
--inputBS_RDOFile=root://eosatlas//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1 \
--jobOptionSvcType TrigConf::HLTJobOptionsSvc \
--useDB TRUE --DBtype Coral --DBserver TRIGGERDBREPR \
--DBsmkey 46 --DBhltpskey 33 --DBextra "{'lvl1key': 14}" \
--precommand "markTest=True" \
--ignoreErrors="True" --runNumber="253010" \
--maxEvents=1 --skipEvents=2 \
--outputHIST_HLTMONFile="HIST_HLTMON.04854087._000852.pool.root.1" \
--outputBSFile="thetestBS.RAW.data" \
> log.txt 2>&1

#may also want to make use of dumpoptions or testing a real precommand
#--dumpOptions=True \
#--precommand "rerunLVL1=True;markTest=True" \

echo "***trf command***"
grep "command line was" log.txt
echo "***trf end status***"
tail log.txt -n 1
echo "***BSRDO->BS completed***"

echo "---test skipEvents (2)---$ grep Skipped.*event log.BSRDOtoRAW"
grep Skipped.*event log.BSRDOtoRAW 
echo "---test maxEvents (1)---$ grep Processed log.BSRDOtoRAW"
grep Processed log.BSRDOtoRAW
echo "---test smkey (46) hltpskey (33) lvlkey (14) server (TRIGGERDBREPR)---$ grep \"TrigDBConnectionConfig after\" log.BSRDOtoRAW"
grep "TrigDBConnectionConfig after" log.BSRDOtoRAW
echo "---test BS file created---$ ls thetestBS.RAW.data"
ls thetestBS.RAW.data
echo "---test MON file created---$ ls HIST_HLTMON.04854087._000852.pool.root.1"
ls HIST_HLTMON.04854087._000852.pool.root.1
echo "---test metadata---$ grep lfn metadata.xml"
grep lfn metadata.xml

cd ..

fi

##############################################################################

if $runTest11 ; then

echo -e "\n******runTest11: BSRDO->BS after RelSwap******"

rm -rf runTest11_BSRDO_BS_RelSwap
mkdir runTest11_BSRDO_BS_RelSwap
cd runTest11_BSRDO_BS_RelSwap

Trig_reco_tf.py \
--inputBS_RDOFile=root://eosatlas//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1 \
--jobOptionSvcType TrigConf::HLTJobOptionsSvc \
--useDB TRUE --DBtype Coral --DBserver TRIGGERDBREPR \
--DBsmkey 46 --DBhltpskey 33 --DBextra "{'lvl1key': 14}" \
--precommand "markTest=True" \
--ignoreErrors="True" --runNumber="253010" \
--maxEvents=1 --skipEvents=2 \
--outputHIST_HLTMONFile="HIST_HLTMON.04854087._000852.pool.root.1" \
--outputBSFile="thetestBS.RAW.data" \
--asetup b2r:AtlasP1HLT,20.1.3.1 \
> log.txt 2>&1

echo "***trf command***"
grep "command line was" log.txt
echo "***trf end status***"
tail log.txt -n 1
echo "***BSRDO->BS completed***"

echo "---compare to runTest01---$ diff -s ../runTest01_BSRDO_BS/runargs.BSRDOtoRAW.py runargs.BSRDOtoRAW.py --ignore-matching-lines=\"Run arguments file auto-generated\""
diff -s ../runTest01_BSRDO_BS/runargs.BSRDOtoRAW.py runargs.BSRDOtoRAW.py --ignore-matching-lines="Run arguments file auto-generated"
echo "---compare to runTest01---$ diff -s ../runTest01_BSRDO_BS/runwrapper.BSRDOtoRAW.sh runwrapper.BSRDOtoRAW.sh"
diff -s ../runTest01_BSRDO_BS/runwrapper.BSRDOtoRAW.sh runwrapper.BSRDOtoRAW.sh 
echo "---compare to runTest01---$ diff -s ../runTest01_BSRDO_BS/runtranslate.BSRDOtoRAW.py runtranslate.BSRDOtoRAW.py"
diff -s ../runTest01_BSRDO_BS/runtranslate.BSRDOtoRAW.py runtranslate.BSRDOtoRAW.py
echo "---test skipEvents (2)---$ grep Skipped.*event log.BSRDOtoRAW"
grep Skipped.*event log.BSRDOtoRAW 
echo "---test maxEvents (1)---$ grep Processed log.BSRDOtoRAW"
grep Processed log.BSRDOtoRAW
echo "---test smkey (46) hltpskey (33) lvlkey (14) server (TRIGGERDBREPR)---$ grep \"TrigDBConnectionConfig after\" log.BSRDOtoRAW"
grep "TrigDBConnectionConfig after" log.BSRDOtoRAW
echo "---test BS file created---$ ls thetestBS.RAW.data"
ls thetestBS.RAW.data
echo "---test MON file created---$ ls HIST_HLTMON.04854087._000852.pool.root.1"
ls HIST_HLTMON.04854087._000852.pool.root.1
echo "---test metadata---$ grep lfn metadata.xml"
grep lfn metadata.xml

cd ..

fi

##############################################################################

if $runTest21 ; then

echo -e "\n******runTest21: BSRDO->BS with DBlvl1pskey******"

rm -rf runTest21_BSRDO_BS_l1key
mkdir runTest21_BSRDO_BS_l1key
cd runTest21_BSRDO_BS_l1key

Trig_reco_tf.py \
--inputBS_RDOFile=root://eosatlas//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1 \
--jobOptionSvcType TrigConf::HLTJobOptionsSvc \
--useDB TRUE --DBtype Coral --DBserver TRIGGERDBREPR \
--DBsmkey 46 --DBhltpskey 33 \
--DBlvl1pskey 14 \
--precommand "markTest=True" \
--ignoreErrors="True" --runNumber="253010" \
--maxEvents=1 --skipEvents=2 \
--outputHIST_HLTMONFile="HIST_HLTMON.04854087._000852.pool.root.1" \
--outputBSFile="thetestBS.RAW.data" \
> log.txt 2>&1

#for additional testing can also add a different DB extra to check lvl1key gets added
#--DBextra "{'blah': 14}" \

echo "***trf command***"
grep "command line was" log.txt
echo "***trf end status***"
tail log.txt -n 1
echo "***BSRDO->BS completed***"

echo "---compare to runTest01---$ diff -s ../runTest01_BSRDO_BS/runargs.BSRDOtoRAW.py runargs.BSRDOtoRAW.py --ignore-matching-lines=\"Run arguments file auto-generated\""
diff -s ../runTest01_BSRDO_BS/runargs.BSRDOtoRAW.py runargs.BSRDOtoRAW.py --ignore-matching-lines="Run arguments file auto-generated"
echo "---compare to runTest01---$ diff -s ../runTest01_BSRDO_BS/runwrapper.BSRDOtoRAW.sh runwrapper.BSRDOtoRAW.sh"
diff -s ../runTest01_BSRDO_BS/runwrapper.BSRDOtoRAW.sh runwrapper.BSRDOtoRAW.sh 
echo "---compare to runTest01---$ diff -s ../runTest01_BSRDO_BS/runtranslate.BSRDOtoRAW.py runtranslate.BSRDOtoRAW.py"
diff -s ../runTest01_BSRDO_BS/runtranslate.BSRDOtoRAW.py runtranslate.BSRDOtoRAW.py
echo "---test skipEvents (2)---$ grep Skipped.*event log.BSRDOtoRAW"
grep Skipped.*event log.BSRDOtoRAW 
echo "---test maxEvents (1)---$ grep Processed log.BSRDOtoRAW"
grep Processed log.BSRDOtoRAW
echo "---test smkey (46) hltpskey (33) lvlkey (14) server (TRIGGERDBREPR)---$ grep \"TrigDBConnectionConfig after\" log.BSRDOtoRAW"
grep "TrigDBConnectionConfig after" log.BSRDOtoRAW
echo "---test BS file created---$ ls thetestBS.RAW.data"
ls thetestBS.RAW.data
echo "---test MON file created---$ ls HIST_HLTMON.04854087._000852.pool.root.1"
ls HIST_HLTMON.04854087._000852.pool.root.1
echo "---test metadata---$ grep lfn metadata.xml"
grep lfn metadata.xml

cd ..

fi

##############################################################################

if $runTest02 ; then

echo -e "\n******runTest02: BSRDO->BS->ESD->AOD->HIST******"

rm -rf runTest02_BSRDO_RECO
mkdir runTest02_BSRDO_RECO
cd runTest02_BSRDO_RECO

Trig_reco_tf.py \
--inputBS_RDOFile=root://eosatlas//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1 \
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
--outputBSFile='thetestBS.RAW.data' \
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
echo "***BSRDO->BS->ESD->AOD->HIST completed***"

echo "---test triggerConfig---$ grep TriggerFlags log.RAWtoESD | grep triggerConfig: log.RAWtoESD"
grep TriggerFlags log.RAWtoESD | grep triggerConfig: log.RAWtoESD
echo "---test metadata---$ grep lfn metadata.xml"
grep lfn metadata.xml
echo "---test BS file created---$ ls thetestBS.RAW.data"
ls thetestBS.RAW.data
echo "---test MON file created---$ ls HIST_HLTMON.04854087._000852.pool.root.1"
ls HIST_HLTMON.04854087._000852.pool.root.1
echo "---test ESD file created---$ ls thetestESD.pool.root"
ls thetestESD.pool.root
echo "---test AOD file created---$ ls thetestAOD.pool.root"
ls thetestAOD.pool.root
echo "---test HIST file created---$ ls thetestHIST.root"
ls thetestHIST.root

cd ..

fi

##############################################################################

if $runTest12 ; then

echo -e "\n******runTest12: BSRDO->BS->COST->RECO with costmon******"

rm -rf runTest12_BSRDO_COST
mkdir runTest12_BSRDO_COST
cd runTest12_BSRDO_COST

#version not from DB
Trig_reco_tf.py \
--inputBS_RDOFile=root://eosatlas//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1 \
--precommand "testPhysicsV5=True;enableCostForCAF=True;enableCostMonitoring=True" \
--ignoreErrors="True" --runNumber="253010" \
--maxEvents=1 --skipEvents=2 \
--outputHIST_HLTMONFile="HIST_HLTMON.04854087._000852.pool.root.1" \
--asetup r2c:AtlasProduction,20.1.4.2 r2e:AtlasProduction,20.1.4.2 e2a:AtlasProduction,20.1.4.2 \
--outputNTUP_TRIGCOSTFile="NTUP_TRIGCOST.04854087._000852.root.1" \
--outputNTUP_TRIGRATEFile="NTUP_TRIGRATE.04854087._000852.root.1" \
--outputNTUP_TRIGEBWGHTFile="NTUP_TRIGEBWGHT.04854087._000852.root.1" \
--geometryVersion 'ATLAS-R2-2015-02-00-00' --conditionsTag 'CONDBR2-ES1PA-2014-01' --beamType 'cosmics' --autoConfiguration 'everything' \
--preExec r2e:'from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArHVCorr=False;jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr.set_Value_and_Lock(False);from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.doInnerDetectorCommissioning.set_Value_and_Lock(True);InDetFlags.useBroadClusterErrors.set_Value_and_Lock(False);DQMonFlags.doStreamAwareMon=False;DQMonFlags.enableLumiAccess=False;from JetRec.JetRecFlags import jetFlags;jetFlags.useTracks=False;DQMonFlags.doLVL1CaloMon=False;DQMonFlags.doCTPMon=False;' \
--outputESDFile='thetestESD.pool.root' \
--outputAODFile='thetestAOD.pool.root'  \
--outputHISTFile='thetestHIST.root' \
--triggerConfig=DATARECO:REPR \
> log.txt 2>&1

#version from DB once enableCostForCAF is set in a smk
#Trig_reco_tf.py \
#--inputBS_RDOFile=root://eosatlas//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1 \
#--jobOptionSvcType TrigConf::HLTJobOptionsSvc \
#--useDB TRUE --DBtype Coral --DBserver TRIGGERDBREPR \
#--DBsmkey 46 --DBhltpskey 33 --DBextra "{'lvl1key': 14}" \
#--ignoreErrors="True" --runNumber="253010" \
#--maxEvents=1 --skipEvents=2 \
#--outputHIST_HLTMONFile="HIST_HLTMON.04854087._000852.pool.root.1" \
#--asetup b2r:20.2.0.2,AtlasP1HLT r2c:AtlasProduction,20.1.4.2 r2e:AtlasProduction,20.1.4.2 e2a:AtlasProduction,20.1.4.2 \
#--outputNTUP_TRIGCOSTFile="trig_cost.root" \
#--outputNTUP_TRIGRATEFile="trig_rate.root" \
# --geometryVersion 'ATLAS-R2-2015-02-00-00' --conditionsTag 'CONDBR2-ES1PA-2014-01' --beamType 'cosmics' --autoConfiguration 'everything' \
# --preExec r2e:'from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArHVCorr=False;jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr.set_Value_and_Lock(False);from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.doInnerDetectorCommissioning.set_Value_and_Lock(True);InDetFlags.useBroadClusterErrors.set_Value_and_Lock(False);DQMonFlags.doStreamAwareMon=False;DQMonFlags.enableLumiAccess=False;from JetRec.JetRecFlags import jetFlags;jetFlags.useTracks=False;DQMonFlags.doLVL1CaloMon=False;DQMonFlags.doCTPMon=False;' \
# --outputESDFile='thetestESD.pool.root' \
# --outputAODFile='thetestAOD.pool.root'  \
# --outputHISTFile='thetestHIST.root' \
# --triggerConfig=MCRECO:DB:TRIGGERDBREPR:46,14,33 \
#> log.txt 2>&1

#--outputNTUP_TRIGEBWEIGHTFile="trig_ebweight.root" \

echo "***trf command***"
grep "command line was" log.txt
echo "***trf end status***"
tail log.txt -n 1
echo "***BSRDO->BS->COST->RECO completed***"

echo "---check costForCAF enabled in BS->BS---$ grep costForCAF log.BSRDOtoRAW"
grep costForCAF log.BSRDOtoRAW
echo "---costmon summary---$ grep TrigCostRun log.RAWtoCOST | grep POST_HLT"
grep TrigCostRun log.RAWtoCOST | grep POST_HLT
echo "---test costmon ntups created---$ ls NTUP_TRIG*"
ls NTUP_TRIG*
echo "---test metadata---$ grep lfn metadata.xml"
grep lfn metadata.xml

cd ..

fi

##############################################################################

if $runTest03 ; then

echo -e "\n******runTest03: BSRDO->BS via JSON******"

rm -rf runTest03_BSRDO_BS_JSON
mkdir runTest03_BSRDO_BS_JSON
cd runTest03_BSRDO_BS_JSON

echo "{\
\"outputBSFile\": \"thetestBS.RAW.data\", \
\"inputBS_RDOFile\": [\"root://eosatlas//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1\"], \
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
echo "***BSRDO->BS via JSON completed***"

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
\"inputBS_RDOFile\": [\"root://eosatlas//eos/atlas/atlastier0/rucio/data15_cos/debug_HLTSVForceAccept/00263146/data15_cos.00263146.debug_HLTSVForceAccept.daq.RAW/data15_cos.00263146.debug_HLTSVForceAccept.daq.RAW._lb0000._SFO-2._0001.data\"], \
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
echo "***BSRDO->Debug completed***"

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

fi

##############################################################################

if $runTest04 ; then

echo -e "\n******runTest04: BSRDO->BS with athenaopt******"

rm -rf runTest04_BSRDO_BS_athOpt
mkdir runTest04_BSRDO_BS_athOpt
cd runTest04_BSRDO_BS_athOpt

Trig_reco_tf.py \
--inputBS_RDOFile=root://eosatlas//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1 \
--jobOptionSvcType TrigConf::HLTJobOptionsSvc \
--useDB TRUE --DBtype Coral --DBserver TRIGGERDBREPR \
--DBsmkey 46 --DBhltpskey 33 --DBextra "{'lvl1key': 14}" \
--precommand "markTest=True" \
--ignoreErrors="True" --runNumber="253010" \
--maxEvents=1 --skipEvents=2 \
--outputHIST_HLTMONFile="HIST_HLTMON.04854087._000852.pool.root.1" \
--outputBSFile="thetestBS.RAW.data" \
--dumpOptions=True \
> logDBargs.txt 2>&1

echo "***trf command using DB args***"
grep "command line was" logDBargs.txt
echo "***now rerun athenaHLT to get options dump***"
cat runwrapper.BSRDOtoRAW.sh

. ./runwrapper.BSRDOtoRAW.sh > dumpDBargs.txt 2>&1

Trig_reco_tf.py \
--inputBS_RDOFile=root://eosatlas//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1 \
--athenaopts=" -J TrigConf::HLTJobOptionsSvc --db-type Coral --db-server TRIGGERDBREPR \
--use-database \
--db-smkey 46 --db-hltpskey 33 --db-extra \"{'lvl1key': 14}\" " \
--precommand "markTest=True" \
--ignoreErrors="True" --runNumber="253010" \
--maxEvents=1 --skipEvents=2 \
--outputHIST_HLTMONFile="HIST_HLTMON.04854087._000852.pool.root.1" \
--outputBSFile="thetestBS.RAW.data" \
--dumpOptions=True \
> logAthenaOpts.txt 2>&1

echo "***trf command using athenaopts***"
grep "command line was" logAthenaOpts.txt
echo "***now rerun athenaHLT to get options dump***"
cat runwrapper.BSRDOtoRAW.sh

. ./runwrapper.BSRDOtoRAW.sh > dumpAthenaOpts.txt 2>&1

echo "---compare dump with/without athenaopts---$ diff -s dumpDBargs.txt dumpAthenaOpts.txt --ignore-matching-lines=\"# User \""
diff -s dumpDBargs.txt dumpAthenaOpts.txt --ignore-matching-lines="# User "

cd ..

fi

##############################################################################

if $runTest05 ; then

echo -e "\n******runTest05: BSRDO->BS not from DB******"

rm -rf runTest05_BSRDO_BS_noDB
mkdir runTest05_BSRDO_BS_noDB
cd runTest05_BSRDO_BS_noDB

Trig_reco_tf.py \
--inputBS_RDOFile=root://eosatlas//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1 \
--precommand "testPhysicsV5=True" \
--ignoreErrors="True" --runNumber="253010" \
--maxEvents=1 --skipEvents=2 \
--outputHIST_HLTMONFile="HIST_HLTMON.04854087._000852.pool.root.1" \
--outputBSFile="thetestBS.RAW.data" \
> log.txt 2>&1

echo "***trf command***"
grep "command line was" log.txt
echo "***trf end status***"
tail log.txt -n 1
echo "***BSRDO->BS completed***"

echo "---test skipEvents (2)---$ grep Skipped.*event log.BSRDOtoRAW"
grep Skipped.*event log.BSRDOtoRAW 
echo "---test maxEvents (1)---$ grep Processed log.BSRDOtoRAW"
grep Processed log.BSRDOtoRAW
echo "---test menu (PhysicsV5)---$ grep setMenu log.BSRDOtoRAW"
grep setMenu log.BSRDOtoRAW
echo "---test BS file created---$ ls thetestBS.RAW.data"
ls thetestBS.RAW.data
echo "---test MON file created---$ ls HIST_HLTMON.04854087._000852.pool.root.1"
ls HIST_HLTMON.04854087._000852.pool.root.1
echo "---test metadata---$ grep lfn metadata.xml"
grep lfn metadata.xml

cd ..

fi

##############################################################################

echo -e "***End time***"
date
