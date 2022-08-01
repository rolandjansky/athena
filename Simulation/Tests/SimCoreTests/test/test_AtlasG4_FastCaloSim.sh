#!/bin/sh
#
# art-description: Tests implementation of FastCaloSimV2 as a Geant4 fast simulation engine with Run 3 geometry and conditions
# art-include: 22.0/Athena
# art-include: master/Athena
# art-type: grid
# art-architecture:  '#x86_64-intel'
# art-output: test.HITS.pool.root

AtlasG4_tf.py \
    --randomSeed '10' \
    --maxEvents '20' \
    --skipEvents '0' \
    --preInclude 'SimulationJobOptions/preInclude.FastCaloSim.py,Campaigns/MC21Simulation.py' \
    --postInclude 'default:PyJobTransforms/UseFrontier.py' \
    --postExec 'sim:topSeq.BeamEffectsAlg.ISFRun=True' \
    --inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
    --outputHITSFile="test.HITS.pool.root" \
    --physicsList="FTFP_BERT_ATL" 		\
    --conditionsTag "default:OFLCOND-MC21-SDR-RUN3-07" 		\
    --geometryVersion="default:ATLAS-R3S-2021-03-00-00" 	\
    --imf False;

rc=$?
rc2=-9999
echo  "art-result: $rc simulation"
if [ $rc -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 20 ${ArtPackage} ${ArtJobName} --mode=semi-detailed
    rc2=$?
fi

echo  "art-result: $rc2 regression"