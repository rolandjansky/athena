#!/bin/sh
#
# art-description: heavy ion reconstruction test from Andrzej Olszewski and Iwona Grabowska-Bold
# art-athena-mt: 4
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena


export TRF_ECHO=True; Reco_tf.py --inputHITSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc15_5TeV.420000.Hijing_PbPb_5p02TeV_MinBias_Flow_JJFV6.merge.HITS.e4962_s3004_s2921/HITS.09732013._000496.pool.root.1 --outputESDFile=ESD.pool.root --outputAODFile=AOD.pool.root --maxEvents=25 --conditionsTag 'all:OFLCOND-MC15c-SDR-11' --postInclude 'all:RecJobTransforms/UseFrontier.py,SimulationJobOptions/postInclude.HijingPars.py' --preExec 'all:rec.doHeavyIon.set_Value_and_Lock(True)' --autoConfiguration 'everything' --triggerConfig 'MCRECO:MC_HI_v3_tight_mc_prescale' --DataRunNumber '226000' --geometryVersion 'all:ATLAS-R2-2015-03-01-00'

RES=$?
echo "art-result: $RES Reco"
return $RES

