#!/bin/sh
#
# art-description: MC16-style simulation comparison of FullG4 and AtlasG4
# art-type: build
# art-include: 21.0/Athena
# art-include: 21.0/AthSimulation
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
--outputHITSFile "HITS.FullG4.pool.root" \
--maxEvents 2 \
--imf False

rc1=$?
echo  "art-result: $rc1 simulation FullG4"

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
--outputHITSFile "HITS.AtlasG4.pool.root" \
--maxEvents 2 \
--imf False

rc2=$?
echo  "art-result: $rc2 simulation AtlasG4"

rc3=-999
if [ $rc1 -eq 0 ]
then
  if [ $rc2 -eq 0 ]
  then
    # Compare the merged outputs
    acmd.py diff-root HITS.FullG4.pool.root HITS.AtlasG4.pool.root --error-mode resilient --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings TrackRecordCollection_p2_CaloEntryLayer TrackRecordCollection_p2_MuonEntryLayer TrackRecordCollection_p2_MuonExitLayer RecoTimingObj_p1_RAWtoESD_mems RecoTimingObj_p1_RAWtoESD_timings RAWtoESD_mems RAWtoESD_timings ESDtoAOD_mems ESDtoAOD_timings HITStoRDO_timings RAWtoALL_mems RAWtoALL_timings RecoTimingObj_p1_RAWtoALL_mems RecoTimingObj_p1_RAWtoALL_timings RecoTimingObj_p1_EVNTtoHITS_timings EVNTtoHITS_timings RecoTimingObj_p1_Bkg_HITStoRDO_timings index_ref
    rc3=$?
  fi
fi
echo "art-result: $rc3 comparison"

