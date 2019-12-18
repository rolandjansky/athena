#!/bin/bash
# art-description: athenaMT trigger test for min bias reconstruction algorithms.
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export EVENTS=20
export THREADS=1
export SLOTS=1
export JOBOPTION="TrigUpgradeTest/MinBias.py"

# This test doesn't produce any output useful for RegTest, so do a dummy reference comparison
export REGTESTEXP="DEBUG REGTEST"
export REGTESTREF=`find_data.py TrigUpgradeTest/minbias.ref`

# Skip dumping chain counts because this test doesn't produce the histogram including them
export SKIP_CHAIN_DUMP=1

source exec_TrigUpgradeTest_art_athenaMT.sh
source exec_TrigUpgradeTest_art_post.sh
