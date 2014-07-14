rm -f NTUP_COMMON.mc.root


Reco_trf.py preExec="from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag='BTagCalibALL-07-08';from RecExConfig.RecAlgsFlags import recAlgs;recAlgs.doMissingET.set_Value_and_Lock(False)" inputAODFile=root://eosatlas//eos/atlas/user/l/lezhou/mc12_8TeV.110002.McAtNloJimmy_AUET2CT10_ttbar_170GeV_dilepton.merge.AOD.e1576_a159_a171_r3549_tid01023276_00/AOD.01023276._000537.pool.root.1 autoConfiguration=everything maxEvents=3 --ignoreerrors=False outputNTUP_COMMONFile=NTUP_COMMON.mc.root postExec='print "dump sequence";from AthenaCommon.AlgSequence import *;dumpSequence(topSequence); print "ssssssssssssss D3PDMakerFlags.TruthWriteHadrons = ", D3PDMakerFlags.TruthWriteHadrons'


#rec.doApplyAODFix.set_Value_and_Lock(False);

#Reco_trf.py preExec="from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag='BTagCalibALL-07-02'" inputAODFile=AOD.01203354._000045.pool.root.1 autoConfiguration=everything maxEvents=100 --ignoreerrors=False outputNTUP_COMMONFile=NTUP_COMMON.mc.root

