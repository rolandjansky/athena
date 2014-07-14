rm -f NTUP_COMMON.rtt.root


Reco_trf.py preExec="from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag='BTagCalibALL-07-05';from RecExConfig.RecAlgsFlags import recAlgs;recAlgs.doMissingET.set_Value_and_Lock(False)" inputAODFile=root://eosatlas.cern.ch//eos/atlas/user/g/gbrooijm/dpd-rtt/data12_8TeV.00206299.physics_Egamma.merge.AOD.f450_m1170._lb0069._0001.1 autoConfiguration=everything maxEvents=100 --ignoreerrors=False outputNTUP_COMMONFile=NTUP_COMMON.rtt.root postExec='print "dump sequence";from AthenaCommon.AlgSequence import *;dumpSequence(topSequence); print "ssssssssssssss D3PDMakerFlags.TruthWriteHadrons = ", D3PDMakerFlags.TruthWriteHadrons'


#rec.doApplyAODFix.set_Value_and_Lock(False);

#Reco_trf.py preExec="from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag='BTagCalibALL-07-02'" inputAODFile=AOD.01203354._000045.pool.root.1 autoConfiguration=everything maxEvents=100 --ignoreerrors=False outputNTUP_COMMONFile=NTUP_COMMON.mc.root

