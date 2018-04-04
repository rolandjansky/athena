#!/bin/sh
#
# art-description: Reco_tf with split trigger runs on mc16
# art-type: grid

Reco_tf.py --inputRDOFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc16_13TeV.410501.PowhegPythia8EvtGen_A14_ttbar_hdamp258p75_nonallhad.merge.RDO.e5458_s3126_d1437/RDO.11426804._000001.pool.root.1 --maxEvents 300 --autoConfiguration everything --conditionsTag=default:OFLCOND-MC16-SDR-16 --geometryVersion=default:ATLAS-R2-2016-01-00-01 --runNumber=410501 --steering RAWtoESD:in-RDO RAWtoESD:in-BS RAWtoESD:in+RDO_TRIG --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputTAGFile myTAG.pool.root --outputHISTFile myHIST.root

echo "art-result: $?"

