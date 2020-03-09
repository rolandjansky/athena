#!/bin/sh
#
# art-description: Test the muon event generation for New Small Wheel.
#
# art-type: grid
# art-include: master/Athena
# art-include: 21.3/Athena

set -x

AtlasG4_tf.py   --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/MuonRecRTT/NSW/evntgen/*.root \
                --conditionsTag OFLCOND-RUN12-SDR-25 \
                --DataRunNumber 222500  \
                --geometryVersion ATLAS-R3-2021-01-00-00_VALIDATION \
                --outputHITSFile NSW_SingleMuon_EvntGen.hits.pool.root  \
                --maxEvents -1
                
echo "art-result: $?"
