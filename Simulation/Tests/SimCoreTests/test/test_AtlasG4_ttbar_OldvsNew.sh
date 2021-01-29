#!/bin/sh
#
# art-description: Run G4AtlasAlg simulation, reading ttbar events, writing HITS, using MC15aPlus geometry and conditions
# art-include: master/Athena

# art-type: grid
# art-output: log.*
# art-output: test.OLD.HITS.pool.root
# art-output: test.NEW.HITS.pool.root

AtlasG4_tf.py \
--CA \
--conditionsTag 'default:OFLCOND-MC16-SDR-14' \
--physicsList 'FTFP_BERT_ATL' \
--truthStrategy 'MC15aPlus' \
--postInclude 'PyJobTransforms.UseFrontier' \
--preInclude 'SimuJobTransforms.BeamPipeKill,SimuJobTransforms.FrozenShowersFCalOnly,SimuJobTransforms.TightMuonStepping' \
--DataRunNumber '284500' \
--geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
--inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
--outputHITSFile "test.NEW.HITS.pool.root" \
--maxEvents 1 \
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
    --conditionsTag 'default:OFLCOND-MC16-SDR-14' \
    --physicsList 'FTFP_BERT_ATL' \
    --truthStrategy 'MC15aPlus' \
    --postInclude 'default:PyJobTransforms/UseFrontier.py' \
    --preInclude 'AtlasG4Tf:SimulationJobOptions/preInclude.BeamPipeKill.py,SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py' \
    --preExec 'AtlasG4Tf:simFlags.TightMuonStepping=True' \
    --DataRunNumber '284500' \
    --geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
    --inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
    --outputHITSFile "test.OLD.HITS.pool.root" \
    --maxEvents 1 \
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
