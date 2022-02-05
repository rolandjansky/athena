#!/bin/sh
#
# art-description: MC16-style simulation using FullG4MT
# art-include: master/Athena
# art-type: grid
# art-output: test.CG.HITS.pool.root
# art-output: test.CA.HITS.pool.root
# art-output: Config*

# RUN3 setup
# ATLAS-R3-2021-01-00-02 and OFLCOND-MC16-SDR-RUN2-09
Sim_tf.py \
--conditionsTag 'default:OFLCOND-MC16-SDR-RUN2-09' \
--simulator 'FullG4MT' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'EVNTtoHITS:Campaigns/MC16Simulation.py' \
--geometryVersion 'default:ATLAS-R2-2016-01-02-01_VALIDATION' \
--inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
--outputHITSFile "test.CG.HITS.pool.root" \
--maxEvents 3 \
--imf False \
--athenaopts '"--config-only=ConfigCG.pkl"'

Sim_tf.py \
--conditionsTag 'default:OFLCOND-MC16-SDR-RUN2-09' \
--simulator 'FullG4MT' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'EVNTtoHITS:Campaigns/MC16Simulation.py' \
--geometryVersion 'default:ATLAS-R2-2016-01-02-01_VALIDATION' \
--inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
--outputHITSFile "test.CG.HITS.pool.root" \
--maxEvents 3 \
--imf False

rc1=$?
status=$rc1
echo "art-result: $rc1 simulation CG"

rc2=-999
if [ $rc1 -eq 0 ]
then
  Sim_tf.py \
  --CA \
  --conditionsTag 'default:OFLCOND-MC16-SDR-RUN2-09' \
  --simulator 'FullG4MT' \
  --postExec 'with open("ConfigCA.pkl", "wb") as f: cfg.store(f)' \
  --postInclude 'default:PyJobTransforms.UseFrontier' \
  --preInclude 'EVNTtoHITS:Campaigns.MC16Simulation' \
  --geometryVersion 'default:ATLAS-R2-2016-01-02-01' \
  --inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
  --outputHITSFile "test.CA.HITS.pool.root" \
  --maxEvents 3 \
  --imf False

  rc2=$?
  status=$rc2
fi

echo  "art-result: $rc2 simulation CA"

rc3=-999
if [ $rc2 -eq 0 ]
then
  # Compare the outputs
  acmd.py diff-root test.CG.HITS.pool.root test.CA.HITS.pool.root \
    --error-mode resilient \
    --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings index_ref
  rc3=$?
  status=$rc3
fi

echo "art-result: $rc3 comparison"

exit $status
