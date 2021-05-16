#!/bin/sh
#
# art-description: Run simulation outside ISF, reading di-jet events, using FTFP_BERT_ATL physics list, writing HITS, using 2015 geometry and conditions
# art-include: master/Athena
# art-include: master/AthSimulation
# art-type: grid
# art-output: test.OLD.HITS.pool.root
# art-output: test.NEW.HITS.pool.root
# art-output: log.G4AtlasAlg*

AtlasG4_tf.py \
--CA \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--physicsList 'FTFP_BERT_ATL' \
--postInclude 'PyJobTransforms.TransformUtils.UseFrontier' \
--DataRunNumber '222525' \
--geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/J2_jetjet-pythia6-7000.evgen.pool.root' \
--outputHITSFile "test.NEW.HITS.pool.root" \
--maxEvents '10' \
--skipEvents '0' \
--randomSeed '10' \
--imf False

rc=$?
mv log.AtlasG4Tf log.G4AtlasAlg_AthenaCA
echo  "art-result: $rc G4AtlasAlg_AthenaCA"
rc2=-9999
if [ $rc -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    AtlasG4_tf.py \
    --conditionsTag 'OFLCOND-RUN12-SDR-19' \
    --physicsList 'FTFP_BERT_ATL' \
    --postInclude 'default:PyJobTransforms/UseFrontier.py' \
    --DataRunNumber '222525' \
    --geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
    --inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/J2_jetjet-pythia6-7000.evgen.pool.root' \
    --outputHITSFile "test.OLD.HITS.pool.root" \
    --maxEvents '10' \
    --skipEvents '0' \
    --randomSeed '10' \
    --truthStrategy 'MC15aPlus' \
    --imf False
    mv log.AtlasG4Tf log.G4AtlasAlg_AthenaCA_OLD
    rc2=$?

fi
echo  "art-result: $rc2 G4AtlasAlg_AthenaCA_OLD"
rc4=-9999
if [ $rc2 -eq 0 ]
then
    acmd.py diff-root test.OLD.HITS.pool.root test.NEW.HITS.pool.root --error-mode resilient --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_AtlasG4Tf_timings index_ref
    rc4=$?
fi
echo  "art-result: $rc4 FullG4MT_OLDvsCA"
