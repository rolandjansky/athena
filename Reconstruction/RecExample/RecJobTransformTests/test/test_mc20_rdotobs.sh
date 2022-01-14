#!/bin/sh
#
# art-description: RDO to BS on tt mc20
# art-athena-mt: 4
# art-type: grid
# art-include: master/Athena

Reco_tf.py --athenaopts="--threads=8" --inputRDOFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc20_13TeV/mc20_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.digit.RDO.e6337_e5984_s3126_d1708/RDO.26820830._000021.pool.root.1 --outputBSFile mc20_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.data --autoConfiguration everything --maxEvents 300 --conditionsTag=default:OFLCOND-MC16-SDR-16 --geometryVersion=default:ATLAS-R2-2016-01-00-01 --runNumber=410470

RES=$?
echo "art-result: $RES Reco"

