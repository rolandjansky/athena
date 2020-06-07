#!/bin/sh
#
# art-description: Reco_tf with split trigger runs on mc16
# art-athena-mt: 4
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena


Reco_tf.py --inputRDOFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc16_13TeV.410501.PowhegPythia8EvtGen_A14_ttbar_hdamp258p75_nonallhad.merge.RDO.e5458_s3126_d1437/RDO.11426804._000001.pool.root.1 --maxEvents 300 --autoConfiguration everything --conditionsTag=default:OFLCOND-MC16-SDR-16 --geometryVersion=default:ATLAS-R2-2016-01-00-01 --runNumber=410501 --steering RAWtoESD:in-RDO RAWtoESD:in-BS RAWtoESD:in+RDO_TRIG --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputHISTFile myHIST.root

RES=$?
echo "art-result: $RES Reco"
return $RES

