# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# default configuration of the PhotonIsEMSelectorCutDefs
# This one is used for stadard Loose photons cuts menus

# Import a needed helper
from PATCore.HelperUtils import *

# Define GeV
GeV = 1000.0

def ForwardElectronIsEMLooseSelectorConfigMC15(theTool) :
    '''
    These are the photon isEM definitions *MC15* Loose 
    '''
    
    theTool = GetTool(theTool)

    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20170711/ForwardElectronIsEMLooseSelectorCutDefs.conf"

