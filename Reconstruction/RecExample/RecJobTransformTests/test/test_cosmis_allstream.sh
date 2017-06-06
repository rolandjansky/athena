#!/bin/sh
#
# art-description: Reco_tf runs on cosmics with all streams
# art-type: grid

Reco_tf.py --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data11_cos.00193024.physics_CosmicCalo.merge.RAW._lb0096._SFO-ALL._0001.1 --maxEvents 300 --autoConfiguration everything --conditionsTag="COMCOND-BLKPA-RUN1-07" --preExec="rec.doTrigger=False;" --outputESDFile myESD.pool.root --outputNTUP_MUONCALIBFile muonCalib.root --outputHISTFile myMergedMonitoring.root --outputTAGFile myTAG.pool.root

