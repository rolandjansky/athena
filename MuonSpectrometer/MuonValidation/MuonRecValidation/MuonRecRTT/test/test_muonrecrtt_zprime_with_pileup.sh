#!/bin/sh
#
# art-description: Run reconstruction on a input RDO file, migrated from RTT job: MuonRecRTT_ZPrime_with_pileup
#
# art-type: grid
# art-input: valid1.361405.Sherpa_CT10_Zmumu_Pt280_500_CVetoBVeto.recon.RDO_TRIG.e5112_s3195_r9723_tid12019967_00
# art-input-nfiles: 10
# art-input-split: 10

Reco_tf.py --maxEvents=5000 --inputRDOFile=*.root.1 --outputAODFile=valid1.361405.Sherpa_CT10_Zmumu_Pt280_500_CVetoBVeto_with_pileup.AOD.pool.root --outputNTUP_PHYSVALFile valid1.361405.Sherpa_CT10_Zmumu_Pt280_500_CVetoBVeto_with_pileup.PHYSVAL.root --validationFlags noExample doMuon --preExec 'from RecExConfig.RecFlags  import rec; rec.doTrigger=False; import MuonCombinedRecExample.MuonCombinedRecOnlySetup'
