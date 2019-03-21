#!/bin/sh
#
# art-description: MC16-style simulation using ATLFASTIIF
# art-type: build
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena

# MC16 setup
# ATLAS-R2-2016-01-00-01 and OFLCOND-MC16-SDR-14

Sim_tf.py \
--conditionsTag 'default:OFLCOND-MC16-SDR-14' \
--physicsList 'FTFP_BERT_ATL' \
--truthStrategy 'MC15aPlus' \
--simulator 'ATLFASTIIF' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' 'EVNTtoHITS:G4AtlasTests/postInclude.DCubeTest.py' \
--preExec 'EVNTtoHITS:simFlags.TightMuonStepping=True' \
--DataRunNumber '284500' \
--geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
--inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
--outputHITSFile "Hits.pool.root" \
--maxEvents 2000 \
--imf False

echo  "art-result: $? simulation"

ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references
art.py compare grid  --entries 10 ${ArtPackage} ${ArtJobName}

echo  "art-result: $? regression"
