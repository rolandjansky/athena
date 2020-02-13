#!/usr/bin/env python

from __future__ import print_function

# Import ROOT without graphics
from ROOT import gROOT
gROOT.SetBatch()

# Load lib
from ROOT import gSystem
gSystem.Load('libPATInterfaces.so')

# Import the systematics classes
from ROOT.CP import SystematicVariation

# Test equality operation
s1 = SystematicVariation('sys', 1)
s2 = SystematicVariation('sys', 1)
if s1 == s2:
    pass
else:
    print (s1.name())
    print (s2.name())
    raise Exception('Equality test failed')
