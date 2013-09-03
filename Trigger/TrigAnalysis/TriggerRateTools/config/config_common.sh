#!/bin/bash
################################################################################
# Author: Marc-Andre Dufour
# Date: 14 May 2007
#
# Description: Main configuration for the TriggerRateTools package.
################################################################################

export LUMINOSITY=10000000.0 #in barn^-1 sec^-1
export MAIN_OUTPUT_PATH="/usatlas/groups/trigger/dufourma/14.2.23/batchJobs"
export CONFIG_PATH="$TriggerRateTools_PATH/config"
export SCRIPT_PATH="$TriggerRateTools_PATH/steps"
#export RUN_DIR="/usatlas/groups/trigger/dufourma/AtlasOffline-14.2.10/run"
export FOLDER=`date +%Y-%b-%d-%H_%M`
export RUN_DIR="/tmp/$FOLDER"
export PYTHON_DIR="generated_scripts"
#export NUM_EVENTS_PER_STEP1=10
export NUM_RDOS_PER_STEP1=10
export NUM_AODS_PER_STEP2=50
export NUM_AODS_PER_STEP3=1

# Configuration of environment setup
COMMANDS[1]='cd /usatlas/u/dufourma/testarea/AtlasOffline-14.2.23'
COMMANDS[2]='. ./init.sh'

export DO_BATCH=true
export SLEEP=0
export BATCH_SYS="condor"

export DO_LVL1="True"
export DO_LVL2="True"
export DO_EF="True"
export DISABLE_RANDOM="True"

export TRIGGER_MENU_SETUP="lumi1E31"
export TRIGGER_PRESCALES_LVL1="L1Prescales100_Physics_lumi1E31"
export TRIGGER_PRESCALES_HLT="HLTPrescales100_Physics_lumi1E31"

# DO NOT MODIFY BELOW THIS LINE


TRT_ENV=${COMMANDS[1]}
index=1
for cmd in "${COMMANDS[@]}"; do
    if [ $index -gt 1 ]; then
        TRT_ENV="$TRT_ENV:$cmd"
    fi
    index=$((index+1))
done

export TRT_ENV
