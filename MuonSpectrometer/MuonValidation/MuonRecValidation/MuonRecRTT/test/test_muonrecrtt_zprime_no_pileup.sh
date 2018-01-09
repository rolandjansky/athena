#!/bin/sh
#
# art-description: Run reconstruction on a input RDO file, migrated from RTT job: MuonRecRTT_ZPrime_no_pileup
#
# art-type: grid
# art-input: user.zhidong.valid1.361405.Sherpa_CT10_Zmumu_Pt280_500_CVetoBVeto.recon.RDO.no_pileup.e5112_s3195_tid12019961_00_EXT0
# art-input-nfiles: 10
# art-input-split: 10

Reco_tf.py --maxEvents=10000 --inputRDOFile=*.root --outputAODFile=valid1.361405.Sherpa_CT10_Zmumu_Pt280_500_CVetoBVeto_no_pileup.AOD.pool.root --outputNTUP_PHYSVALFile valid1.361405.Sherpa_CT10_Zmumu_Pt280_500_CVetoBVeto_no_pileup.PHYSVAL.root --validationFlags noExample doMuon --preExec 'from RecExConfig.RecFlags  import rec; rec.doTrigger=False; import MuonCombinedRecExample.MuonCombinedRecOnlySetup'
