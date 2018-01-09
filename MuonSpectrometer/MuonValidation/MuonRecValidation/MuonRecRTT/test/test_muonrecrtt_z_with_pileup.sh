#!/bin/sh
#
# art-description: Run reconstruction on a input RDO file, migrated from RTT job: MuonRecRTT_Z_with_pileup
#
# art-type: grid
# art-input: valid1.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.recon.RDO_TRIG.e5112_s3195_r9723_tid12019822_00
# art-input-nfiles: 10
# art-input-split: 10

Reco_tf.py --maxEvents=5000 --inputRDOFile=*.root.1 --outputAODFile=valid1.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu_with_pileup.AOD.pool.root --outputNTUP_PHYSVALFile valid1.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu_with_pileup.PHYSVAL.root --validationFlags noExample doMuon --preExec 'from RecExConfig.RecFlags  import rec; rec.doTrigger=False; import MuonCombinedRecExample.MuonCombinedRecOnlySetup'	      
