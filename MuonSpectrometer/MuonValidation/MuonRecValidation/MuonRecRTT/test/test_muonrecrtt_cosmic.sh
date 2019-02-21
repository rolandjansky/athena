#!/bin/sh
#
# art-description: Test the reconstruction of muon cosmic samples.
#
# art-type: grid
# art-include: master/Athena
# art-include: 21.0/Athena
# art-input: user.zhidong.data17_cos.00342172.physics_CosmicMuons.merge.RAW_subset01
# art-input-nfiles: 1

set -x

echo "List of files = " ${ArtInFile}

Reco_tf.py --maxEvents=9000 --conditionsTag RAWtoESD:CONDBR2-BLKPA-2017-12 --geometryVersion ATLAS-R2-2016-01-00-01 --inputBSFile=${ArtInFile} --outputAODFile=MuonCosmic_Reco.AOD.pool.root --preExec 'from RecExConfig.RecFlags  import rec; rec.doTrigger=False; import MuonCombinedRecExample.MuonCombinedRecOnlySetup' --postExec 'r2e:topSequence.BeamBackgroundFiller.Enable = False'
