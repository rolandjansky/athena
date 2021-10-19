# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##===============================================================================
## Name:        TrigEgammaElectronIsEMCutDefs_loose.py
##
## Author:      Ryan Mackenzie White
## Created:     June 2014
##
## Description: Loose trigger electron cut definitions for 2014 new tunes. 
## Optimized by S. Kahn            
##===============================================================================

# Import a needed helper
from PATCore.HelperUtils import GetTool


def TrigElectronIsEMLooseSelectorConfigDC14(theTool) :
    '''
    This is for the Loose isEM definitions for the Trigger.
    '''
 
    theTool = GetTool(theTool)
    
    theTool.ConfigFile = "ElectronPhotonSelectorTools/trigger/mc15_20150329/ElectronIsEMLooseSelectorCutDefs.conf"


