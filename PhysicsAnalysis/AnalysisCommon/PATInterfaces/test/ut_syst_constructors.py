#!/usr/bin/env python

# Import ROOT without graphics
from ROOT import gROOT
gROOT.SetBatch()

# Load lib
from ROOT import gSystem
gSystem.Load('libPATInterfaces.so')

# Import the systematics classes
from ROOT.CP import SystematicVariation

# Test constructor resolution
sys = SystematicVariation('sys', 1)
if sys.name() != 'sys__1up':
    raise Exception('Constructor resolution failed')
