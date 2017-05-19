#!/bin/sh
set -e

Digi_tf.py --inputHITSFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/HITS.04919495._0004*.pool.root*' --digiSeedOffset1 '1' --digiSeedOffset2 '2' --geometryVersion 'ATLAS-R2-2015-03-01-00' --conditionsTag 'OFLCOND-RUN12-SDR-28' --DataRunNumber '222525' --maxEvents '50' --outputRDOFile 'MCtest_ttbar.digit.pool.root'

Reco_tf.py --outputTAGFile=myTAG_ttbar_2.root --outputHISTFile=myMergedMonitoring_ttbar_2.root --maxEvents=500 --outputESDFile=myESD_ttbar_2.pool.root --outputAODFile=myAOD_ttbar_2.AOD.pool.root --inputRDOFile=MCtest_ttbar.digit.pool.root --autoConfiguration=everything --triggerConfig=NONE --preExec 'rec.doTrigger=False;'
