# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# default configuration of the PhotonIsEMSelectorCutDefs
# This one is used for stadard photons cuts menus


# Import a needed helper
from PATCore.HelperUtils import GetTool


def PhotonIsEMMediumSelectorConfig(theTool) :
    '''
    These are the photon isEM definitions from *DC14*
    '''
    
    theTool = GetTool(theTool)

    # MEDIUM (20/11/2013 -- Ludovica@cern.ch)
    #  Coming from Fer
    #
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20150712/PhotonIsEMMediumSelectorCutDefs.conf"


