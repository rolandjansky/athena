#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Test the reflex dictionary usage in PyROOT to 
# instantiate and configure the JER tools.

import sys

# Setup libs
from ROOT import gROOT
gROOT.SetBatch()
gROOT.Macro('$ROOTCOREDIR/scripts/load_packages.C')

# Import using the PyROOT bindings and reflex dictionary
from ROOT import xAOD
from ROOT import JERTool, JERSmearingTool

evt = xAOD.TEvent()

# Configure the tools
jerTool = JERTool('JERTool')
smearTool = JERSmearingTool('JERSmearingTool')
if smearTool.setProperty('JERToolName', jerTool.name()).isFailure():
    sys.exit(1)

# Initialize the tools
if jerTool.initialize().isFailure():
    sys.exit(1)
if smearTool.initialize().isFailure():
    sys.exit(1)
