#!/bin/bash
# art-description: athenaMT trigger test using the full LS2_v1 menu from TriggerMenuMT. Reverse EventView order to test their independence
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export SKIPEVENTS=10
export EVENTS=20
export THREADS=1
export SLOTS=1
export JOBOPTION="TrigUpgradeTest/full_menu.py"
export REGTESTEXP="TriggerSummaryStep.*HLT_.*|TriggerMonitorFinal.*HLT_.*|TrigSignatureMoniMT.*HLT_.*"
export EXTRA="isOnline=True;reverseViews=True;"

# Find the regtest reference installed with the release
export REGTESTREF=`find_data.py TrigUpgradeTest/full_menu_build.ref`

source exec_TrigUpgradeTest_art_athenaMT.sh
source exec_TrigUpgradeTest_art_post.sh
