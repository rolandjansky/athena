#! /usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from future import standard_library
standard_library.install_aliases()

__doc__ = "To print the vmem usage before and after instantiating egammaMVACalib"

def Vmem():
  "Return the Vmem usage in kB"
  import os, subprocess
  cmd = "cat /proc/%d/status | grep VmSize | awk '{print($2)}'" % os.getpid()
  return int( subprocess.getoutput(cmd) )

# initial
memUsage = [Vmem()]

# import ROOT and load dictionary
import ROOT, cppyy, numpy as np # noqa: F401
cppyy.loadDict('egammaMVACalibDict')
memUsage.append( Vmem() )

# Electrons
m = ROOT.egammaMVACalib(1, True, 'egammaMVACalib/online/v3')
memUsage.append( Vmem() )

# Photons
m = ROOT.egammaMVACalib(0, True, 'egammaMVACalib/online/v3')
memUsage.append( Vmem() )

for i in memUsage:
  print (i/1e6 , 'GB')
