# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        JetCleaningCutDefs
##
## Author:      Karsten Koeneke (CERN)
## Created:     July 2011
##
## Description: Define the cut values for jet cleaning
##
##=============================================================================

# Import a needed helper
from PATCore.HelperUtils import GetTool


def JetCleaningToolConfig_Loose(theTool) :
    """
    This defines the jet cleaning cut values for the Loose operating point.
    """
    theTool = GetTool(theTool)
    theTool.CutLevel = 'LooseBad'
    pass

def JetCleaningToolConfig_Tight(theTool) :
    """
    This defines the jet cleaning cut values for the Tight operating point.
    """
    theTool = GetTool(theTool)
    theTool.CutLevel = 'TightBad'
    pass

