#!/bin/sh
#
# art-description: Reco_tf runs on 13TeV collision data with floating point exception enabled 2017
# art-type: grid

Reco_tf.py --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data17_13TeV.00324910.physics_Main.daq.RAW._lb0713._SFO-6._0001.data --maxEvents -1 --AMI=f908 --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputTAGFile myTAG.pool.root --outputHISTFile myHist.root --preExec 'rec.doFloatingPointException=True;rec.doTrigger=False;' --postExec 'FPEAuditor.NStacktracesOnFPE=5'

echo "art-result: $?"

