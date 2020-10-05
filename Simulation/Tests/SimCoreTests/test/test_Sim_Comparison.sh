#!/bin/sh
#
# art-description: Simulation example to test agreement between ConfGetter and ComponentAccumulator configurations.
# art-include: master/Athena
# art-type: grid
# art-output: test.HITS.pool.root
# art-output: myHITSnew.pool.root

# ComponentAccumulator
G4AtlasAlgConfigNew_Test.py
rc=$?
echo "art-result: $rc CAsim"

# ConfGetter
rc2=-9999
if [ $rc -eq 0 ]
then
    AtlasG4_tf.py \
    --conditionsTag 'default:OFLCOND-MC16-SDR-14' \
    --physicsList 'FTFP_BERT_ATL' \
    --truthStrategy 'MC15aPlus' \
    --postInclude 'default:PyJobTransforms/UseFrontier.py' \
    --preInclude 'AtlasG4Tf:SimulationJobOptions/preInclude.BeamPipeKill.py,SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py' \
    --preExec 'AtlasG4Tf:simFlags.TightMuonStepping=True' \
    --DataRunNumber '284500' \
    --geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
    --inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
    --outputHITSFile "test.HITS.pool.root" \
    --maxEvents 4 \
    --imf False \
    --postExec 'topSeq+=CfgMgr.JobOptsDumperAlg(FileName="G4AtlasTestConfigOld.txt")'
    rc2=$?
fi

rc3=-9999
echo "art-result: $rc simulation"
if [ $rc2 -eq 0 ]
then
    acmd.py diff-root test.HITS.pool.root myHITSnew.pool.root --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_HITStoRDO_timings index_ref --error-mode resilient
    rc3=$?
fi

echo "art-result: $rc2 regression"
