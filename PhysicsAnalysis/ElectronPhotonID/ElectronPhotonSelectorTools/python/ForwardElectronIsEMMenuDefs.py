# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# default configuration of the Forward Electron IsEM Selectors
# Import a needed helper
from PATCore.HelperUtils import GetTool

# Define GeV
GeV = 1000.0


def ForwardElectronIsEMLooseSelectorConfigMC15(theTool):
    '''
    These are the forward electron isEM definitions Loose
    '''

    theTool = GetTool(theTool)

    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20170711/ForwardElectronIsEMLooseSelectorCutDefs.conf"


def ForwardElectronIsEMMediumSelectorConfigMC15(theTool):
    '''
    These are the forward electron isEM definitions  Medium
    '''

    theTool = GetTool(theTool)

    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20170711/ForwardElectronIsEMMediumSelectorCutDefs.conf"


def ForwardElectronIsEMTightSelectorConfigMC15(theTool):
    '''
    These are the forward electron isEM definitions  Tight
    '''

    theTool = GetTool(theTool)

    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20170711/ForwardElectronIsEMTightSelectorCutDefs.conf"
