# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        PhotonIsEMSelectorMapping.py
##
## Author:      Tulay Cuhadar Donszelmann, Jovan Mitrevski
## Created:     Dec 2011
##
## Description: Find mapping of mask and function for ID quality
##=============================================================================

import PyCintex
PyCintex.loadDictionary('ElectronPhotonSelectorToolsDict')

from ROOT import egammaPID

class photonPIDmenu:
    menu2011 = 0
    menu2012 = 1
    origMenu2012 = 2 # this is the original menu not meant for anyone to use

import ElectronPhotonSelectorTools.PhotonIsEMSelectorCutDefs as PhotonIsEMSelectorCutDefs
import ElectronPhotonSelectorTools.PhotonIsEMMediumSelectorCutDefs as PhotonIsEMMediumSelectorCutDefs

# format - key: (mask, function)

PhotonIsEMMap2011 = {
    egammaPID.PhotonIDLoose: ( egammaPID.PhotonLoose, PhotonIsEMSelectorCutDefs.PhotonIsEMSelectorConfig2011 ),
    egammaPID.PhotonIDTight: ( egammaPID.PhotonTight, PhotonIsEMSelectorCutDefs.PhotonIsEMSelectorConfig2011 ),

    egammaPID.PhotonIDLooseAR: ( egammaPID.PhotonLooseAR, PhotonIsEMSelectorCutDefs.PhotonIsEMSelectorConfig2011 ),
    egammaPID.PhotonIDTightAR: ( egammaPID.PhotonTightAR, PhotonIsEMSelectorCutDefs.PhotonIsEMSelectorConfig2011 ),
    
    egammaPID.NoIDCut: ( 0, PhotonIsEMSelectorCutDefs.PhotonIsEMSelectorConfig2012 )
    }

PhotonIsEMMap2012 = {
    egammaPID.PhotonIDLoose: ( egammaPID.PhotonLoose, PhotonIsEMSelectorCutDefs.PhotonIsEMSelectorConfig2012 ),
    egammaPID.PhotonIDMedium: ( egammaPID.PhotonMedium, PhotonIsEMMediumSelectorCutDefs.PhotonIsEMMediumSelectorConfig2012 ),
    egammaPID.PhotonIDTight: ( egammaPID.PhotonTight, PhotonIsEMSelectorCutDefs.PhotonIsEMSelectorConfig2012 ),

    egammaPID.PhotonIDLooseAR: ( egammaPID.PhotonLooseAR, PhotonIsEMSelectorCutDefs.PhotonIsEMSelectorConfig2012 ),
    egammaPID.PhotonIDMediumAR: ( egammaPID.PhotonMediumAR, PhotonIsEMMediumSelectorCutDefs.PhotonIsEMMediumSelectorConfig2012 ),
    egammaPID.PhotonIDTightAR: ( egammaPID.PhotonTightAR, PhotonIsEMSelectorCutDefs.PhotonIsEMSelectorConfig2012 ),
    
    egammaPID.NoIDCut: ( 0, PhotonIsEMSelectorCutDefs.PhotonIsEMSelectorConfig2012 )
    }

PhotonIsEMMap2012OLD = {
    egammaPID.PhotonIDLoose: ( egammaPID.PhotonLoose, PhotonIsEMSelectorCutDefs.PhotonIsEMSelectorConfig2012OLD ),
    egammaPID.PhotonIDMedium: ( egammaPID.PhotonMedium, PhotonIsEMMediumSelectorCutDefs.PhotonIsEMMediumSelectorConfig2012 ), # unchanged
    egammaPID.PhotonIDTight: ( egammaPID.PhotonTight, PhotonIsEMSelectorCutDefs.PhotonIsEMSelectorConfig2012OLD ),

    egammaPID.PhotonIDLooseAR: ( egammaPID.PhotonLooseAR, PhotonIsEMSelectorCutDefs.PhotonIsEMSelectorConfig2012OLD ),
    egammaPID.PhotonIDMediumAR: ( egammaPID.PhotonMediumAR, PhotonIsEMMediumSelectorCutDefs.PhotonIsEMMediumSelectorConfig2012 ),
    egammaPID.PhotonIDTightAR: ( egammaPID.PhotonTightAR, PhotonIsEMSelectorCutDefs.PhotonIsEMSelectorConfig2012OLD ),
    
    egammaPID.NoIDCut: ( 0, PhotonIsEMSelectorCutDefs.PhotonIsEMSelectorConfig2012OLD )
    }


def PhotonIsEMMap(quality, menu):
    if menu == photonPIDmenu.menu2011:
        return PhotonIsEMMap2011[quality]
    elif menu == photonPIDmenu.menu2012:
        return PhotonIsEMMap2012[quality]
    elif menu == photonPIDmenu.origMenu2012:
        return PhotonIsEMMap2012OLD[quality]
    else:
        raise ValueError("Requested menu is undefined: %d" % menu)
