#!/bin/sh
#
# art-description: Run simulation outside ISF, using 2012 geometry and conditions, reading single muon events, writing HITS
# art-include: master/Athena
# art-include: master/AthSimulation
# art-type: grid
# art-output: *.HITS.pool.root
# art-output: log.*
# art-output: Config*.pkl

AtlasG4_tf.py \
--CA \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--physicsList 'FTFP_BERT' \
--postInclude 'PyJobTransforms.TransformUtils.UseFrontier' \
--DataRunNumber '212272' \
--geometryVersion 'ATLAS-R1-2012-03-00-00' \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/mu_E200_eta0-25.evgen.pool.root' \
--outputHITSFile 'test.CA.HITS.pool.root' \
--maxEvents '10' \
--skipEvents '0' \
--randomSeed '10' \
--truthStrategy 'MC15aPlus' \
--postExec 'with open("ConfigSimCA.pkl", "wb") as f: cfg.store(f)' \
--imf False

rc=$?
mv log.AtlasG4Tf log.AtlasG4Tf_CA
echo  "art-result: $rc simCA"

AtlasG4_tf.py \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--physicsList 'FTFP_BERT' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--DataRunNumber '212272' \
--geometryVersion 'ATLAS-R1-2012-03-00-00' \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/mu_E200_eta0-25.evgen.pool.root' \
--outputHITSFile 'test.CA.HITS.pool.root' \
--maxEvents '10' \
--skipEvents '0' \
--randomSeed '10' \
--truthStrategy 'MC15aPlus' \
--imf False \
--athenaopts '"--config-only=ConfigSimCG.pkl"'

AtlasG4_tf.py \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--physicsList 'FTFP_BERT' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--DataRunNumber '212272' \
--geometryVersion 'ATLAS-R1-2012-03-00-00' \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/mu_E200_eta0-25.evgen.pool.root' \
--outputHITSFile 'test.HITS.pool.root' \
--maxEvents '10' \
--skipEvents '0' \
--randomSeed '10' \
--truthStrategy 'MC15aPlus' \
--imf False

rc2=$?
mv log.AtlasG4Tf log.AtlasG4Tf_OLD
echo  "art-result: $rc2 simOLD"

rc3=-9999
if [ $rc -eq 0 ] && [ $rc2 -eq 0 ]
then
    acmd.py diff-root test.HITS.pool.root test.CA.HITS.pool.root --error-mode resilient --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings index_ref
    rc3=$?
fi
echo  "art-result: $rc3 OLDvsCA"

rc4=-9999
if [ $rc2 -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=semi-detailed --file=test.HITS.pool.root
    rc4=$?
fi
echo  "art-result: $rc4 regression"
