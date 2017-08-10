# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# default configuration of the PhotonIsEMSelectorCutDefs
# This one is used for stadard Tight photons cuts menus

import cppyy
try :
    cppyy.loadDictionary('ElectronPhotonSelectorToolsDict')
except :
    pass

from ROOT import egammaPID

# Import a needed helper
from PATCore.HelperUtils import *

# Define GeV
GeV = 1000.0

def ForwardElectronIsEMTightSelectorConfigMC15(theTool) :
    '''
    These are the photon isEM definitions for Tight menu
    '''
    
    theTool = GetTool(theTool)

    #
    # PHOTON tight cuts, with updated using *MC15*
    #
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20170711/ForwardElectronIsEMTightSelectorCutDefs.conf"

