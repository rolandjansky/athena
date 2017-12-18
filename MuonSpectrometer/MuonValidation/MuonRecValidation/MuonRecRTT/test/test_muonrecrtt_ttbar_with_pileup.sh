#!/bin/sh
#
# art-description: Run reconstruction on a input RDO file, migrated from RTT job: MuonRecRTT_ttbar_with_pileup
#
# art-type: grid
# art-input: valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.RDO_TRIG.e4993_s3195_r9723_tid12020031_00
# art-input-nfiles: 10
# art-input-split: 10

Reco_tf.py --maxEvents=5000 --inputRDOFile=*.root.1 --outputAODFile=valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad_with_pileup.AOD.pool.root --outputNTUP_PHYSVALFile valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad_with_pileup.PHYSVAL.root --validationFlags noExample doMuon --preExec 'from RecExConfig.RecFlags  import rec; rec.doTrigger=False; import MuonCombinedRecExample.MuonCombinedRecOnlySetup'
