#!/bin/sh
# art-description: athenaMT HLT emulation test
# art-type: build
# art-include: master/Athena

# This is a unit test of HLT Control Flow and should be moved to TriggerMenuMT

export THREADS=1
export EVENTS=4
export SLOTS=1
export JOBOPTION="TrigUpgradeTest/EmuStepProcessingTest.py"
export REGTESTEXP="TrigSignatureMoniMT.*INFO HLT_.*|TrigSignatureMoniMT.*-- #[0-9]+ (Events|Features).*|TriggerSummaryStep.* chains passed:|TriggerSummaryStep.*+++ HLT_.*|TriggerSummaryStep.*+++ leg.*"
export DOPERFMON=0
export ATHENAOPTS=' -l DEBUG -c "DoMenu=True"'
export REGTESTREF=`find_data.py TrigUpgradeTest/emu_step_menu_processing.ref`


source exec_TrigUpgradeTest_art_athenaMT.sh
source exec_TrigUpgradeTest_art_post.sh
