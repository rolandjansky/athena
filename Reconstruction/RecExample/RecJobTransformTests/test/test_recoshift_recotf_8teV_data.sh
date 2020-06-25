#!/bin/sh
#
# art-description: Jamie Boyd, data12_8TeV reconstruction
# art-athena-mt: 4
# art-type: build
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena


export TRF_ECHO=True; Reco_tf.py --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data12_8TeV.00209109.physics_JetTauEtmiss.merge.RAW._lb0186._SFO-1._0001.1 --autoConfiguration everything --conditionsTag="COMCOND-BLKPA-RUN1-07" --maxEvents 5 --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputHISTFile myHist.root --preExec 'rec.doDetailedAuditor=True' 'rec.doNameAuditor=True' 'rec.doTrigger=False'

RES=$?

xAODDigest.py myAOD.pool.root digest.txt

echo "art-result: $RES Reco"




