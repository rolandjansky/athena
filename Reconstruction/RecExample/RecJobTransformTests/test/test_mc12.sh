#!/bin/sh
#
#art-description: Reco_tf runs on mc12
#art-queue: long
#
Reco_tf.py --inputRDOFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/RDO.01177879._000001.pool.root.1 --maxEvents 300 --autoConfiguration everything --conditionsTag='OFLCOND-RUN12-SDR-31' --geometryVersion='ATLAS-R2-2015-03-01-00' --preExec 'rec.doTrigger=False;' --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputTAGFile myTAG.pool.root --outputHISTFile myHIST.root

