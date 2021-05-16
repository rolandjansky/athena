#!/bin/sh
#
# art-description: Run simulation outside ISF, with alternative QGS_BIC physics list, reading in SU3 SUSY events, using 2015 geometry and conditions
# art-include: 21.0/Athena
# art-include: 21.0/AthSimulation
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-include: master/AthSimulation
# art-type: grid
# art-output: test.HITS.pool.root

AtlasG4_tf.py \
--CA \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/susy_SU3-herwigpp-7000.evgen.pool.root' \
--outputHITSFile 'test.NEW.HITS.pool.root' \
--maxEvents '5' \
--skipEvents '0' \
--randomSeed '42' \
--geometryVersion 'ATLAS-R2-2015-03-01-00' \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--DataRunNumber '222525' \
--physicsList 'QGSP_BIC' \
--postInclude 'PyJobTransforms.TransformUtils.UseFrontier' \
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
	--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/susy_SU3-herwigpp-7000.evgen.pool.root' \
	--outputHITSFile 'test.OLD.HITS.pool.root' \
	--maxEvents '5' \
	--skipEvents '0' \
	--randomSeed '42' \
	--geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' \
	--conditionsTag 'OFLCOND-RUN12-SDR-19' \
	--DataRunNumber '222525' \
	--physicsList 'QGSP_BIC' \
	--postInclude 'PyJobTransforms/UseFrontier.py' \
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
