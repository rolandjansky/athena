# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# =============================================================================
# Name:        ElectronIsEMSelectorMapping.py
##
# Author:      Tulay Cuhadar Donszelmann, Jovan Mitrevski
# Created:     Dec 2011
##
# Description: Find mapping of mask and function for ID quality
# =============================================================================

import ElectronPhotonSelectorTools.ElectronIsEMMenuDefs as ElectronIsEMSelectorCutDefs
from ElectronPhotonSelectorTools.EgammaPIDdefs import egammaPID


class electronPIDmenu:
    menuTrigDC14 = 0
    menuDC14 = 1


# format - key: (mask, function)
ElectronIsEMMapDC14 = {
    egammaPID.ElectronIDLoosePP: (
        egammaPID.ElectronLoosePP,
        ElectronIsEMSelectorCutDefs.ElectronIsEMLooseSelectorConfigDC14),
    egammaPID.ElectronIDMediumPP: (
        egammaPID.ElectronMediumPP,
        ElectronIsEMSelectorCutDefs.ElectronIsEMMediumSelectorConfigDC14),
    egammaPID.ElectronIDTightPP: (
        egammaPID.ElectronTightPP,
        ElectronIsEMSelectorCutDefs.ElectronIsEMTightSelectorConfigDC14),
    egammaPID.NoIDCut: (
        0,
        ElectronIsEMSelectorCutDefs.ElectronIsEMLooseSelectorConfigDC14)
}


TrigElectronIsEMMapDC14 = {
    egammaPID.ElectronIDLooseHLT:
    (egammaPID.ElectronLooseHLT,
     ElectronIsEMSelectorCutDefs.TrigElectronIsEMLooseSelectorConfigDC14),
    egammaPID.ElectronIDMediumHLT:
    (egammaPID.ElectronMediumHLT,
     ElectronIsEMSelectorCutDefs.TrigElectronIsEMMediumSelectorConfigDC14),
    egammaPID.ElectronIDTightHLT:
    (egammaPID.ElectronTightHLT,
     ElectronIsEMSelectorCutDefs.TrigElectronIsEMTightSelectorConfigDC14),
}


def ElectronIsEMMap(quality, menu):
    if menu == electronPIDmenu.menuDC14:
        return ElectronIsEMMapDC14[quality]
    elif menu == electronPIDmenu.menuTrigDC14:
        return TrigElectronIsEMMapDC14[quality]
    else:
        raise ValueError("Requested menu is undefined: %d" % menu)
