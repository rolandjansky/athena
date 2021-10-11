#!/bin/sh
#
# art-description: MC16-style simulation using FullG4
# art-include: master/Athena
# art-type: grid
# art-output: test.CG.HITS.pool.root
# art-output: test.CA.HITS.pool.root

# RUN3 setup
# ATLAS-R3-2021-01-00-02 and OFLCOND-MC16-SDR-RUN3-01
Sim_tf.py \
--conditionsTag 'default:OFLCOND-MC16-SDR-RUN3-01' \
--physicsList 'FTFP_BERT_ATL' \
--truthStrategy 'MC15aPlus' \
--simulator 'FullG4MT' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py,SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py' \
--preExec 'EVNTtoHITS:simFlags.TightMuonStepping=True' \
--DataRunNumber '330000' \
--geometryVersion 'default:ATLAS-R3-2021-01-00-02_VALIDATION' \
--inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
--outputHITSFile "test.CG.HITS.pool.root" \
--maxEvents 4 \
--imf False

rc1=$?
status=$rc1
echo "art-result: $rc1 simulation CG"

rc2=-999
if [ $rc1 -eq 0 ]
then
  Sim_tf.py \
  --CA \
  --conditionsTag 'default:OFLCOND-MC16-SDR-RUN3-01' \
  --physicsList 'FTFP_BERT_ATL' \
  --truthStrategy 'MC15aPlus' \
  --simulator 'FullG4MT' \
  --postInclude 'PyJobTransforms.UseFrontier' \
  --preInclude 'EVNTtoHITS:SimuJobTransforms.BeamPipeKill,SimuJobTransforms.FrozenShowersFCalOnly,SimuJobTransforms.TightMuonStepping' \
  --DataRunNumber '330000' \
  --geometryVersion 'default:ATLAS-R3-2021-01-00-02' \
  --inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
  --outputHITSFile "test.CA.HITS.pool.root" \
  --maxEvents 4 \
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
