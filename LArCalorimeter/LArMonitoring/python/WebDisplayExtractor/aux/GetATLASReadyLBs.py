#! /usr/bin/env python 

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, sys, string ,re

from ROOT import * 

sys.path.append("/afs/cern.ch/user/l/larmon/public/LArCalorimeter/LArMonitoring/python/MiscLibraries")
import MyCOOLlib

LB = []

if len(sys.argv) == 0  :
  sys.exit()
else:
  runnum = int(sys.argv[1])
  atlasready = MyCOOLlib.GetReadyFlag(runnum)
  for lb,ready in enumerate(atlasready):
    if ready: LB.append(lb+1)
print LB
