#!/bin/sh
#
# art-description: Reco_tf runs on 8TeV collision data with all streams 2012
# art-athena-mt: 4
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena


Reco_tf.py --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data12_8TeV.00209109.physics_JetTauEtmiss.merge.RAW._lb0186._SFO-1._0001.1 --maxEvents 300 --autoConfiguration everything --conditionsTag="COMCOND-BLKPA-RUN1-07" --preExec 'rec.doTrigger=False;' --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputHISTFile myHist.root 

RES=$?
echo "art-result: $RES Reco"
return $RES
