#!/bin/sh
#
# art-description: ReSimulation Workflow running with MC16 conditions/geometry
# art-include: master/Athena
# art-type: grid
# art-output: log.*
# art-output: original.HITS.pool.root
# art-output: resim.HITS.pool.root

INPUTEVNTFILE="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1"
#INPUTEVNTFILE='/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/pi_E50_eta0-60.evgen.pool.root'
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
--inputEVNTFile $INPUTEVNTFILE \
--outputHITSFile "original.HITS.pool.root" \
--maxEvents $MAXEVENTS \
--imf False

echo "art-result: $? initial-sim"
cp log.EVNTtoHITS log.EVNTtoHITS.initial

ReSim_tf.py \
--conditionsTag 'ReSim:OFLCOND-MC16-SDR-14' \
--physicsList 'FTFP_BERT_ATL' \
--truthStrategy 'MC15aPlus' \
--simulator 'FullG4_QS' \
--postInclude 'ReSim:PyJobTransforms/UseFrontier.py' \
--preInclude 'ReSim:SimulationJobOptions/preInclude.BeamPipeKill.py,SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py,SimulationJobOptions/preInclude.ExtraParticles.py,SimulationJobOptions/preInclude.G4ExtraProcesses.py' \
--preExec 'ReSim:simFlags.TightMuonStepping=True' \
--DataRunNumber '284500' \
--geometryVersion 'ReSim:ATLAS-R2-2016-01-00-01' \
--inputHITSFile "original.HITS.pool.root" \
--outputHITS_RSMFile "resim.HITS.pool.root" \
--maxEvents $MAXEVENTS \
--imf False

echo "art-result: $? re-sim"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --diff-root --mode=semi-detailed --ignore-leave RecoTimingObj_p1_EVNTtoHITS_timingsOLD

echo  "art-result: $? regression"
