#!/bin/bash
# art-description: athenaMT trigger test of BS decoding
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

# Common settings for all steps
export EVENTS=5
export THREADS=1
export SLOTS=1

# Remove the BS file from previous run
rm -rf data_test.*

# Step 1 - RDO to BS
export SKIPEVENTS=10
export JOB_LOG="athena.RDOtoBS.log"
export JOBOPTION="TrigUpgradeTest/egamma.withViews.py"
source exec_TrigUpgradeTest_art_athenaMT.sh

# Remove the ESD file from previous run
rm -rf  myESDfromBS.pool.root

# Step 2 - BS to ESD
export SKIPEVENTS=0
export JOB_LOG="athena.BStoESD.log"
export INPUT="data_test.00327265.Single_Stream.daq.RAW._lb0100._Athena._0000.data"
export JOBOPTION="TrigUpgradeTest/decodeBS.py"
export EXTRA="doL1Unpacking=False"
source exec_TrigUpgradeTest_art_athenaMT.sh

# Step 3 - post-processing of the results
export REGTESTEXP="FillMissingEDM.*(present|absent)|TrigDeserialiser.*fragment.*type"
export REGTESTREF=`find_data.py TrigUpgradeTest/decodeBS.ref`
export ESDTOCHECK="myESDfromBS.pool.root"
export AODTOCHECK="myESDfromBS.pool.root"
source exec_TrigUpgradeTest_art_post.sh
