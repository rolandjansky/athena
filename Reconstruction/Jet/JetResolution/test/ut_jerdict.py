#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Test the reflex dictionary usage in PyROOT to 
# instantiate and configure the JER tools.

# Setup libs
import PyCintex
from ROOT import gROOT
gROOT.SetBatch()
gROOT.ProcessLine('.x $ROOTCOREDIR/scripts/load_packages.C')

# Import using the PyROOT bindings and reflex dictionary
from ROOT import xAOD
from ROOT import JERTool, JERSmearingTool

evt = xAOD.TEvent()

# Configure the tools
jerTool = JERTool('JERTool')
smearTool = JERSmearingTool('JERSmearingTool')
smearTool.setProperty(bool)('ApplyNominalSmearing', True)
smearTool.setProperty('JERToolName', jerTool.name())

# Initialize the tools
jerTool.initialize()
smearTool.initialize()
