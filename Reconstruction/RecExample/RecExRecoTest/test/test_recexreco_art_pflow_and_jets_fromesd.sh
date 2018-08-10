#!/bin/sh
#
# art-description: Athena runs topoclustering from an ESD file
# art-type: grid
athena eflowRec/run_ESDStandardReco.py
echo "art-result: $?"
