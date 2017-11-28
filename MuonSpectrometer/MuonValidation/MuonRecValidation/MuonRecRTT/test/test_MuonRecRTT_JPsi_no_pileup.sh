#!/bin/sh
#
# art-description: Run reconstruction on a input RDO file, migrated from RTT job: MuonRecRTT_JPsi_no_pileup
#
# art-type: grid

Reco_tf.py --maxEvents=500 --inputRDOFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/MuonRecRTT/* --outputAODFile=my.AOD.pool.root --outputNTUP_PHYSVALFile my.PHYSVAL.root --validationFlags noExample doMuon --preExec 'from RecExConfig.RecFlags  import rec; rec.doTrigger=False; import MuonCombinedRecExample.MuonCombinedRecOnlySetup'
