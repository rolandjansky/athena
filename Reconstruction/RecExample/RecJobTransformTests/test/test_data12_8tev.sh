#!/bin/sh
#
# art-description: Reco_tf runs on 8TeV collision data
# art-type: grid

Reco_tf.py --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data12_8TeV.00209109.physics_JetTauEtmiss.merge.RAW._lb0186._SFO-1._0001.1 --maxEvents 300 --autoConfiguration everything --conditionsTag="COMCOND-BLKPA-RUN1-07" --preExec 'rec.doTrigger=False;' --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputTAGFile myTAG.pool.root --outputHISTFile myHist.root

