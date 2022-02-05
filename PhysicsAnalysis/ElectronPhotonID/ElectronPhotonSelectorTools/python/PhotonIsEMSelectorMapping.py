# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# =============================================================================
#  Name:        PhotonIsEMSelectorMapping.py
##
# Author:      Tulay Cuhadar Donszelmann, Jovan Mitrevski
# Created:     Dec 2011
##
# Description: Find mapping of mask and function for ID quality
# =============================================================================

#
import ElectronPhotonSelectorTools.PhotonIsEMMenuDefs as PhotonIsEMMenuDefs
from ElectronPhotonSelectorTools.EgammaPIDdefs import egammaPID

#
# The "photonPIDmenu" used to store every menu in existence...
# now we will simply update the
# location of the conf file to the new path.
# Therefore, the CURRENT MENU is: menuCurrentCuts
# (corresponding to the PhotonIsEMMapCurrent dict)
# and the other menu that exists is the "menuPtInclJan2018"
# (the PhotonIsEMMapPtInclJan2018 dict)
# (because it will be used concurrently in the same tag of athena).
# Should remove menuPtInclJan2018 when it is no longer supported.
#


class photonPIDmenu:
    menuCurrentCuts = 0
    menuPtInclJan2018 = 1


# format - key: (mask, function)
PhotonIsEMMapCurrent = {
    egammaPID.PhotonIDLoose:  (
        egammaPID.PhotonLoose,
        PhotonIsEMMenuDefs.PhotonIsEMLooseSelectorConfig),
    egammaPID.PhotonIDMedium: (
        egammaPID.PhotonMedium,
        PhotonIsEMMenuDefs.PhotonIsEMMediumSelectorConfig),
    egammaPID.PhotonIDTight:  (
        egammaPID.PhotonTight,
        PhotonIsEMMenuDefs.PhotonIsEMTightSelectorConfig),

    egammaPID.PhotonIDLooseAR:  (
        egammaPID.PhotonLooseAR,
        PhotonIsEMMenuDefs.PhotonIsEMLooseSelectorConfig),
    egammaPID.PhotonIDMediumAR: (
        egammaPID.PhotonMediumAR,
        PhotonIsEMMenuDefs.PhotonIsEMMediumSelectorConfig),
    egammaPID.PhotonIDTightAR:  (
        egammaPID.PhotonTightAR,
        PhotonIsEMMenuDefs.PhotonIsEMTightSelectorConfig),
    egammaPID.NoIDCut: (
        0,
        PhotonIsEMMenuDefs.PhotonIsEMLooseSelectorConfig)
}

PhotonIsEMMapPtInclJan2018 = {
    egammaPID.PhotonIDTight:  (
        egammaPID.PhotonTight,
        PhotonIsEMMenuDefs.PhotonIsEMTightSelectorConfigPtInclJan2018),
}


def PhotonIsEMMap(quality, menu):
    # These are the "current menus" (non-legacy)
    if menu == photonPIDmenu.menuCurrentCuts and quality in PhotonIsEMMapCurrent.keys():
        return PhotonIsEMMapCurrent[quality]
    elif menu == photonPIDmenu.menuPtInclJan2018 and quality in PhotonIsEMMapPtInclJan2018.keys():
        return PhotonIsEMMapPtInclJan2018[quality]
    else:
        raise ValueError("Requested menu is undefined: %d" % menu)
