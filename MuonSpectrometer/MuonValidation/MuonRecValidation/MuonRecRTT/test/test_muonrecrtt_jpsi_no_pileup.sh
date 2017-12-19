#!/bin/sh
#
# art-description: Run reconstruction on a input RDO file, migrated from RTT job: MuonRecRTT_JPsi_no_pileup
#
# art-type: grid
# art-input: user.zhidong.valid1.424100.Pythia8B_A14_CTEQ6L1_Jpsimu4mu4.RDO.recon.no_pileup.e5112_s3195_tid12019932_01_EXT0
# art-input-nfiles: 10
# art-input-split: 10

Reco_tf.py --maxEvents=10000 --inputRDOFile=*.root --outputAODFile=valid1.424100.Pythia8B_A14_CTEQ6L1_Jpsimu4mu4_no_pileup.AOD.pool.root --outputNTUP_PHYSVALFile valid1.424100.Pythia8B_A14_CTEQ6L1_Jpsimu4mu4_no_pileup.PHYSVAL.root --validationFlags noExample doMuon --preExec 'from RecExConfig.RecFlags  import rec; rec.doTrigger=False; import MuonCombinedRecExample.MuonCombinedRecOnlySetup'
