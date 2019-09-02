#!/bin/bash
# art-description: athenaMT trigger test using the egamma.withViews.py job options
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export SKIPEVENTS=10
export EVENTS=20
export THREADS=1
export SLOTS=1
export JOBOPTION="TrigUpgradeTest/egamma.withViews.py"
export REGTESTEXP="TrigSignatureMoniMT.*HLT_.*|Fragment size"
export REGTESTREF=`find_data.py TrigUpgradeTest/egamma_cf.ref` # Find the regtest reference installed with the release
export ESDTOCHECK="myESD.pool.root"
export AODTOCHECK="myESD.pool.root"

source exec_TrigUpgradeTest_art_athenaMT.sh
source exec_TrigUpgradeTest_art_post.sh

# Extra step running checkESD job options
export JOB_LOG="athena.checkESD.log"
export JOBOPTION="TrigUpgradeTest/checkESD.py"
export INPUT=${ESDTOCHECK}
export SKIPEVENTS=0
source exec_TrigUpgradeTest_art_athenaMT.sh
