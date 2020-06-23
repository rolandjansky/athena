#!/bin/sh
#
# art-description: MC16-style simulation comparison of FullG4 and AtlasG4
# art-type: build
# art-include: 21.0/Athena
# art-include: 21.0/AthSimulation
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-include: master/AthSimulation

# MC16 setup
# ATLAS-R2-2016-01-00-01 and OFLCOND-MC16-SDR-14

export TRF_ECHO=1
Sim_tf.py \
--conditionsTag 'default:OFLCOND-MC16-SDR-14' \
--physicsList 'FTFP_BERT_ATL' \
--truthStrategy 'MC15aPlus' \
--simulator 'FullG4' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py' \
--preExec 'EVNTtoHITS:simFlags.TightMuonStepping=True' \
--DataRunNumber '284500' \
--geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
--inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
--outputHITSFile "Hits.FullG4.pool.root" \
--maxEvents 2 \
--imf False

echo  "art-result: $? simulation FullG4"


AtlasG4_tf.py \
--conditionsTag 'default:OFLCOND-MC16-SDR-14' \
--physicsList 'FTFP_BERT_ATL' \
--truthStrategy 'MC15aPlus' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'sim:SimulationJobOptions/preInclude.BeamPipeKill.py' \
--preExec 'sim:simFlags.TightMuonStepping=True' \
--postExec 'sim:topSeq.BeamEffectsAlg.ISFRun=True' \
--DataRunNumber '284500' \
--geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
--inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
--outputHITSFile "Hits.AtlasG4.pool.root" \
--maxEvents 2 \
--imf False

echo  "art-result: $? simulation AtlasG4"


# Compare the merged outputs
acmd.py diff-root Hits.FullG4.pool.root Hits.AtlasG4.pool.root --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings TrackRecordCollection_p2_CaloEntryLayer TrackRecordCollection_p2_MuonEntryLayer TrackRecordCollection_p2_MuonExitLayer

echo "art-result: $? comparison"

