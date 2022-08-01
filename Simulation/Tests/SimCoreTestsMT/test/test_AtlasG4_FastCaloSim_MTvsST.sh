#!/bin/sh
#
# art-description: Run MT and ST simulation outside ISF, reading ttbar events, writing HITS, using MC16 geometry and conditions
# art-include: 22.0/Athena
# art-include: master/Athena

# art-type: grid
# art-architecture:  '#x86_64-intel'
# art-athena-mt: 8
# art-output: log.*
# art-output: test.MT.HITS.pool.root
# art-output: test.ST.HITS.pool.root

export ATHENA_CORE_NUMBER=8

AtlasG4_tf.py \
--multithreaded \
--randomSeed '10' \
--maxEvents '20' \
--skipEvents '0' \
--preInclude 'SimulationJobOptions/preInclude.FastCaloSim.py,Campaigns/MC21Simulation.py' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--postExec 'sim:topSeq.BeamEffectsAlg.ISFRun=True' \
--inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
--outputHITSFile="test.MT.HITS.pool.root" \
--physicsList="FTFP_BERT_ATL" 		\
--conditionsTag "default:OFLCOND-MC21-SDR-RUN3-07" 		\
--geometryVersion="default:ATLAS-R3S-2021-03-00-00" 	\
--preExec 'AtlasG4Tf:simFlags.ReleaseGeoModel=False;' \
--imf False;

rc=$?
echo  "art-result: $rc MTsim"
rc2=-9999
if [ $rc -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    unset ATHENA_CORE_NUMBER
    AtlasG4_tf.py \
    --randomSeed '10' \
    --maxEvents '20' \
    --skipEvents '0' \
    --preInclude 'SimulationJobOptions/preInclude.FastCaloSim.py,Campaigns/MC21Simulation.py' \
    --postInclude 'default:PyJobTransforms/UseFrontier.py' \
    --postExec 'sim:topSeq.BeamEffectsAlg.ISFRun=True' \
    --inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
    --outputHITSFile="test.ST.HITS.pool.root" \
    --physicsList="FTFP_BERT_ATL" 		\
    --conditionsTag "default:OFLCOND-MC21-SDR-RUN3-07" 		\
    --geometryVersion="default:ATLAS-R3S-2021-03-00-00" 	\
    --preExec 'AtlasG4Tf:simFlags.ReleaseGeoModel=False;' \
    --imf False;
    rc2=$?
fi
echo  "art-result: $rc2 STsim"
rc3=-9999
if [ $rc2 -eq 0 ]
then
    acmd.py diff-root test.MT.HITS.pool.root test.ST.HITS.pool.root --error-mode resilient --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings index_ref
    rc3=$?
fi
echo  "art-result: $rc3 comparision"



