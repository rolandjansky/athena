#!/bin/bash
#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

baseFileName="trig_edmr3_repr_test_"
rm -f ${baseFileName}*

for i in `seq 3`; do
  python -c "from TrigEDMConfig.testEDMRun3 import dumpListToJson; dumpListToJson('${baseFileName}${i}.json')"
done

diff ${baseFileName}1.json ${baseFileName}2.json >${baseFileName}diff1.txt
ret1=$?

diff ${baseFileName}1.json ${baseFileName}3.json >${baseFileName}diff2.txt
ret2=$?

if [[ $ret1 != 0 || $ret2 != 0 ]]; then
  echo "ERROR Trigger EDM Run3 list is irreproducible"
  exit 1
fi
