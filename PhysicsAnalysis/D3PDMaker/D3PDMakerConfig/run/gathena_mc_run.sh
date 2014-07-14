rm -f NTUP_COMMON.mc.root

mypreexec="from@BTagging.BTaggingFlags@import@BTaggingFlags;BTaggingFlags.CalibrationTag='BTagCalibALL-07-07';from@RecExConfig.RecAlgsFlags@import@recAlgs;recAlgs.doMissingET.set_Value_and_Lock(False)"

gathena --profilerMode eventLoop --profilerInitEvent 21 --profilerOutput my.athena.data.profile  --trf "Reco_trf.py preExec=${mypreexec} autoConfiguration=everything maxEvents=120 inputAODFile=root://eosatlas//eos/atlas/user/l/lezhou/mc12_8TeV.110002.McAtNloJimmy_AUET2CT10_ttbar_170GeV_dilepton.merge.AOD.e1576_a159_a171_r3549_tid01023276_00/AOD.01023276._000537.pool.root.1 --ignoreerrors=True outputNTUP_COMMONFile=NTUP_COMMON.mc.root"

