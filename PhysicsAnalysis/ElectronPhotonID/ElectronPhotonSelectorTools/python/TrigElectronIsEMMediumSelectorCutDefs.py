# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##===============================================================================
## Name:        TrigEgammaElectronIsEMCutDefs_medium.py
##
## Author:      Ryan Mackenzie White
## Created:     June 2014
##
## Description: Medium trigger electron cut definitions for 2014 new tunes. 
##              
##===============================================================================

# Import a needed helper
from PATCore.HelperUtils import GetTool


def TrigElectronIsEMMediumSelectorConfigDC14(theTool) :
    '''
    This is for the Medium++ isEM definitions for the LATEST Trigger.
    '''
    theTool = GetTool(theTool)
    
    theTool.ConfigFile = "ElectronPhotonSelectorTools/trigger/mc15_20150329/ElectronIsEMMediumSelectorCutDefs.conf"


