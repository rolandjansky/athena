#!/bin/bash
# art-description: athenaMT trigger test of MET from jets algorithm
# art-type: build
# art-include: master/Athena
# art-output: *.log
# art-output: *.new
# art-output: *.txt
# art-output: *.root

export EVENTS=10
export THREADS=1
export SLOTS=1
export JOBOPTION="TrigUpgradeTest/met.fromJets.py"
export REGTESTEXP="METHypoAlg.*MET.*value"

# Find the regtest reference installed with the release
export REGTESTREF=`find_data.py TrigUpgradeTest/met_fromJets.ref`

# Skip dumping chain counts because this test doesn't produce the histogram including them
export SKIP_CHAIN_DUMP=1

source exec_TrigUpgradeTest_art_athenaMT.sh
source exec_TrigUpgradeTest_art_post.sh
