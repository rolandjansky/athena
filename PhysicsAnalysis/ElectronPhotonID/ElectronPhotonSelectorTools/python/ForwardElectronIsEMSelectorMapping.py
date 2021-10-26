# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# =============================================================================
# Name:        ForwardElectronIsEMSelectorMapping.py
#
# Author:      Tulay Cuhadar Donszelmann, Jovan Mitrevski
# Created:     Dec 2011
##
# Description: Find mapping of mask and function for ID quality
# =============================================================================

from ElectronPhotonSelectorTools.EgammaPIDdefs import egammaPID
#
import ElectronPhotonSelectorTools.ForwardElectronIsEMMenuDefs as ForwardElectronIsEMSelectorCutDefs


class forwardelectronPIDmenu:
    menuMC15 = 0


# format - key: (mask, function)

ForwardElectronIsEMMapMC15 = {
    egammaPID.ForwardElectronIDLoose: (
        egammaPID.ID_ForwardElectron,
        ForwardElectronIsEMSelectorCutDefs.ForwardElectronIsEMLooseSelectorConfigMC15),
    egammaPID.ForwardElectronIDMedium: (
        egammaPID.ID_ForwardElectron,
        ForwardElectronIsEMSelectorCutDefs.ForwardElectronIsEMMediumSelectorConfigMC15),
    egammaPID.ForwardElectronIDTight: (
        egammaPID.ID_ForwardElectron,
        ForwardElectronIsEMSelectorCutDefs.ForwardElectronIsEMTightSelectorConfigMC15),
    egammaPID.NoIDCut: (
        0,
        ForwardElectronIsEMSelectorCutDefs.ForwardElectronIsEMLooseSelectorConfigMC15)
}


def ForwardElectronIsEMMap(quality, menu):
    if menu == forwardelectronPIDmenu.menuMC15:
        return ForwardElectronIsEMMapMC15[quality]
    else:
        raise ValueError("Requested menu is undefined: %d" % menu)
