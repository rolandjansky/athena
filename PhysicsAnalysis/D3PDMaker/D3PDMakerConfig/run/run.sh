rm -f NTUP_COMMON.root


Reco_trf.py preExec="from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag='BTagCalibALL-07-07';from RecExConfig.RecAlgsFlags import recAlgs;recAlgs.doMissingET.set_Value_and_Lock(False)" inputAODFile=root://eosatlas.cern.ch//eos/atlas/user/l/lezhou/data12_8TeV.00208811.physics_JetTauEtmiss.merge.AOD.r4042_p1263_tid01016451_00/AOD.01016451._000059.pool.root autoConfiguration=everything maxEvents=100 --ignoreerrors=False outputNTUP_COMMONFile=NTUP_COMMON.root postExec='print "dump sequence";from AthenaCommon.AlgSequence import *;dumpSequence(topSequence); print "ssssssssssssss D3PDMakerFlags.TruthWriteHadrons = ", D3PDMakerFlags.TruthWriteHadrons'


#rec.doApplyAODFix.set_Value_and_Lock(False);

#Reco_trf.py preExec="from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag='BTagCalibALL-07-02'" inputAODFile=AOD.01203354._000045.pool.root.1 autoConfiguration=everything maxEvents=100 --ignoreerrors=False outputNTUP_COMMONFile=NTUP_COMMON.mc.root

