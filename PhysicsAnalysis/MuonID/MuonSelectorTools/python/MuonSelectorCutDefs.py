# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        MuonSelectorCutDefs
##
## Author:      Karsten Koeneke (CERN)
## Created:     July 2011
##
## Description: Define the cut values for the MuonSelectors
##
##=============================================================================

# Import a needed helper
from PATCore.HelperUtils import *

# Define GeV
GeV = 1000.0



def MuonSelectorConfig(theTool) :
    """
    This defines the cut values for the muon
    """
    theTool = GetTool(theTool)
    theTool.nPixMin     = 1
    theTool.nSCTMin     = 5
    theTool.nIDHolesMax = 2
    theTool.etaMax      = 1.9
    theTool.etaMin      = 0.1
    theTool.nTRTMin     = 6
    pass

def MuonSelectorConfig2011(theTool) :
    """
    This defines the cut values for the muons in 2011
    """
    theTool = GetTool(theTool)
    theTool.nBLMin      = 1
    theTool.nPixMin     = 2
    theTool.nSCTMin     = 6
    theTool.nIDHolesMax = 2
    theTool.etaMax      = 1.9
    theTool.etaMin      = 0.
    theTool.nTRTMin     = 6
    pass

def CaloMuonSelectorConfig(theTool) :
    """
    This defines the cut values for the muon
    """
    theTool = GetTool(theTool)
    theTool.nPixMin             = 1
    theTool.nSCTMin             = 5
    theTool.nIDHolesMax         = 2
    theTool.etaMax              = 1.9
    theTool.etaMin              = 0.1
    theTool.caloEtaMax          = 0.1
    theTool.caloMuonIDTagMin    = 11
    theTool.caloLRLikelihoodMin = 0.9
    pass

def StandAloneMuonSelectorConfig(theTool) :
    """
    This defines the cut values for the muon
    """
    theTool = GetTool(theTool)
    theTool.nCSCHitsMin   = 1
    theTool.nMDTEMHitsMin = 1
    theTool.nMDTEOHitsMin = 1
    pass

