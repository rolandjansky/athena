#!/bin/sh
#
# art-description: Run simulation using ISF with the ATLFASTIIF simulator, reading ttbar events, writing HITS, using 2015 geometry and conditions
# art-type: grid

Sim_tf.py --conditionsTag 'default:OFLCOND-RUN12-SDR-19' --physicsList 'FTFP_BERT' --truthStrategy 'MC12' --simulator 'ATLFASTIIF' --postInclude 'default:PyJobTransforms/UseFrontier.py' 'G4AtlasTests/postInclude.DCubeTest.py' --DataRunNumber '222525' --geometryVersion 'default:ATLAS-R2-2015-03-01-00_VALIDATION' --inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/mc12_valid.110401.PowhegPythia_P2012_ttbar_nonallhad.evgen.EVNT.e3099.01517252._000001.pool.root.1" --outputHITSFile "test.HITS.pool.root" --maxEvents 2000

ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}

#TODO Add Digi and Reco steps
