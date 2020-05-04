#!/bin/bash
# art-description: athenaMT trigger test using the full menu from TrigUpgradeTest job options
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export EVENTS=10000
export THREADS=1
export SLOTS=1
export JOBOPTION="TrigUpgradeTest/full_menu_cf.py"
export REGTESTEXP="TriggerSummaryStep.*HLT_.*|TriggerMonitorFinal.*HLT_.*|TrigSignatureMoniMT.*HLT_.*|TrigSignatureMoniMT.*-- #[0-9]+ (Events|Features).*"
#export EXTRA="doWriteBS=False;doWriteRDOTrigger=True;"
export EXTRA="doEmptyMenu=True;forceEnableAllChais=True;isOnline=True;doWriteBS=False;doWriteRDOTrigger=True;"
#ex.args = '-c "isOnline=True;doWriteBS=False;doWriteRDOTrigger=True;"'
#ex.args = '-c "doEmptyMenu=True;doBjetSlice=True;isOnline=True;doWriteBS=False;doWriteRDOTrigger=True;"'
#ex.args = '-c "doEmptyMenu=True;forceEnableAllChais=True;isOnline=True;doWriteBS=False;doWriteRDOTrigger=True;"'

# Find the regtest reference installed with the release
export REGTESTREF=`find_data.py TrigUpgradeTest/full_menu_cf_build.ref`

source exec_TrigUpgradeTest_art_athenaMT.sh
source exec_TrigUpgradeTest_art_post.sh
