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

def PhotonIsEMTightSelectorConfigDC14(theTool) :
    '''
    These are the photon isEM definitions for Tight menu
    '''
    
    theTool = GetTool(theTool)

    #
    # PHOTON tight cuts, now with January 2018 re-optimization
    #
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/20180116/PhotonIsEMTightSelectorCutDefs.conf"

