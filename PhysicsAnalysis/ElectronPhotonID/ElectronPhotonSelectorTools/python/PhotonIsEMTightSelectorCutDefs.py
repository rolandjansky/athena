# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# default configuration of the PhotonIsEMSelectorCutDefs
# This one is used for stadard Tight photons cuts menus

import PyCintex
try :
    PyCintex.loadDictionary('ElectronPhotonSelectorToolsDict')
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

    # the isEM name
    theTool.PIDName = egammaPID.IsEMTight
    #
    # PHOTON tight cuts, with updated using *DC14*
    #
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/dc14b_20141031/PhotonIsEMTightSelectorCutDefs.conf"   

