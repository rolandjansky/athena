#!/bin/sh
#
# art-description: MC21-style simulation using FullG4, checking that the SkipEvents argument works (7 TeV ttbar input - needs updating)
# art-include: master/Athena
# art-include: master/AthSimulation
# art-type: grid
# art-output: hitsFull.ttbar.pool.root
# art-output: hitsHalf1.ttbar.pool.root
# art-output: hitsHalf2.ttbar.pool.root
# art-output: hitsMerged.ttbar.pool.root
# art-output: hitsFullMerged.ttbar.pool.root

# Run 10 events normally
Sim_tf.py \
--conditionsTag 'default:OFLCOND-MC21-SDR-RUN3-03' \
--simulator 'FullG4' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'EVNTtoHITS:Campaigns/MC21Simulation.py,SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py' \
--geometryVersion 'default:ATLAS-R3S-2021-02-00-00_VALIDATION' \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/mc12_valid.110401.PowhegPythia_P2012_ttbar_nonallhad.evgen.EVNT.e3099.01517252._000001.pool.root.1' \
--outputHITSFile 'hitsFull.ttbar.pool.root' \
--maxEvents '10' \
--skipEvents '0'

echo "art-result: $? unsplit-sim"

# Run first 5 events
Sim_tf.py \
--conditionsTag 'default:OFLCOND-MC21-SDR-RUN3-03' \
--simulator 'FullG4' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'EVNTtoHITS:Campaigns/MC21Simulation.py,SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py' \
--geometryVersion 'default:ATLAS-R3S-2021-02-00-00_VALIDATION' \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/mc12_valid.110401.PowhegPythia_P2012_ttbar_nonallhad.evgen.EVNT.e3099.01517252._000001.pool.root.1' \
--outputHITSFile 'hitsHalf1.ttbar.pool.root' \
--maxEvents '5' \
--skipEvents '0'

echo "art-result: $? split-sim1"

# Run next 5 events
Sim_tf.py \
--conditionsTag 'default:OFLCOND-MC21-SDR-RUN3-03' \
--simulator 'FullG4' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'EVNTtoHITS:Campaigns/MC21Simulation.py,SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py' \
--geometryVersion 'default:ATLAS-R3S-2021-02-00-00_VALIDATION' \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/mc12_valid.110401.PowhegPythia_P2012_ttbar_nonallhad.evgen.EVNT.e3099.01517252._000001.pool.root.1' \
--outputHITSFile 'hitsHalf2.ttbar.pool.root' \
--maxEvents '5' \
--skipEvents '5'

echo "art-result: $? split-sim2"

# Merge the partial files
HITSMerge_tf.py --inputHitsFile hitsHalf1.ttbar.pool.root \
                                hitsHalf2.ttbar.pool.root \
                --outputHitsFile 'hitsMerged.ttbar.pool.root'

echo "art-result: $? split-merge"

# Run a dummy merge on the full hits file to deal with lossy compression:
HITSMerge_tf.py --inputHitsFile 'hitsFull.ttbar.pool.root' --outputHitsFile 'hitsFullMerged.ttbar.pool.root'

echo "art-result: $? dummy-merge"

# Compare the merged outputs
acmd.py diff-root hitsFullMerged.ttbar.pool.root hitsMerged.ttbar.pool.root --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings index_ref

echo "art-result: $? comparison"
