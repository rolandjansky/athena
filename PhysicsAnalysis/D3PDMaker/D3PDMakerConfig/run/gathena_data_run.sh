rm -f NTUP_COMMON.root

mypreexec="from@BTagging.BTaggingFlags@import@BTaggingFlags;BTaggingFlags.CalibrationTag='BTagCalibALL-07-07';from@RecExConfig.RecAlgsFlags@import@recAlgs;recAlgs.doMissingET.set_Value_and_Lock(False)"
myposexec="from@AthenaCommon.AlgSequence@import@*;dumpSequence(topSequence)"

gathena --profilerMode eventLoop --profilerInitEvent 21 --profilerOutput my.athena.data.profile  --trf "Reco_trf.py preExec=${mypreexec} postExec=${myposexec} autoConfiguration=everything maxEvents=120 inputAODFile=root://eosatlas//eos/atlas/user/l/lezhou/data12_8TeV.00208811.physics_JetTauEtmiss.merge.AOD.r4042_p1263_tid01016451_00/AOD.01016451._000059.pool.root --ignoreerrors=True outputNTUP_COMMONFile=NTUP_COMMON.root"