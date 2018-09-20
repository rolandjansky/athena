#!/bin/sh
#
# art-description: Test the reconstruction of muon samples for toroid off.
#
# art-type: grid
# art-include: master/Athena
# art-include: 21.0/Athena
# art-input: user.zhidong.data17_13TeV.00325028.physics_Main.daq.RAW_subset01
# art-input-nfiles: 3

set -x

fileList="['${ArtInFile//,/', '}']"

Reco_tf.py --maxEvents=7500 --conditionsTag RAWtoESD:CONDBR2-BLKPA-2017-12 --geometryVersion ATLAS-R2-2016-01-00-01 --inputBSFile=$fileList --outputAODFile=MuonToroidOff_Reco.AOD.pool.root --preExec 'from RecExConfig.RecFlags  import rec; rec.doTrigger=False; import MuonCombinedRecExample.MuonCombinedRecOnlySetup'
