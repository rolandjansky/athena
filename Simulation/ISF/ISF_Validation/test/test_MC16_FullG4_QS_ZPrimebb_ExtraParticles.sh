#!/bin/sh
#
# art-description: MC16-style simulation using FullG4_LongLived and the ExtraParticles package
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-type: grid
# art-output: test.HITS.pool.root
# art-output: truth.root

# MC16 setup
# ATLAS-R2-2016-01-00-01 and OFLCOND-MC16-SDR-14
Sim_tf.py \
--conditionsTag 'default:OFLCOND-MC16-SDR-14' \
--physicsList 'FTFP_BERT_ATL' \
--truthStrategy 'MC15aPlus' \
--simulator 'FullG4_LongLived' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py,SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py,SimulationJobOptions/preInclude.ExtraParticles.py' \
--preExec 'EVNTtoHITS:simFlags.TightMuonStepping=True;simFlags.OptionalUserActionList.addAction("G4UA::VerboseSelectorTool");simFlags.UserActionConfig.addConfig("G4UA::VerboseSelectorTool","TargetEvent",-1);simFlags.UserActionConfig.addConfig("G4UA::VerboseSelectorTool","TargetPdgIDs", [413, 511, 513, 521]);simFlags.UserActionConfig.addConfig("G4UA::VerboseSelectorTool","TargetTrack",999999999);simFlags.UserActionConfig.addConfig("G4UA::VerboseSelectorTool","VerboseLevel",2);' \
--DataRunNumber '284500' \
--geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
--inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/user.sgargiul.ZPrimebb.FlatPt.9999_EVNT.pool.root" \
--outputHITSFile "test.HITS.pool.root" \
--maxEvents 5 \
--imf False

rc=$?
rc2=-9999
echo  "art-result: $rc simulation"
if [ $rc -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 5 ${ArtPackage} ${ArtJobName} --mode=semi-detailed
    rc2=$?
fi

echo  "art-result: $rc2 regression"
