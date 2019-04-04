#!/bin/sh
# art-description: athenaMT trigger test using the photon menu from TrigUpgradeTest job options
# art-type: build
# art-include: master/Athena
# art-output: *.log
# art-output: *.root
# art-output: *.regtest.new

export NAME="TrigUpgradeTest_photon_menu"
export SKIPEVENTS=0
export EVENTS=10
export THREADS=1
export SLOTS=1
export JOBOPTION="TrigUpgradeTest/full_menu.py"
export REGTESTEXP="TriggerSummaryStep.*HLT_.*|TriggerMonitorFinal.*HLT_.*|TrigSignatureMoniMT.*HLT_.*"
export EXTRA="doEmptyMenu=True;doPhotonSlice=True"

# Find the regtest reference installed with the release
export REGTESTREF=`find_data.py TrigUpgradeTest/photon_menu.ref`

source exec_TrigUpgradeTest_art_athenaMT.sh
source exec_TrigUpgradeTest_art_post.sh

