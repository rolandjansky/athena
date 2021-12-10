#!/bin/sh
#
# art-description: Run simulation outside ISF, reading lead ions peripheral simulation (low multiplicity) events, writing HITS, using 2015 geometry and conditions
# art-include: master/Athena
# art-type: grid
# art-output: *.HITS.pool.root
# art-output: log.*
# art-output: Config*.pkl

AtlasG4_tf.py \
--CA \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/pbpb_Peripheral-hijing-5500.evgen.pool.root' \
--outputHITSFile 'test.CA.HITS.pool.root' \
--maxEvents '1' \
--skipEvents '4' \
--randomSeed '10' \
--geometryVersion 'ATLAS-R1-2012-03-00-00' \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--DataRunNumber '210184' \
--physicsList 'FTFP_BERT' \
--postInclude 'PyJobTransforms.TransformUtils.UseFrontier' \
--truthStrategy 'MC12' \
--postExec 'with open("ConfigSimCA.pkl", "wb") as f: cfg.store(f)' \
--imf False

#todo add postInclude SimuJobTransforms.HijingPars
rc=$?
mv log.AtlasG4Tf log.AtlasG4Tf_CA
echo  "art-result: $rc simCA"

AtlasG4_tf.py \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/pbpb_Peripheral-hijing-5500.evgen.pool.root' \
--outputHITSFile 'test.CA.HITS.pool.root' \
--maxEvents '1' \
--skipEvents '4' \
--randomSeed '10' \
--geometryVersion 'ATLAS-R1-2012-03-00-00_VALIDATION' \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--DataRunNumber '210184' \
--physicsList 'FTFP_BERT' \
--postInclude 'PyJobTransforms/UseFrontier.py' 'AtlasG4Tf:SimulationJobOptions/postInclude.HijingPars.py' \
--truthStrategy 'MC12' \
--imf False \
--athenaopts '"--config-only=ConfigSimCG.pkl"'

AtlasG4_tf.py \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/pbpb_Peripheral-hijing-5500.evgen.pool.root' \
--outputHITSFile 'test.HITS.pool.root' \
--maxEvents '1' \
--skipEvents '4' \
--randomSeed '10' \
--geometryVersion 'ATLAS-R1-2012-03-00-00_VALIDATION' \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--DataRunNumber '210184' \
--physicsList 'FTFP_BERT' \
--postInclude 'PyJobTransforms/UseFrontier.py' 'AtlasG4Tf:SimulationJobOptions/postInclude.HijingPars.py' \
--truthStrategy 'MC12' \
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
    art.py compare grid --entries 1 ${ArtPackage} ${ArtJobName} --mode=semi-detailed --file=test.HITS.pool.root
    rc4=$?
fi
echo  "art-result: $rc4 regression"
