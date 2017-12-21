#!/bin/sh
#
# art-description: Run reconstruction on a input RDO file, migrated from RTT job: MuonRecRTT_ttbar_no_pileup
#
# art-type: grid
# art-input: user.zhidong.valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.RDO.no_pileup.e4993_s3195_tid12020024_01_EXT0
# art-input-nfiles: 10
# art-input-split: 10

Reco_tf.py --maxEvents=10000 --inputRDOFile=*.root --outputAODFile=valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad_no_pileup.AOD.pool.root --outputNTUP_PHYSVALFile valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad_no_pileup.PHYSVAL.root --validationFlags noExample doMuon --preExec 'from RecExConfig.RecFlags  import rec; rec.doTrigger=False; import MuonCombinedRecExample.MuonCombinedRecOnlySetup'
