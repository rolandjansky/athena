
echo "\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n"

SUFF='nothing'
echo $SUFF
time Reco_trf.py inputBSFile=/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data maxEvents=10 trigStream=IDCosmic autoConfiguration=FieldAndGeo,BeamType,ConditionsTag preExec='rec.doCalo=False',,'rec.doInDet=False',,'rec.doMuon=False',,'rec.doJetMissingETTag=False',,'rec.doEgamma=False',,'rec.doMuonCombined=False',,'rec.doTau=False',,'rec.doTrigger=False' preInclude=RecExCommon/RecoUsefulFlags.py,RecExCommission/RecExCommissionRepro.py,RecExCommission/MinimalCommissioningSetup.py,RecJobTransforms/debugConfig.py,RecJobTransforms/UseOracle.py  postInclude=RecJobTransforms/reducedRegionSelectorPostConfig.py   --ignoreunknown --athenaopts=-s   > rawtoesd_$SUFF.log
grep -e "so far" -e "Leaving with" -e "JobTransform completed" rawtoesd_$SUFF.log

echo "\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n"
SUFF='id'
echo $SUFF
time Reco_trf.py inputBSFile=/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data maxEvents=10 trigStream=IDCosmic autoConfiguration=FieldAndGeo,BeamType,ConditionsTag preExec='rec.doCalo=False',,'rec.doInDet=True',,'rec.doMuon=False',,'rec.doJetMissingETTag=False',,'rec.doEgamma=False',,'rec.doMuonCombined=False',,'rec.doTau=False',,'rec.doTrigger=False' preInclude=RecExCommon/RecoUsefulFlags.py,RecExCommission/RecExCommissionRepro.py,RecExCommission/MinimalCommissioningSetup.py,RecJobTransforms/debugConfig.py,RecJobTransforms/UseOracle.py postInclude=RecJobTransforms/reducedRegionSelectorPostConfig.py   --ignoreunknown --athenaopts=-s   > rawtoesd_$SUFF.log
grep -e "so far" -e "Leaving with" -e "JobTransform completed" rawtoesd_$SUFF.log

echo "\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n"
SUFF='calo'
echo $SUFF
time Reco_trf.py inputBSFile=/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data maxEvents=10 trigStream=IDCosmic autoConfiguration=FieldAndGeo,BeamType,ConditionsTag preExec='rec.doInDet=False',,'rec.doMuon=False',,'rec.doJetMissingETTag=False',,'rec.doEgamma=False',,'rec.doMuonCombined=False',,'rec.doTau=False',,'rec.doTrigger=False' preInclude=RecExCommon/RecoUsefulFlags.py,RecExCommission/RecExCommissionRepro.py,RecExCommission/MinimalCommissioningSetup.py,RecJobTransforms/debugConfig.py,RecJobTransforms/UseOracle.py postInclude=RecJobTransforms/reducedRegionSelectorPostConfig.py   --ignoreunknown --athenaopts=-s   > rawtoesd_$SUFF.log
grep -e "so far" -e "Leaving with" -e "JobTransform completed" rawtoesd_$SUFF.log


echo "\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n"
SUFF='muon'
echo $SUFF
time Reco_trf.py inputBSFile=/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data maxEvents=10 trigStream=IDCosmic autoConfiguration=FieldAndGeo,BeamType,ConditionsTag preExec='rec.doCalo=False',,'rec.doInDet=False',,'rec.doMuon=True',,'rec.doJetMissingETTag=False',,'rec.doEgamma=False',,'rec.doMuonCombined=False',,'rec.doTau=False',,'rec.doTrigger=False' preInclude=RecExCommon/RecoUsefulFlags.py,RecExCommission/RecExCommissionRepro.py,RecExCommission/MinimalCommissioningSetup.py,RecJobTransforms/debugConfig.py,RecJobTransforms/UseOracle.py postInclude=RecJobTransforms/reducedRegionSelectorPostConfig.py   --ignoreunknown --athenaopts=-s   > rawtoesd_$SUFF.log
grep -e "so far" -e "Leaving with" -e "JobTransform completed" rawtoesd_$SUFF.log

echo "\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n"
SUFF='idmuon'
echo $SUFF
time Reco_trf.py inputBSFile=/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data maxEvents=10 trigStream=IDCosmic autoConfiguration=FieldAndGeo,BeamType,ConditionsTag preExec='rec.doCalo=False',,'rec.doInDet=True',,'rec.doMuon=True',,'rec.doJetMissingETTag=False',,'rec.doEgamma=False',,'rec.doMuonCombined=False',,'rec.doTau=False',,'rec.doTrigger=False' preInclude=RecExCommon/RecoUsefulFlags.py,RecExCommission/RecExCommissionRepro.py,RecExCommission/MinimalCommissioningSetup.py,RecJobTransforms/debugConfig.py,RecJobTransforms/UseOracle.py postInclude=RecJobTransforms/reducedRegionSelectorPostConfig.py   --ignoreunknown --athenaopts=-s   > rawtoesd_$SUFF.log
grep -e "so far" -e "Leaving with" -e "JobTransform completed" rawtoesd_$SUFF.log

echo "\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n"
SUFF='idmuoncalo'
echo $SUFF
time Reco_trf.py inputBSFile=/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data maxEvents=10 trigStream=IDCosmic autoConfiguration=FieldAndGeo,BeamType,ConditionsTag preExec='rec.doCalo=True',,'rec.doInDet=True',,'rec.doMuon=True',,'rec.doJetMissingETTag=False',,'rec.doEgamma=False',,'rec.doMuonCombined=False',,'rec.doTau=False',,'rec.doTrigger=False' preInclude=RecExCommon/RecoUsefulFlags.py,RecExCommission/RecExCommissionRepro.py,RecExCommission/MinimalCommissioningSetup.py,RecJobTransforms/debugConfig.py,RecJobTransforms/UseOracle.py postInclude=RecJobTransforms/reducedRegionSelectorPostConfig.py   --ignoreunknown --athenaopts=-s   > rawtoesd_$SUFF.log
grep -e "so far" -e "Leaving with" -e "JobTransform completed" rawtoesd_$SUFF.log

echo "\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n"
SUFF='idmuoncalotrig'
echo $SUFF
time Reco_trf.py inputBSFile=/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data maxEvents=10 trigStream=IDCosmic autoConfiguration=FieldAndGeo,BeamType,ConditionsTag preExec='rec.doCalo=True',,'rec.doInDet=True',,'rec.doMuon=True',,'rec.doJetMissingETTag=False',,'rec.doEgamma=False',,'rec.doMuonCombined=False',,'rec.doTau=False',,'rec.doTrigger=True' preInclude=RecExCommon/RecoUsefulFlags.py,RecExCommission/RecExCommissionRepro.py,RecExCommission/MinimalCommissioningSetup.py,RecJobTransforms/debugConfig.py,RecJobTransforms/UseOracle.py postInclude=RecJobTransforms/reducedRegionSelectorPostConfig.py   --ignoreunknown --athenaopts=-s   > rawtoesd_$SUFF.log
grep -e "so far" -e "Leaving with" -e "JobTransform completed" rawtoesd_$SUFF.log


echo "\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n"
SUFF='idmuoncalotrigmuoncombined'
echo $SUFF
time Reco_trf.py inputBSFile=/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data maxEvents=10 trigStream=IDCosmic autoConfiguration=FieldAndGeo,BeamType,ConditionsTag preExec='rec.doCalo=True',,'rec.doInDet=True',,'rec.doMuon=True',,'rec.doJetMissingETTag=False',,'rec.doEgamma=False',,'rec.doMuonCombined=True',,'rec.doTau=False',,'rec.doTrigger=True' preInclude=RecExCommon/RecoUsefulFlags.py,RecExCommission/RecExCommissionRepro.py,RecExCommission/MinimalCommissioningSetup.py,RecJobTransforms/debugConfig.py,RecJobTransforms/UseOracle.py postInclude=RecJobTransforms/reducedRegionSelectorPostConfig.py   --ignoreunknown --athenaopts=-s   > rawtoesd_$SUFF.log
grep -e "so far" -e "Leaving with" -e "JobTransform completed" rawtoesd_$SUFF.log

echo "\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n"
SUFF='idmuoncalotrigegamma'
echo $SUFF
time Reco_trf.py inputBSFile=/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data maxEvents=10 trigStream=IDCosmic autoConfiguration=FieldAndGeo,BeamType,ConditionsTag preExec='rec.doCalo=True',,'rec.doInDet=True',,'rec.doMuon=True',,'rec.doJetMissingETTag=False',,'rec.doEgamma=True',,'rec.doMuonCombined=False',,'rec.doTau=False',,'rec.doTrigger=True' preInclude=RecExCommon/RecoUsefulFlags.py,RecExCommission/RecExCommissionRepro.py,RecExCommission/MinimalCommissioningSetup.py,RecJobTransforms/debugConfig.py,RecJobTransforms/UseOracle.py postInclude=RecJobTransforms/reducedRegionSelectorPostConfig.py   --ignoreunknown --athenaopts=-s   > rawtoesd_idmuoncalotrigegamma.log
grep -e "so far" -e "Leaving with" -e "JobTransform completed" rawtoesd_$SUFF.log

echo "\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n"
SUFF='idmuoncalotrigegammamucomb'
echo $SUFF
time Reco_trf.py inputBSFile=/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data maxEvents=10 trigStream=IDCosmic autoConfiguration=FieldAndGeo,BeamType,ConditionsTag preExec='rec.doCalo=True',,'rec.doInDet=True',,'rec.doMuon=True',,'rec.doJetMissingETTag=False',,'rec.doEgamma=True',,'rec.doMuonCombined=True',,'rec.doTau=False',,'rec.doTrigger=True' preInclude=RecExCommon/RecoUsefulFlags.py,RecExCommission/RecExCommissionRepro.py,RecExCommission/MinimalCommissioningSetup.py,RecJobTransforms/debugConfig.py,RecJobTransforms/UseOracle.py postInclude=RecJobTransforms/reducedRegionSelectorPostConfig.py   --ignoreunknown --athenaopts=-s   > rawtoesd_$SUFF.log
grep -e "so far" -e "Leaving with" -e "JobTransform completed" rawtoesd_$SUFF.log

echo "\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n"
SUFF='idmuoncalotrigegammamucombjet'
echo $SUFF
time Reco_trf.py inputBSFile=/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data maxEvents=10 trigStream=IDCosmic autoConfiguration=FieldAndGeo,BeamType,ConditionsTag preExec='rec.doCalo=True',,'rec.doInDet=True',,'rec.doMuon=True',,'rec.doJetMissingETTag=True',,'rec.doEgamma=True',,'rec.doMuonCombined=True',,'rec.doTau=False',,'rec.doTrigger=True' preInclude=RecExCommon/RecoUsefulFlags.py,RecExCommission/RecExCommissionRepro.py,RecExCommission/MinimalCommissioningSetup.py,RecJobTransforms/debugConfig.py,RecJobTransforms/UseOracle.py postInclude=RecJobTransforms/reducedRegionSelectorPostConfig.py   --ignoreunknown --athenaopts=-s   > rawtoesd_$SUFF.log
grep -e "so far" -e "Leaving with" -e "JobTransform completed" rawtoesd_$SUFF.log

echo "\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n"
SUFF='idmuoncalotrigegammamucombjettau'
echo $SUFF
time Reco_trf.py inputBSFile=/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data maxEvents=10 trigStream=IDCosmic autoConfiguration=FieldAndGeo,BeamType,ConditionsTag preExec='rec.doCalo=True',,'rec.doInDet=True',,'rec.doMuon=True',,'rec.doJetMissingETTag=True',,'rec.doEgamma=True',,'rec.doMuonCombined=True',,'rec.doTau=True',,'rec.doTrigger=True' preInclude=RecExCommon/RecoUsefulFlags.py,RecExCommission/RecExCommissionRepro.py,RecExCommission/MinimalCommissioningSetup.py,RecJobTransforms/debugConfig.py,RecJobTransforms/UseOracle.py postInclude=RecJobTransforms/reducedRegionSelectorPostConfig.py   --ignoreunknown --athenaopts=-s   > rawtoesd_$SUFF.log
grep -e "so far" -e "Leaving with" -e "JobTransform completed" rawtoesd_$SUFF.log

echo "\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n"
SUFF='idmuoncalotrigegammamucombjettaumon'
echo $SUFF
time Reco_trf.py inputBSFile=/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data maxEvents=10 trigStream=IDCosmic autoConfiguration=FieldAndGeo,BeamType,ConditionsTag preExec='rec.doCalo=True',,'rec.doInDet=True',,'rec.doMuon=True',,'rec.doJetMissingETTag=True',,'rec.doEgamma=True',,'rec.doMuonCombined=True',,'rec.doTau=True',,'rec.doTrigger=True' preInclude=RecExCommon/RecoUsefulFlags.py,RecExCommission/RecExCommissionRepro.py,RecExCommission/MinimalCommissioningSetup.py,RecJobTransforms/debugConfig.py,RecJobTransforms/UseOracle.py postInclude=RecJobTransforms/reducedRegionSelectorPostConfig.py  HIST=myMergedMonitoring.root  --ignoreunknown --athenaopts=-s   >! rawtoesd_$SUFF.log
grep -e "so far" -e "Leaving with" -e "JobTransform completed" rawtoesd_$SUFF.log

echo "\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n"
SUFF='idmuoncalotrigegammamucombjettaumonmuoncalib'
echo $SUFF
time Reco_trf.py inputBSFile=/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data maxEvents=10 trigStream=IDCosmic autoConfiguration=FieldAndGeo,BeamType,ConditionsTag preExec='rec.doCalo=True',,'rec.doInDet=True',,'rec.doMuon=True',,'rec.doJetMissingETTag=True',,'rec.doEgamma=True',,'rec.doMuonCombined=True',,'rec.doTau=True',,'rec.doTrigger=True' preInclude=RecExCommon/RecoUsefulFlags.py,RecExCommission/RecExCommissionRepro.py,RecExCommission/MinimalCommissioningSetup.py,RecJobTransforms/debugConfig.py,RecJobTransforms/UseOracle.py postInclude=RecJobTransforms/reducedRegionSelectorPostConfig.py  outputMuonCalibNtup=muonCalib.root HIST=myMergedMonitoring.root  --ignoreunknown --athenaopts=-s   >! rawtoesd_$SUFF.log
grep -e "so far" -e "Leaving with" -e "JobTransform completed" rawtoesd_$SUFF.log

echo "\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n"
SUFF='all' 
echo $SUFF
time Reco_trf.py inputBSFile=/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data maxEvents=10 trigStream=IDCosmic autoConfiguration=FieldAndGeo,BeamType,ConditionsTag  preInclude=RecExCommon/RecoUsefulFlags.py,RecExCommission/RecExCommissionRepro.py,RecExCommission/MinimalCommissioningSetup.py,RecJobTransforms/debugConfig.py,RecJobTransforms/UseOracle.py postInclude=RecJobTransforms/reducedRegionSelectorPostConfig.py   outputESDFile=myESD.pool.root  outputMuonCalibNtup=muonCalib.root HIST=myMergedMonitoring.root RAW_IDPROJCOMM=IDPROJCOMM._0001.data outputTAGComm=myTAGCOMM.root --ignoreunknown --athenaopts=-s   >! rawtoesd_$SUFF.log
grep -e "so far" -e "Leaving with" -e "JobTransform completed" rawtoesd_$SUFF.log
