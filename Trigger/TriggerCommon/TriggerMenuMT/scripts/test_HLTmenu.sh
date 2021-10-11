#!/usr/bin/env bash

if [ $# -ne 1 ]; then
   echo "Syntax: `basename $0` menu"
   exit 1
fi

menu=$1

# Fail if any command fails:
set -e

# Run athena:
athena --threads=1 --filesInput="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1" -c "setMenu='${menu}';endJobAfterGenerate=True" TriggerJobOpts/runHLT_standalone.py

# Verify the menu:
verify_menu_config.py -f $PWD || { echo "ERROR: Menu verification failed" ; exit 1; }
