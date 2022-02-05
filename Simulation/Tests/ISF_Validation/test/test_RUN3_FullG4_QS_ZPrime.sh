#!/bin/sh
#
# art-description: MC21-style simulation using FullG4_QS (13 TeV Zprime input - needs updating)
# art-include: master/Athena
# art-include: master/AthSimulation
# art-type: grid
# art-output: test.HITS.pool.root
#/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/EVNT.04607198._000001.pool.root.1
Sim_tf.py \
--CA True \
--conditionsTag 'default:OFLCOND-MC21-SDR-RUN3-03' \
--simulator 'FullG4MT_QS' \
--postInclude 'PyJobTransforms.TransformUtils.UseFrontier' \
--preInclude 'EVNTtoHITS:Campaigns.MC21Simulation,SimuJobTransforms.SimulationHelpers.enableFrozenShowersFCalOnly' \
--DataRunNumber '330000' \
--geometryVersion 'default:ATLAS-R3S-2021-02-00-00' \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/EVNT.04607198._000001.pool.root.1' \
--outputHITSFile 'test.CA.HITS.pool.root' \
--maxEvents '10' \
--postExec 'with open("ConfigSimCA.pkl", "wb") as f: cfg.store(f)' \
--imf False

rc=$?
mv log.EVNTtoHITS log.EVNTtoHITS_CA
echo  "art-result: $rc simCA"

Sim_tf.py \
--conditionsTag 'default:OFLCOND-MC21-SDR-RUN3-03' \
--simulator 'FullG4MT_QS' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'EVNTtoHITS:Campaigns/MC21Simulation.py,SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py,SimulationJobOptions/preInclude.ExtraParticles.py,SimulationJobOptions/preInclude.G4ExtraProcesses.py' \
--DataRunNumber '330000' \
--geometryVersion 'default:ATLAS-R3S-2021-02-00-00' \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/EVNT.04607198._000001.pool.root.1' \
--outputHITSFile 'test.CA.HITS.pool.root' \
--maxEvents '10' \
--imf False \
--athenaopts '"--config-only=ConfigSimCG.pkl"'

Sim_tf.py \
--conditionsTag 'default:OFLCOND-MC21-SDR-RUN3-03' \
--simulator 'FullG4MT_QS' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'EVNTtoHITS:Campaigns/MC21Simulation.py,SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py,SimulationJobOptions/preInclude.ExtraParticles.py,SimulationJobOptions/preInclude.G4ExtraProcesses.py' \
--DataRunNumber '330000' \
--geometryVersion 'default:ATLAS-R3S-2021-02-00-00' \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/EVNT.04607198._000001.pool.root.1' \
--outputHITSFile 'test.HITS.pool.root' \
--maxEvents '10' \
--imf False

rc2=$?
mv log.EVNTtoHITS log.EVNTtoHITS_OLD
echo  "art-result: $rc2 simOLD"

rc3=-9999
if [ $rc -eq 0 ] && [ $rc2 -eq 0 ]
then
    acmd.py diff-root test.HITS.pool.root test.CA.HITS.pool.root --error-mode resilient --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings index_ref
    rc3=$?
fi
echo  "art-result: $rc3 OLDvsCA"

rc4=-9999
if [ $rc2 -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=semi-detailed --file=test.HITS.pool.root
    rc4=$?
fi
echo  "art-result: $rc4 regression"
