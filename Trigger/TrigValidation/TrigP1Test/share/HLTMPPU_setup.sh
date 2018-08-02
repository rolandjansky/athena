#!/bin/bash

# Script to setup the EOS build of HLTMPPU
# Author: Leonardo Badurina
# Date: 01/08/18

# Determines paths before sourcing the HLTMPPU setup script
CORAL_AUTH_PATH_GOOD="$(printenv CORAL_AUTH_PATH)"
CORAL_DBLOOKUP_PATH_GOOD="$(printenv CORAL_DBLOOKUP_PATH)"

# Gives priority to local installation.
source /eos/atlas/atlascerngroupdisk/data-art/grid-input/TrigP1Test/HLTMPPU/installed/setup.sh 

# Overwrites new paths with correct paths
export CORAL_AUTH_PATH="${CORAL_AUTH_PATH_GOOD}"
export CORAL_DBLOOKUP_PATH="${CORAL_DBLOOKUP_PATH_GOOD}"
