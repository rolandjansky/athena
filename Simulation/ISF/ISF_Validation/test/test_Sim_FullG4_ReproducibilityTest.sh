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
# art-output: hitsFull.ttbar.pool.root
# art-output: hitsHalf1.ttbar.pool.root
# art-output: hitsHalf2.ttbar.pool.root
# art-output: hitsMerged.ttbar.pool.root
# art-output: hitsFullMerged.ttbar.pool.root

# Run 10 events normally
Sim_tf.py \
--conditionsTag 'default:OFLCOND-RUN12-SDR-19' \
--physicsList 'FTFP_BERT' \
--truthStrategy 'MC15aPlus' \
--simulator 'FullG4' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py' \
--DataRunNumber '222525' \
--geometryVersion 'default:ATLAS-R2-2015-03-01-00' \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/mc12_valid.110401.PowhegPythia_P2012_ttbar_nonallhad.evgen.EVNT.e3099.01517252._000001.pool.root.1' \
--outputHITSFile 'hitsFull.ttbar.pool.root' \
--maxEvents '10' \
--skipEvents '0'

echo "art-result: $? unsplit-sim"

# Run first 5 events
Sim_tf.py \
--conditionsTag 'default:OFLCOND-RUN12-SDR-19' \
--physicsList 'FTFP_BERT' \
--truthStrategy 'MC15aPlus' \
--simulator 'FullG4' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py' \
--DataRunNumber '222525' \
--geometryVersion 'default:ATLAS-R2-2015-03-01-00' \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/mc12_valid.110401.PowhegPythia_P2012_ttbar_nonallhad.evgen.EVNT.e3099.01517252._000001.pool.root.1' \
--outputHITSFile 'hitsHalf1.ttbar.pool.root' \
--maxEvents '5' \
--skipEvents '0'

echo "art-result: $? split-sim1"

# Run next 5 events
Sim_tf.py \
--conditionsTag 'default:OFLCOND-RUN12-SDR-19' \
--physicsList 'FTFP_BERT' \
--truthStrategy 'MC15aPlus' \
--simulator 'FullG4' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py' \
--DataRunNumber '222525' \
--geometryVersion 'default:ATLAS-R2-2015-03-01-00' \
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
acmd.py diff-root hitsFullMerged.ttbar.pool.root hitsMerged.ttbar.pool.root --error-mode resilient --mode=semi-detailed --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings

echo "art-result: $? comparison"
