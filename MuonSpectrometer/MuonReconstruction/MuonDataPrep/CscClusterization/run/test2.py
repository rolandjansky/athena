# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# test2.py
#
# David Adams
# April 2007
#
# Test load with job options.
#
# Usage: athena.py test1.py

import traceback
from sys import *

theApp.EvtMax = 2

print "hello"

# Geometry.
# Neede for tools and algorithm to initialize.
include("CscValidationUtil/MuonGeometry.py")

# Load the clusterization algorithm.
include("CscClusterization/CscThresholdClusterizationOptions.py")

topalgs = AlgSequence()
print topalgs

print "goodbye"

# Exit before initialization.
#sys.exit(0)
