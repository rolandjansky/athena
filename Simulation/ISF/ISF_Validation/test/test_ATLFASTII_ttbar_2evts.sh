#!/bin/sh
#
# art-description: MC15-style simulation using ATLFASTII
# art-type: build
# art-include: 21.0/Athena
# art-include: 21.0/AthSimulation
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena

# MC15 setup
# ATLAS-R2-2015-03-01-00 and OFLCOND-RUN12-SDR-19
export TRF_ECHO=1
Sim_tf.py \
--conditionsTag 'default:OFLCOND-RUN12-SDR-19' \
--physicsList 'FTFP_BERT' \
--truthStrategy 'MC12' \
--simulator 'ATLFASTII' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py' \
--DataRunNumber '222525' \
--geometryVersion 'default:ATLAS-R2-2015-03-01-00_VALIDATION' \
--inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/mc12_valid.110401.PowhegPythia_P2012_ttbar_nonallhad.evgen.EVNT.e3099.01517252._000001.pool.root.1" \
--outputHITSFile "Hits.pool.root" \
--maxEvents 2

echo  "art-result: $? simulation"
