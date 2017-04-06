#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
__doc__ = "To print the vmem usage before and after instantiating egammaMVACalib"

def Vmem():
  "Return the Vmem usage in kB"
  import os, commands
  cmd = "cat /proc/%d/status | grep VmSize | awk '{print($2)}'" % os.getpid()
  return int( commands.getoutput(cmd) )

# initial
memUsage = [Vmem()]

# import ROOT and load dictionary
import ROOT, cppyy, numpy as np
cppyy.loadDict('egammaMVACalibDict')
memUsage.append( Vmem() )

# Electrons
m = ROOT.egammaMVACalib(1, True, 'egammaMVACalib/online/v3')
memUsage.append( Vmem() )

# Photons
m = ROOT.egammaMVACalib(0, True, 'egammaMVACalib/online/v3')
memUsage.append( Vmem() )

print np.diff(memUsage)/1e6, 'GB'
