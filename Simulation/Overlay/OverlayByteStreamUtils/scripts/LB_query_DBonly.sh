#!/bin/bash
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Only works on lxplus now!

set -e

# TODO: read from args
#sstream="physics_ZeroBias" #pp or UPC
sstream="physics_MinBiasOverlay" #HI

runs=$(grep "subchild node value" lbn | cut -f 3 -d : | sed 's/[\x01-\x1F\x7F]//g' | sed 's%\[0m%%g' | sed 's%\[34m%%g' | sed ':a;N;$!ba;s/\n/ /g')
# loop over all runs
for runn in $runs; do
  if [[ ! -f "lbnevents_${runn}.txt" ]]; then 
    echo "Getting Nevent info for run $runn "
    LB_GetNEvents.py -r "$runn" -s "$sstream" | grep "... Run" | tee "lbnevents_${runn}.txt"
  fi
done
