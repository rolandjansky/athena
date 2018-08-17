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
AtlasG4_tf.py \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/ttbar_muplusjets-pythia6-7000.evgen.pool.root' \
--DataRunNumber '222510' \
--geometryVersion 'ATLAS-R2-2015-03-01-00' \
--conditionsTag 'OFLCOND-RUN12-SDR-21' \
--outputHITSFile 'hitsFull.ttbar.pool.root' \
--maxEvents '10' \
--skipEvents '0'

echo "art-result: $? unsplit-sim"

# Run first 5 events
AtlasG4_tf.py \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/ttbar_muplusjets-pythia6-7000.evgen.pool.root' \
--DataRunNumber '222510' \
--geometryVersion 'ATLAS-R2-2015-03-01-00' \
--conditionsTag 'OFLCOND-RUN12-SDR-21' \
--outputHITSFile 'hitsHalf1.ttbar.pool.root' \
--maxEvents '5' \
--skipEvents '0'

echo "art-result: $? split-sim1"

# Run next 5 events
AtlasG4_tf.py \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/ttbar_muplusjets-pythia6-7000.evgen.pool.root' \
--DataRunNumber '222510' \
--geometryVersion 'ATLAS-R2-2015-03-01-00' \
--conditionsTag 'OFLCOND-RUN12-SDR-21' \
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
acmd.py diff-root hitsFullMerged.ttbar.pool.root hitsMerged.ttbar.pool.root --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings

echo "art-result: $? comparison"
