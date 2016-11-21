# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# JetRecStandardTools.py
#
# David Adams
# March 2014
#
# Define the low-level tools used in jet reconstruction.
#
# Tools are configured and put in the global jet tool manager so
# they can be accessed when configuring JetRec tools.
#
# Execute this file to add the definitions to
# JetRecStandardToolManager.jtm, e.g.
#   import JetRecStandardTools

# Import the jet flags.
from JetRecFlags import jetFlags

from JetRecStandardToolManager import jtm

if not "UseTriggerStore " in locals():
  UseTriggerStore = False

# get levels defined VERBOSE=1 etc.
from GaudiKernel.Constants import *

try:
  from JetRecCalo.JetRecCaloConf import MissingCellListTool
  jtm.haveJetRecCalo = True
except ImportError:
  jtm.haveJetRecCalo = False

# Import configurations from jet packages
import JetMomentTools.DefaultTools
import JetSubStructureMomentTools.DefaultTools
import ParticleJetTools.DefaultTools
import JetRec.DefaultTools 
import PFlowUtils.DefaultTools
import JetRecTools.DefaultTools

JetMomentTools.DefaultTools.declareDefaultTools()
JetSubStructureMomentTools.DefaultTools.declareDefaultTools()
ParticleJetTools.DefaultTools.declareDefaultTools()
JetRec.DefaultTools.declareDefaultTools()
PFlowUtils.DefaultTools.declareDefaultTools()
JetRecTools.DefaultTools.declareDefaultTools()
