#!/bin/bash

########################################################
#
# Update env variables for running in standalone mode
# on D3PDs (ROOT ntuples) with tauid-redo
# or scripts in taumva (see README)
#
# - Noel Dawe
#
########################################################

# deterine path to this script
# http://stackoverflow.com/questions/59895/can-a-bash-script-tell-what-directory-its-stored-in
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ] ; do SOURCE="$(readlink "$SOURCE")"; done
TD_DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

export LD_LIBRARY_PATH=${TD_DIR}/StandAlone${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
export PATH=${TD_DIR}/run${PATH:+:$PATH}
export DATAPATH=${TD_DIR}/share${DATAPATH:+:$DATAPATH}
