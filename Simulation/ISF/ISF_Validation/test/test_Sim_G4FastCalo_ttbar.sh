#!/bin/sh
#
# art-description: Run simulation  using ISF with the G4FastCalo simulator, reading ttbar events, writing HITS, using 2015 geometry and conditions
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-type: grid
# art-output: test.HITS.pool.root
# art-output: truth.root

Sim_tf.py \
--conditionsTag 'default:OFLCOND-RUN12-SDR-19' \
--physicsList 'FTFP_BERT' \
--truthStrategy 'MC15aPlus' \
--simulator 'G4FastCalo' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' 'EVNTtoHITS:G4AtlasTests/postInclude.DCubeTest.py' \
--preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py' \
--DataRunNumber '222525' \
--geometryVersion 'default:ATLAS-R2-2015-03-01-00' \
--inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/mc12_valid.110401.PowhegPythia_P2012_ttbar_nonallhad.evgen.EVNT.e3099.01517252._000001.pool.root.1" \
--outputHITSFile "test.HITS.pool.root" \
--maxEvents 250 \
--imf False \
--preExec 'from ISF_FastCaloSimServices.ISF_FastCaloSimJobProperties import ISF_FastCaloSimFlags;ISF_FastCaloSimFlags.ParamsInputFilename="/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/FastCaloSim/MC16/TFCSparam_v012.root"'

echo  "art-result: $? simulation"

ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=semi-detailed

echo  "art-result: $? regression"
