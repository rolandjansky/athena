# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# default configuration of the ElectronIsEMSelectorCutDefs
# This one is used for loose++ menu

# Import a needed helper
from PATCore.HelperUtils import GetTool

# Define GeV
GeV = 1000.0

def ElectronIsEMLooseSelectorConfigDC14(theTool) :
    '''
    These are the cut base isEM definitions: Loose from MC15
    '''
    
    theTool = GetTool(theTool)
    
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20150329/ElectronIsEMLooseSelectorCutDefs.conf"


