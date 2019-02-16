# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# default configuration of the PhotonIsEMSelectorCutDefs
# This one is used for stadard Loose photons cuts menus


# Import a needed helper
from PATCore.HelperUtils import *

# Define GeV
GeV = 1000.0

def PhotonIsEMLooseSelectorConfigDC14(theTool) :
    '''
    These are the photon isEM definitions *DC14* Loose 
    '''
    
    theTool = GetTool(theTool)

    #
    # PHOTON Loose cuts, with updated using *DC14*.
    #
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20150712/PhotonIsEMLooseSelectorCutDefs.conf"  

