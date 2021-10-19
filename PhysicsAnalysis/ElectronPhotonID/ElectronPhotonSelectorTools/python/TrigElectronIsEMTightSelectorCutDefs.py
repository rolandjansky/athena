# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##===============================================================================
## Name:        TrigEgammaElectronIsEMCutDefs_tight.py
##
## Author:      Ryan Mackenzie White
## Created:     June 2014
##
## Description: Tight trigger electron cut definitions for 2014 new tunes. 
##              
##===============================================================================

# Import a needed helper
from PATCore.HelperUtils import GetTool


def TrigElectronIsEMTightSelectorConfigDC14(theTool) :
    '''
    This is for the Tight MC15 LATEST isEM definitions for the Trigger.
    '''
    
    theTool = GetTool(theTool)
    
    # the isEM name
    theTool.ConfigFile = "ElectronPhotonSelectorTools/trigger/mc15_20150329/ElectronIsEMTightSelectorCutDefs.conf"

