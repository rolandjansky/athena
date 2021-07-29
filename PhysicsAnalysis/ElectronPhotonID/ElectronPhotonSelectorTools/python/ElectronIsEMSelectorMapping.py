# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# =============================================================================
# Name:        ElectronIsEMSelectorMapping.py
##
# Author:      Tulay Cuhadar Donszelmann, Jovan Mitrevski
# Created:     Dec 2011
##
# Description: Find mapping of mask and function for ID quality
# =============================================================================


import ElectronPhotonSelectorTools.TrigElectronIsEMLooseSelectorCutDefs as TrigElectronIsEMLooseSelectorCutDefs
import ElectronPhotonSelectorTools.TrigElectronIsEMTight1SelectorCutDefs as TrigElectronIsEMTight1SelectorCutDefs
import ElectronPhotonSelectorTools.TrigElectronIsEMLoose1SelectorCutDefs as TrigElectronIsEMLoose1SelectorCutDefs
import ElectronPhotonSelectorTools.TrigPhotonIsEMCaloCutsOnlySelectorCutDefs as TrigPhotonIsEMCaloCutsOnlySelectorCutDefs
import ElectronPhotonSelectorTools.TrigElectronIsEMTightSelectorCutDefs as TrigElectronIsEMTightSelectorCutDefs
import ElectronPhotonSelectorTools.TrigElectronIsEMMediumSelectorCutDefs as TrigElectronIsEMMediumSelectorCutDefs
import ElectronPhotonSelectorTools.TrigElectronIsEMMedium1SelectorCutDefs as TrigElectronIsEMMedium1SelectorCutDefs
import ElectronPhotonSelectorTools.ElectronIsEMTightSelectorCutDefs as ElectronIsEMTightSelectorCutDefs
import ElectronPhotonSelectorTools.ElectronIsEMMediumSelectorCutDefs as ElectronIsEMMediumSelectorCutDefs
import ElectronPhotonSelectorTools.ElectronIsEMLooseSelectorCutDefs as ElectronIsEMLooseSelectorCutDefs

#
from ElectronPhotonSelectorTools.EgammaPIDdefs import egammaPID


class electronPIDmenu:
    menu2012 = 0
    menuTrig2012 = 1
    menuTrigDC14 = 2
    menuDC14 = 3


# format - key: (mask, function)
ElectronIsEMMap2012 = {
    egammaPID.ElectronIDLoosePP: (
        egammaPID.ElectronLoosePP,
        ElectronIsEMLooseSelectorCutDefs.ElectronIsEMLooseSelectorConfig2012),
    egammaPID.ElectronIDMediumPP: (
        egammaPID.ElectronMediumPP,
        ElectronIsEMMediumSelectorCutDefs.ElectronIsEMMediumSelectorConfig2012),
    egammaPID.ElectronIDTightPP: (
        egammaPID.ElectronTightPP,
        ElectronIsEMTightSelectorCutDefs.ElectronIsEMTightSelectorConfig2012),
    egammaPID.NoIDCut: (
        0,
        ElectronIsEMLooseSelectorCutDefs.ElectronIsEMLooseSelectorConfig2012)
}


ElectronIsEMMapDC14 = {
    egammaPID.ElectronIDLoosePP: (
        egammaPID.ElectronLoosePP,
        ElectronIsEMLooseSelectorCutDefs.ElectronIsEMLooseSelectorConfigDC14),
    egammaPID.ElectronIDMediumPP: (
        egammaPID.ElectronMediumPP,
        ElectronIsEMMediumSelectorCutDefs.ElectronIsEMMediumSelectorConfigDC14),
    egammaPID.ElectronIDTightPP: (
        egammaPID.ElectronTightPP,
        ElectronIsEMTightSelectorCutDefs.ElectronIsEMTightSelectorConfigDC14),
    egammaPID.NoIDCut: (
        0,
        ElectronIsEMLooseSelectorCutDefs.ElectronIsEMLooseSelectorConfigDC14)
}

TrigElectronIsEMMap2012 = {
    egammaPID.ElectronIDLoose1: (
        egammaPID.ElectronLoose1,
        TrigElectronIsEMLoose1SelectorCutDefs.TrigElectronIsEMLoose1SelectorConfig2012),
    egammaPID.ElectronIDMedium1: (
        egammaPID.ElectronMedium1,
        TrigElectronIsEMMedium1SelectorCutDefs.TrigElectronIsEMMedium1SelectorConfig2012),
    egammaPID.ElectronIDTight1: (
        egammaPID.ElectronTight1,
        TrigElectronIsEMTight1SelectorCutDefs.TrigElectronIsEMTight1SelectorConfig2012),
    egammaPID.PhotonIDLooseEF: (
        egammaPID.PhotonLooseEF,
        TrigPhotonIsEMCaloCutsOnlySelectorCutDefs.TrigPhotonIsEMLooseCaloCutsOnlySelectorConfig2012),
    egammaPID.PhotonIDMediumEF: (
        egammaPID.PhotonMediumEF,
        TrigPhotonIsEMCaloCutsOnlySelectorCutDefs.TrigPhotonIsEMMediumCaloCutsOnlySelectorConfig2012),
}

TrigElectronIsEMMapDC14 = {
    egammaPID.ElectronIDLooseHLT:
    (egammaPID.ElectronLooseHLT,
     TrigElectronIsEMLooseSelectorCutDefs.TrigElectronIsEMLooseSelectorConfigDC14),
    egammaPID.ElectronIDMediumHLT:
    (egammaPID.ElectronMediumHLT,
     TrigElectronIsEMMediumSelectorCutDefs.TrigElectronIsEMMediumSelectorConfigDC14),
    egammaPID.ElectronIDTightHLT:
    (egammaPID.ElectronTightHLT,
     TrigElectronIsEMTightSelectorCutDefs.TrigElectronIsEMTightSelectorConfigDC14),
}


def ElectronIsEMMap(quality, menu):
    if menu == electronPIDmenu.menu2012:
        return ElectronIsEMMap2012[quality]
    elif menu == electronPIDmenu.menuDC14:
        return ElectronIsEMMapDC14[quality]
    elif menu == electronPIDmenu.menuTrig2012:
        return TrigElectronIsEMMap2012[quality]
    elif menu == electronPIDmenu.menuTrigDC14:
        return TrigElectronIsEMMapDC14[quality]
    else:
        raise ValueError("Requested menu is undefined: %d" % menu)
