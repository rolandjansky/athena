#!/bin/sh
#
# art-description: Run FullG4MT simulation in AthenaMT with old and new style configuration, reading ttbar events, writing HITS, using MC16 geometry and conditions
# art-include: master/Athena
# art-type: grid
# art-output: log.*
# art-output: test.*.HITS.pool.root
# art-output: log.*
# art-output: Config*.pkl

export ATHENA_CORE_NUMBER=8

Sim_tf.py \
--CA \
--multithreaded \
--inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
--outputHITSFile "test.CA.HITS.pool.root" \
--maxEvents 10 \
--geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
--conditionsTag 'default:OFLCOND-MC16-SDR-14' \
--simulator 'FullG4MT' \
--postInclude 'PyJobTransforms.UseFrontier' \
--preInclude 'Campaigns.MC16Simulation' \
--postExec 'with open("ConfigSimCA.pkl", "wb") as f: cfg.store(f)' \
--imf False

rc=$?
mv log.EVNTtoHITS log.EVNTtoHITS.CA
echo  "art-result: $rc simCA"
status=$rc

rc2=-9999
if [ $rc -eq 0 ]
then
    Sim_tf.py \
    --multithreaded \
    --inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
  --outputHITSFile "test.CA.HITS.pool.root" \
  --maxEvents 10 \
  --geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
  --conditionsTag 'default:OFLCOND-MC16-SDR-14' \
  --simulator 'FullG4MT' \
  --postInclude 'default:PyJobTransforms/UseFrontier.py' \
  --preInclude 'EVNTtoHITS:Campaigns/MC16Simulation.py' \
  --imf False \
  --athenaopts '"--config-only=ConfigSimCG.pkl"'

    Sim_tf.py \
    --multithreaded \
    --inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
  --outputHITSFile "test.CG.HITS.pool.root" \
  --maxEvents 10 \
  --geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
  --conditionsTag 'default:OFLCOND-MC16-SDR-14' \
  --simulator 'FullG4MT' \
  --postInclude 'default:PyJobTransforms/UseFrontier.py' \
  --preInclude 'EVNTtoHITS:Campaigns/MC16Simulation.py' \
  --imf False

    rc2=$?
    status=$rc2
    mv log.EVNTtoHITS log.EVNTtoHITS.CG
fi
echo  "art-result: $rc2 simOLD"

rc3=-9999
if [ $rc2 -eq 0 ]
then
  # Compare the outputs
  acmd.py diff-root test.CG.HITS.pool.root test.CA.HITS.pool.root \
    --error-mode resilient \
    --mode semi-detailed \
    --order-trees \
    --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings index_ref
  rc3=$?
  status=$rc3
fi
echo "art-result: $rc3 OLDvsCA"

rc4=-9999
if [ $rc2 -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --order-trees  --mode=semi-detailed --file=test.CG.HITS.pool.root --diff-root
    rc4=$?
    status=$rc4
fi
echo  "art-result: $rc4 regression"

exit $status
