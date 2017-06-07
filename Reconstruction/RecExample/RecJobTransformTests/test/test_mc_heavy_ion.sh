#!/bin/sh
#
# art-description: heavy ion reconstruction test from Andrzej Olszewski and Iwona Grabowska-Bold
# art-type: grid

export TRF_ECHO=True; Reco_tf.py --inputHITSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/HITS.04974747._000006.pool.root.1 --outputRDOFile=RDO.pool.root --outputESDFile=ESD.pool.root --outputAODFile=AOD.pool.root --autoConfiguration=everything --conditionsTag=OFLCOND-RUN12-SDR-14 --geometryVersion=ATLAS-R2-2015-01-01-00 --DataRunNumber=222222 --maxEvents=25 --preExec 'rec.Commissioning.set_Value_and_Lock(True)' --preInclude 'RAWtoESD:RecJobTransforms/HeavyIonRecConfig.py' --triggerConfig MCRECO:Physics_HI_v3

