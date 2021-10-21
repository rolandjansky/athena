# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# default configuration of the PhotonIsEMSelectorCutDefs

# Import a needed helper
from PATCore.HelperUtils import GetTool


def PhotonIsEMLooseSelectorConfig(theTool):
    '''
    These are the photon isEM definitions *DC14* Loose
    '''

    theTool = GetTool(theTool)

    #
    # PHOTON Loose cuts, with updated using *DC14*.
    #
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20150712/PhotonIsEMLooseSelectorCutDefs.conf"


def PhotonIsEMMediumSelectorConfig(theTool):
    '''
    These are the photon isEM definitions from *DC14*
    '''

    theTool = GetTool(theTool)

    # MEDIUM (20/11/2013 -- Ludovica@cern.ch)
    #  Coming from Fer
    #
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20150712/PhotonIsEMMediumSelectorCutDefs.conf"


# Current cut-based tight menu. When updating tight, simply change the config-file below
# (and please add a meaningful description of the conf file)
# Note: keep this conf file up to date with the PhotonCutPointToConfFile map in Root/EGSelectorConfigurationMapping.h


def PhotonIsEMTightSelectorConfig(theTool):
    '''
    These are the photon isEM definitions for Tight menu
    '''

    theTool = GetTool(theTool)

    #
    # Pt-dependent tight cuts, derived in August 2018 ("v11")
    #
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/20180825/PhotonIsEMTightSelectorCutDefs.conf"

#
# Pt-inclusive tight ID menu, derived in January 2018.
# Needs to be kept around until the new menu is fully supported,
# but can be removed when it becomes obsolete.
#


def PhotonIsEMTightSelectorConfigPtInclJan2018(theTool):
    '''
    These are the photon isEM definitions for Tight menu
    '''

    theTool = GetTool(theTool)

    #
    # PHOTON tight cuts, now with January 2018 re-optimization
    #
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/20180116/PhotonIsEMTightSelectorCutDefs.conf"
