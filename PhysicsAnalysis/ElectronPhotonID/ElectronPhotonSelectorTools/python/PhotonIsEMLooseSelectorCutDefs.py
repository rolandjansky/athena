# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# default configuration of the PhotonIsEMSelectorCutDefs
# This one is used for stadard Loose photons cuts menus

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

def PhotonIsEMLooseSelectorConfigDC14(theTool) :
    '''
    These are the photon isEM definitions *DC14* Loose 
    '''
    
    theTool = GetTool(theTool)

    # the isEM name
    theTool.PIDName = egammaPID.IsEMLoose
    #
    # PHOTON Loose cuts, with updated using *DC14*.
    #
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/dc14b_20141031/PhotonIsEMLooseSelectorCutDefs.conf"  

