rm -f NTUP_COMMON.mc.root


Reco_trf.py preExec="from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag='BTagCalibALL-07-07';from RecExConfig.RecAlgsFlags import recAlgs;recAlgs.doMissingET.set_Value_and_Lock(False)" inputAODFile=root://eosatlas//eos/atlas/user/l/lezhou/mc12_8TeV.105204.McAtNloJimmy_AUET2CT10_ttbar_allhad.merge.AOD.e1576_s1499_s1504_r3658_r3549_tid01012790_00/AOD.01012790._000023.pool.root autoConfiguration=everything maxEvents=100 --ignoreerrors=False outputNTUP_COMMONFile=NTUP_COMMON.mc.root postExec='print "dump sequence";from AthenaCommon.AlgSequence import *;dumpSequence(topSequence); print "ssssssssssssss D3PDMakerFlags.TruthWriteHadrons = ", D3PDMakerFlags.TruthWriteHadrons'


#rec.doApplyAODFix.set_Value_and_Lock(False);

#Reco_trf.py preExec="from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag='BTagCalibALL-07-02'" inputAODFile=AOD.01203354._000045.pool.root.1 autoConfiguration=everything maxEvents=100 --ignoreerrors=False outputNTUP_COMMONFile=NTUP_COMMON.mc.root

