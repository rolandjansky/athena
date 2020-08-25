# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        PhotonIsEMSelectorMapping.py
##
## Author:      Tulay Cuhadar Donszelmann, Jovan Mitrevski
## Created:     Dec 2011
##
## Description: Find mapping of mask and function for ID quality
##=============================================================================

import cppyy
try :
    cppyy.loadDictionary('ElectronPhotonSelectorToolsDict')
except :
    pass

from ROOT import egammaPID

#
# The "photonPIDmenu" used to store every menu in existence... now we will simply update the
# location of the conf file to the new path.
# Therefore, the CURRENT MENU is: menuCurrentCuts (corresponding to the PhotonIsEMMapCurrent dict)
# and the other menu that exists is the "menuPtInclJan2018" (the PhotonIsEMMapPtInclJan2018 dict)
# (because it will be used concurrently in the same tag of athena).
# Should remove menuPtInclJan2018 when it is no longer supported.
#
class photonPIDmenu:
    # menu2011 = 0
    # menu2012 = 1
    # origMenu2012 = 2 # this is the original menu not meant for anyone to use
    # menuDC14 = 3
    # menuTrigDC14 = 4
    menuCurrentCuts = 5
    menuPtInclJan2018 = 6

import ElectronPhotonSelectorTools.PhotonIsEMSelectorCutDefs as PhotonIsEMSelectorCutDefs
import ElectronPhotonSelectorTools.PhotonIsEMLooseSelectorCutDefs as PhotonIsEMLooseSelectorCutDefs
import ElectronPhotonSelectorTools.PhotonIsEMMediumSelectorCutDefs as PhotonIsEMMediumSelectorCutDefs
import ElectronPhotonSelectorTools.PhotonIsEMTightSelectorCutDefs as PhotonIsEMTightSelectorCutDefs

import ElectronPhotonSelectorTools.TrigPhotonIsEMCaloCutsOnlySelectorCutDefs as TrigPhotonIsEMCaloCutsOnlySelectorCutDefs

# format - key: (mask, function)

PhotonIsEMMapCurrent = {
    egammaPID.PhotonIDLoose:  ( egammaPID.PhotonLoose, PhotonIsEMLooseSelectorCutDefs.PhotonIsEMLooseSelectorConfig    ),
    egammaPID.PhotonIDMedium: ( egammaPID.PhotonMedium, PhotonIsEMMediumSelectorCutDefs.PhotonIsEMMediumSelectorConfig ),
    egammaPID.PhotonIDTight:  ( egammaPID.PhotonTight, PhotonIsEMTightSelectorCutDefs.PhotonIsEMTightSelectorConfig    ),

    egammaPID.PhotonIDLooseAR:  ( egammaPID.PhotonLooseAR, PhotonIsEMLooseSelectorCutDefs.PhotonIsEMLooseSelectorConfig    ),
    egammaPID.PhotonIDMediumAR: ( egammaPID.PhotonMediumAR, PhotonIsEMMediumSelectorCutDefs.PhotonIsEMMediumSelectorConfig ),
    egammaPID.PhotonIDTightAR:  ( egammaPID.PhotonTightAR, PhotonIsEMTightSelectorCutDefs.PhotonIsEMTightSelectorConfig    ),
    
    egammaPID.NoIDCut: ( 0, PhotonIsEMLooseSelectorCutDefs.PhotonIsEMLooseSelectorConfig )
    }

PhotonIsEMMapPtInclJan2018 = {
    egammaPID.PhotonIDTight:  ( egammaPID.PhotonTight , PhotonIsEMTightSelectorCutDefs.PhotonIsEMTightSelectorConfigPtInclJan2018 ),
    }


def PhotonIsEMMap(quality, menu):
    # These are the "current menus" (non-legacy)
    if menu == photonPIDmenu.menuCurrentCuts and quality in PhotonIsEMMapCurrent.keys() :
        return PhotonIsEMMapCurrent[quality]
    elif menu == photonPIDmenu.menuPtInclJan2018 and quality in PhotonIsEMMapPtInclJan2018.keys() :
        return PhotonIsEMMapPtInclJan2018[quality]
    else:
        raise ValueError("Requested menu is undefined: %d" % menu)
