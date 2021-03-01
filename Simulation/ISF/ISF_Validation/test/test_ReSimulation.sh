#!/bin/sh
#
# art-description: Reading single particle gen events, checking that the SkipEvents argument works, using 2015 geometry and conditions
# art-include: 21.0/Athena
# art-include: 21.0/AthSimulation
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-include: master/AthSimulation
# art-type: grid
# art-output: log.*
# art-output: original.HITS.pool.root
# art-output: renamed.HITS.pool.root
# art-output: retest.HITS.pool.root
# art-output: test.HITS.pool.root

INPUTHITSFILE="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1"
#INPUTHITSFILE='/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/pi_E50_eta0-60.evgen.pool.root'
MAXEVENTS=10

Sim_tf.py \
--conditionsTag 'default:OFLCOND-MC16-SDR-14' \
--physicsList 'FTFP_BERT_ATL' \
--truthStrategy 'MC15aPlus' \
--simulator 'FullG4' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py,SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py' \
--preExec 'EVNTtoHITS:simFlags.TightMuonStepping=True' \
--DataRunNumber '284500' \
--geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
--inputEVNTFile $INPUTHITSFILE \
--outputHITSFile "original.HITS.pool.root" \
--maxEvents $MAXEVENTS \
--imf False

echo "art-result: $? initial-sim"
cp log.EVNTtoHITS log.EVNTtoHITS.initial

ReSim_tf.py \
--conditionsTag 'ReSim:OFLCOND-MC16-SDR-14' \
--physicsList 'FTFP_BERT_ATL' \
--truthStrategy 'MC15aPlus' \
--simulator 'FullG4_LongLived' \
--postInclude 'ReSim:PyJobTransforms/UseFrontier.py,SimulationJobOptions/postInclude.ExcludeOldHITS.py' \
--postExec 'ReSim:ServiceMgr.ISF_LongLivedInputConverter.OutputLevel=VERBOSE;ServiceMgr.MessageSvc.enableSuppression=False' \
--preInclude 'ReSim:SimulationJobOptions/preInclude.BeamPipeKill.py,SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py,SimulationJobOptions/preInclude.ExtraParticles.py,SimulationJobOptions/preInclude.G4ExtraProcesses.py' \
--preExec 'ReSim:simFlags.TightMuonStepping=True' \
--DataRunNumber '284500' \
--geometryVersion 'ReSim:ATLAS-R2-2016-01-00-01' \
--inputHITSFile "original.HITS.pool.root" \
--outputHITS_RNMFile "renamed.HITS.pool.root" \
--outputHITS_RSMFile "retest.HITS.pool.root" \
--maxEvents $MAXEVENTS \
--imf False

echo "art-result: $? re-sim"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --diff-root --mode=semi-detailed --ignore-leave RecoTimingObj_p1_EVNTtoHITS_timingsOLD

echo  "art-result: $? regression"
