#!/bin/sh
#
# art-description: Run G4AtlasAlg simulation, reading ttbar events, writing HITS, using MC15aPlus geometry and conditions
# art-include: master/Athena

# art-type: grid
# art-output: log.*
# art-output: HITS.pool.root
# art-output: CA.HITS.pool.root
# art-output: Config*.pkl

AtlasG4_tf.py \
--CA \
--conditionsTag 'default:OFLCOND-MC16-SDR-14' \
--postInclude 'PyJobTransforms.UseFrontier' \
--preInclude 'Campaigns.MC16Simulation' \
--geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
--inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
--outputHITSFile "test.CA.HITS.pool.root" \
--maxEvents 1 \
--postExec 'with open("ConfigSimCA.pkl", "wb") as f: cfg.store(f)' \
--imf False

rc=$?
mv log.AtlasG4Tf log.AtlasG4Tf_CA
echo  "art-result: $rc simCA"

rc2=-9999
if [ $rc -eq 0 ]
then
    AtlasG4_tf.py \
    --conditionsTag 'default:OFLCOND-MC16-SDR-14' \
    --postInclude 'default:PyJobTransforms/UseFrontier.py' \
    --preInclude 'AtlasG4Tf:Campaigns/MC16Simulation.py' \
    --geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
    --inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
    --outputHITSFile "test.CA.HITS.pool.root" \
    --maxEvents 1 \
    --imf False \
   --athenaopts '"--config-only=ConfigSimCG.pkl"'

     AtlasG4_tf.py \
    --conditionsTag 'default:OFLCOND-MC16-SDR-14' \
    --postInclude 'default:PyJobTransforms/UseFrontier.py' \
    --preInclude 'AtlasG4Tf:Campaigns/MC16Simulation.py' \
    --geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
    --inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
    --outputHITSFile "test.HITS.pool.root" \
    --maxEvents 1 \
    --imf False
    rc2=$?
    mv log.AtlasG4Tf log.AtlasG4Tf_OLD
fi
echo  "art-result: $rc2 simOLD"
rc4=-9999
if [ $rc2 -eq 0 ]
then
    acmd.py diff-root test.HITS.pool.root test.CA.HITS.pool.root --error-mode resilient --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings index_ref
    rc4=$?
fi
echo  "art-result: $rc4 FullG4MT_OLDvsCA"
