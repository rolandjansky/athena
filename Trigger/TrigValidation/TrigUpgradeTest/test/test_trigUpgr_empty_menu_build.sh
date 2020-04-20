#!/bin/bash
# art-description: athenaMT trigger test on data running no HLT menu
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export EVENTS=20
export THREADS=2
export SLOTS=2
export JOBOPTION="TriggerJobOpts/runHLT_standalone.py"
export EXTRA="isOnline=True;doEmptyMenu=True;doWriteBS=False;doWriteRDOTrigger=True;"

# Skip dumping chain counts because this test doesn't produce the histogram including them
export SKIP_CHAIN_DUMP=1

source exec_TrigUpgradeTest_art_athenaMT.sh
source exec_TrigUpgradeTest_art_post.sh
