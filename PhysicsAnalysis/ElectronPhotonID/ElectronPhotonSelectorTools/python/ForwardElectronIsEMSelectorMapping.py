# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        ForwardElectronIsEMSelectorMapping.py
##
## Author:      Tulay Cuhadar Donszelmann, Jovan Mitrevski
## Created:     Dec 2011
##
## Description: Find mapping of mask and function for ID quality
##=============================================================================

import ROOT,cppyy
cppy.include("ElectronPhotonSelectorTools/egammaPIDdefs.h")

from ROOT import egammaPID

class forwardelectronPIDmenu:
    menuMC15 = 0

import ElectronPhotonSelectorTools.ForwardElectronIsEMLooseSelectorCutDefs as ForwardElectronIsEMLooseSelectorCutDefs
import ElectronPhotonSelectorTools.ForwardElectronIsEMMediumSelectorCutDefs as ForwardElectronIsEMMediumSelectorCutDefs
import ElectronPhotonSelectorTools.ForwardElectronIsEMTightSelectorCutDefs as ForwardElectronIsEMTightSelectorCutDefs

# format - key: (mask, function)


ForwardElectronIsEMMapMC15 = {
    egammaPID.ForwardElectronIDLoose: ( egammaPID.ID_ForwardElectron, ForwardElectronIsEMLooseSelectorCutDefs.ForwardElectronIsEMLooseSelectorConfigMC15 ),
    egammaPID.ForwardElectronIDMedium: ( egammaPID.ID_ForwardElectron, ForwardElectronIsEMMediumSelectorCutDefs.ForwardElectronIsEMMediumSelectorConfigMC15 ),
    egammaPID.ForwardElectronIDTight: ( egammaPID.ID_ForwardElectron, ForwardElectronIsEMTightSelectorCutDefs.ForwardElectronIsEMTightSelectorConfigMC15 ),
    egammaPID.NoIDCut: ( 0, ForwardElectronIsEMLooseSelectorCutDefs.ForwardElectronIsEMLooseSelectorConfigMC15 )
    }


def ForwardElectronIsEMMap(quality, menu):
    if menu == forwardelectronPIDmenu.menuMC15:
        return ForwardElectronIsEMMapMC15[quality]
    else:
        raise ValueError("Requested menu is undefined: %d" % menu)
    
