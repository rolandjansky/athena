#!/usr/bin/env bash

# Fail if any command fails:
set -e

# Job options snippet to exit after configuration:
exit_py=`mktemp --suffix=_exit.py`
trap 'rm -f -- "${exit_py}"' EXIT
echo "theApp.exit()" > ${exit_py}

# Run athena:
athena --threads=1 --filesInput="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1" -c "setMenu='LS2_v1'" TriggerJobOpts/runHLT_standalone.py ${exit_py}

# Verify the menu:
verify_menu_config.py -f $PWD || { echo "ERROR: Menu verification failed" ; exit 1; }
