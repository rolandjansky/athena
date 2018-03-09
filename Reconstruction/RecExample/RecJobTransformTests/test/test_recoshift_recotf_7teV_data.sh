#!/bin/sh
#
# art-description: Jamie Boyd, Running on highmu Data at 7 TeV
# art-type: build


export TRF_ECHO=True; Reco_tf.py --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/high_mu-data11_7TeV.00179725.physics_JetTauEtmiss.merge.RAW._lb0021.data --autoConfiguration everything --conditionsTag="COMCOND-BLKPA-RUN1-07" --maxEvents 5 --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputTAGFile myTAG.pool.root --outputHISTFile myHist.root --preExec 'rec.doDetailedAuditor=True' 'rec.doNameAuditor=True' 'rec.doTrigger=False'

