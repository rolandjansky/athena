# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# default configuration of the PhotonIsEMSelectorCutDefs
# This one is used for stadard Loose photons cuts menus

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

#
# The "current" Loose ID menu, to be picked up by the rest of Athena.
#
def PhotonIsEMLooseSelectorConfig(theTool) :
    '''
    These are the photon isEM definitions for Loose
    '''
    
    theTool = GetTool(theTool)

    #
    # PHOTON Loose cuts, with updated using *DC14*.
    #
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20150712/PhotonIsEMLooseSelectorCutDefs.conf"  

