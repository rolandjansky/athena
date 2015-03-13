
echo "***begin testing with BSRDO->BS"

rm -rf tmpTestDirBSRDO_BS
mkdir tmpTestDirBSRDO_BS
cd tmpTestDirBSRDO_BS

Trig_reco_tf.py \
--inputBS_RDOFile=root://eosatlas//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1 \
--joboptionsvc-type TrigConf::HLTJobOptionsSvc \
--use-database TRUE --db-type Coral --db-server TRIGGERDBREPR \
--db-smkey 11 --db-hltpskey 7 --db-extra "{'lvl1key': 4}" \
--precommand "rerunLVL1=True;markTest=True" \
--ignoreErrors="True" --runNumber="253010" \
--maxEvents=1 --skipEvents=2 \
--outputHIST_HLTMONFile="HIST_HLTMON.04854087._000852.pool.root.1" \
--outputBSFile="temp.BS" \
> log.txt 2>&1

#--dumpOptions=True \

echo "***trf end status"
tail log.txt -n 1
echo "***BSRDO->BS completed"

echo "---test skipEvents (2), grep Skipp log.BSRDOtoRAW:"
grep Skipp log.BSRDOtoRAW 
echo "---test maxEvents (1), grep Processed log.BSRDOtoRAW:"
grep Processed log.BSRDOtoRAW
echo "---test smkey (11) hltpskey (7) lvlkey (4) server (TRIGGERDBREPR), grep TrigDBConnectionConfig log.BSRDOtoRAW:"
grep TrigDBConnectionConfig log.BSRDOtoRAW
echo "---test BS file created, ls temp.BS.*"
ls temp.BS.*
echo "---test MON file created, ls HIST_HLTMON.04854087._000852.pool.root.1"
ls HIST_HLTMON.04854087._000852.pool.root.1
echo "---test metadata: grep lfn metadata.xml"
grep lfn metadata.xml

cd ..

echo "***next test BSRDO->BS->ESD"

rm -rf tmpTestDirBSRDO_ESD
mkdir tmpTestDirBSRDO_ESD
cd tmpTestDirBSRDO_ESD


Trig_reco_tf.py \
--inputBS_RDOFile=root://eosatlas//eos/atlas/atlasdatadisk/rucio/data15_cos/fe/0c/data15_cos.00253010.physics_IDCosmic.merge.RAW._lb0010._SFO-ALL._0001.1 \
--joboptionsvc-type TrigConf::HLTJobOptionsSvc \
--use-database TRUE --db-type Coral --db-server TRIGGERDBREPR \
--db-smkey 11 --db-hltpskey 7 --db-extra "{'lvl1key': 4}" \
--precommand "rerunLVL1=True;markTest=True" \
--ignoreErrors="True" --runNumber="253010" \
--maxEvents=1 --skipEvents=2 \
--outputHIST_HLTMONFile="HIST_HLTMON.04854087._000852.pool.root.1" \
--asetup r2e:AtlasProduction,20.1.3.3 \
--geometryVersion 'ATLAS-R2-2015-02-00-00' --conditionsTag 'CONDBR2-ES1PA-2014-01' --beamType 'cosmics' --autoConfiguration 'everything' \
--preExec r2e:'from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArHVCorr=False;jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr.set_Value_and_Lock(False);from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.doInnerDetectorCommissioning.set_Value_and_Lock(True);InDetFlags.useBroadClusterErrors.set_Value_and_Lock(False);DQMonFlags.doStreamAwareMon=False;DQMonFlags.enableLumiAccess=False;from JetRec.JetRecFlags import jetFlags;jetFlags.useTracks=False;DQMonFlags.doLVL1CaloMon=False;DQMonFlags.doCTPMon=False;' \
--outputESDFile="thetestrecoESD.pool.root" \
--triggerConfig r2e=MCRECO:DB:TRIGGERDBREPR:11,4,7 \
> log.txt 2>&1


#--outputBSFile="temp.BS" \
#--outputAODFile=thetestrecoAOD.pool.root 
#--outputHISTFile=testHIST.root 

echo "***trf end status"
tail log.txt -n 1
echo "***BSRDO->BS->ESD completed"

echo "---test triggerConfig, grep TriggerFlags log.BSRDOtoRAW | grep triggerConfig log.BSRDOtoRAW:"
grep TriggerFlags log.RAWtoESD | grep triggerConfig log.RAWtoESD
echo "---test metadata: grep lfn metadata.xml"
grep lfn metadata.xml
echo "---test BS file created, ls tmp.BS.*"
ls tmp.BS.*
echo "---test MON file created, ls HIST_HLTMON.04854087._000852.pool.root.1"
ls HIST_HLTMON.04854087._000852.pool.root.1
echo "---test ESD file created, ls thetestrecoESD.pool.root"
ls thetestrecoESD.pool.root


cd ..






