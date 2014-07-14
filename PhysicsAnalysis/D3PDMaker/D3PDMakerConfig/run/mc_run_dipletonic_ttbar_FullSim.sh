rm -f NTUP_COMMON.mc.root


Reco_trf.py preExec="from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag='BTagCalibALL-07-07';from RecExConfig.RecAlgsFlags import recAlgs;recAlgs.doMissingET.set_Value_and_Lock(False)" inputAODFile=/afs/cern.ch/work/l/lezhou/D3PD_making/mc12_8TeV.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.merge.AOD.e1513_s1499_s1504_r3658_r3549_tid01012783_00/AOD.01012783._001768.pool.root.1 autoConfiguration=everything maxEvents=100 --ignoreerrors=False outputNTUP_COMMONFile=NTUP_COMMON.mc.root postExec='print "dump sequence";from AthenaCommon.AlgSequence import *;dumpSequence(topSequence); print "ssssssssssssss D3PDMakerFlags.TruthWriteHadrons = ", D3PDMakerFlags.TruthWriteHadrons'


#rec.doApplyAODFix.set_Value_and_Lock(False);

#Reco_trf.py preExec="from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag='BTagCalibALL-07-02'" inputAODFile=AOD.01203354._000045.pool.root.1 autoConfiguration=everything maxEvents=100 --ignoreerrors=False outputNTUP_COMMONFile=NTUP_COMMON.mc.root

