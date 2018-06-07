#!/bin/sh
#
# art-description: Reco_tf runs on splash events with all streams
# art-type: grid

export TRF_ECHO=True; Reco_tf.py --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data15_comm.00265545.physics_MinBias.merge.RAW._lb0048._SFO-ALL._0001.1 --autoConfiguration everything --conditionsTag="CONDBR2-BLKPA-2015-04" --geometryVersion="ATLAS-R2-2015-03-01-00" --outputESDFile myESD.pool.root --outputNTUP_MUONCALIBFile muonCalib.root --outputHISTFile myMergedMonitoring.root --outputTAGFile myTAG.pool.root --maxEvents 300 --preExec 'rec.doTrigger=False;'

