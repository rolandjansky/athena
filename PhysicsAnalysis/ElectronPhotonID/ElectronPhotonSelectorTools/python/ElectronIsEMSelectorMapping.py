# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        ElectronIsEMSelectorMapping.py
##
## Author:      Tulay Cuhadar Donszelmann, Jovan Mitrevski
## Created:     Dec 2011
##
## Description: Find mapping of mask and function for ID quality
##=============================================================================

import ROOT,cppyy

cppy.include("ElectronPhotonSelectorTools/egammaPIDdefs.h")

from ROOT import egammaPID

class electronPIDmenu:
    menu2011 = 0
    menu2012 = 1
    origMenu2012 = 2
    menuH4l2011 = 3
    menuTrig2012 = 4
    menuTrigDC14 = 5
    menuDC14 = 6

import ElectronPhotonSelectorTools.ElectronIsEMSelectorCutDefs as ElectronIsEMSelectorCutDefs
import ElectronPhotonSelectorTools.ElectronIsEMLooseSelectorCutDefs as ElectronIsEMLooseSelectorCutDefs
import ElectronPhotonSelectorTools.ElectronIsEMMediumSelectorCutDefs as ElectronIsEMMediumSelectorCutDefs
import ElectronPhotonSelectorTools.ElectronIsEMTightSelectorCutDefs as ElectronIsEMTightSelectorCutDefs
import ElectronPhotonSelectorTools.ElectronIsEMH4l2011SelectorCutDefs as ElectronIsEMH4l2011SelectorCutDefs

# Trigger menu
import ElectronPhotonSelectorTools.TrigElectronIsEMLoose1SelectorCutDefs as TrigElectronIsEMLoose1SelectorCutDefs 
import ElectronPhotonSelectorTools.TrigElectronIsEMMedium1SelectorCutDefs as TrigElectronIsEMMedium1SelectorCutDefs
import ElectronPhotonSelectorTools.TrigElectronIsEMTight1SelectorCutDefs as TrigElectronIsEMTight1SelectorCutDefs 
import ElectronPhotonSelectorTools.TrigElectronIsEMLooseSelectorCutDefs as TrigElectronIsEMLooseSelectorCutDefs 
import ElectronPhotonSelectorTools.TrigElectronIsEMMediumSelectorCutDefs as TrigElectronIsEMMediumSelectorCutDefs
import ElectronPhotonSelectorTools.TrigElectronIsEMTightSelectorCutDefs as TrigElectronIsEMTightSelectorCutDefs
import ElectronPhotonSelectorTools.TrigPhotonIsEMCaloCutsOnlySelectorCutDefs as TrigPhotonIsEMCaloCutsOnlySelectorCutDefs
# format - key: (mask, function)

ElectronIsEMMap2011 = {
    egammaPID.ElectronIDLoose:   (egammaPID.ElectronLoose, ElectronIsEMSelectorCutDefs.ElectronIsEMSelectorConfig2011 ),
    egammaPID.ElectronIDMedium: ( egammaPID.ElectronMedium, ElectronIsEMSelectorCutDefs.ElectronIsEMSelectorConfig2011 ),
    egammaPID.ElectronIDTight: ( egammaPID.ElectronTight, ElectronIsEMSelectorCutDefs.ElectronIsEMSelectorConfig2011 ),
    egammaPID.ElectronIDLoosePP: ( egammaPID.ElectronLoosePP, ElectronIsEMLooseSelectorCutDefs.ElectronIsEMLooseSelectorConfig2011 ),
    egammaPID.ElectronIDMediumPP: ( egammaPID.ElectronMediumPP, ElectronIsEMMediumSelectorCutDefs.ElectronIsEMMediumSelectorConfig2011 ),
    egammaPID.ElectronIDTightPP: ( egammaPID.ElectronTightPP, ElectronIsEMTightSelectorCutDefs.ElectronIsEMTightSelectorConfig2011 ),
    egammaPID.NoIDCut: ( 0, ElectronIsEMLooseSelectorCutDefs.ElectronIsEMLooseSelectorConfig2011 )
    }

ElectronIsEMMap2012 = {
    egammaPID.ElectronIDLoosePP: ( egammaPID.ElectronLoosePP, ElectronIsEMLooseSelectorCutDefs.ElectronIsEMLooseSelectorConfig2012 ),
    egammaPID.ElectronIDMediumPP: ( egammaPID.ElectronMediumPP, ElectronIsEMMediumSelectorCutDefs.ElectronIsEMMediumSelectorConfig2012 ),
    egammaPID.ElectronIDTightPP: ( egammaPID.ElectronTightPP, ElectronIsEMTightSelectorCutDefs.ElectronIsEMTightSelectorConfig2012 ),
    egammaPID.NoIDCut: ( 0, ElectronIsEMLooseSelectorCutDefs.ElectronIsEMLooseSelectorConfig2012 )
    }

ElectronIsEMMap2012OLD = {
    egammaPID.ElectronIDLoosePP: ( egammaPID.ElectronLoosePP, ElectronIsEMLooseSelectorCutDefs.ElectronIsEMLooseSelectorConfig2012OLD ),
    egammaPID.ElectronIDMediumPP: ( egammaPID.ElectronMediumPP, ElectronIsEMMediumSelectorCutDefs.ElectronIsEMMediumSelectorConfig2012OLD ),
    egammaPID.ElectronIDTightPP: ( egammaPID.ElectronTightPP, ElectronIsEMTightSelectorCutDefs.ElectronIsEMTightSelectorConfig2012OLD ),    
    egammaPID.NoIDCut: ( 0, ElectronIsEMLooseSelectorCutDefs.ElectronIsEMLooseSelectorConfig2012OLD )
    }

ElectronIsEMMapDC14 = {
    egammaPID.ElectronIDLoosePP: ( egammaPID.ElectronLoosePP, ElectronIsEMLooseSelectorCutDefs.ElectronIsEMLooseSelectorConfigDC14 ),
    egammaPID.ElectronIDMediumPP: ( egammaPID.ElectronMediumPP, ElectronIsEMMediumSelectorCutDefs.ElectronIsEMMediumSelectorConfigDC14 ),
    egammaPID.ElectronIDTightPP: ( egammaPID.ElectronTightPP, ElectronIsEMTightSelectorCutDefs.ElectronIsEMTightSelectorConfigDC14 ),
    egammaPID.NoIDCut: ( 0, ElectronIsEMLooseSelectorCutDefs.ElectronIsEMLooseSelectorConfigDC14 )
    }

TrigElectronIsEMMap2012 = {
    egammaPID.ElectronIDLoose1: ( egammaPID.ElectronLoose1, TrigElectronIsEMLoose1SelectorCutDefs.TrigElectronIsEMLoose1SelectorConfig2012 ),
    egammaPID.ElectronIDMedium1: ( egammaPID.ElectronMedium1, TrigElectronIsEMMedium1SelectorCutDefs.TrigElectronIsEMMedium1SelectorConfig2012 ),
    egammaPID.ElectronIDTight1: ( egammaPID.ElectronTight1, TrigElectronIsEMTight1SelectorCutDefs.TrigElectronIsEMTight1SelectorConfig2012 ),
    egammaPID.PhotonIDLooseEF: ( egammaPID.PhotonLooseEF, TrigPhotonIsEMCaloCutsOnlySelectorCutDefs.TrigPhotonIsEMLooseCaloCutsOnlySelectorConfig2012),
    egammaPID.PhotonIDMediumEF: ( egammaPID.PhotonMediumEF, TrigPhotonIsEMCaloCutsOnlySelectorCutDefs.TrigPhotonIsEMMediumCaloCutsOnlySelectorConfig2012),
    }

TrigElectronIsEMMapDC14 = {
    egammaPID.ElectronIDLooseHLT: ( egammaPID.ElectronLooseHLT, TrigElectronIsEMLooseSelectorCutDefs.TrigElectronIsEMLooseSelectorConfigDC14 ),
    egammaPID.ElectronIDMediumHLT: ( egammaPID.ElectronMediumHLT, TrigElectronIsEMMediumSelectorCutDefs.TrigElectronIsEMMediumSelectorConfigDC14 ),
    egammaPID.ElectronIDTightHLT: ( egammaPID.ElectronTightHLT, TrigElectronIsEMTightSelectorCutDefs.TrigElectronIsEMTightSelectorConfigDC14 ),
    }

def ElectronIsEMMap(quality, menu):
    if menu == electronPIDmenu.menu2011:
        return ElectronIsEMMap2011[quality]
    elif menu == electronPIDmenu.menu2012:
        return ElectronIsEMMap2012[quality]
    elif menu == electronPIDmenu.origMenu2012:
        return ElectronIsEMMap2012OLD[quality]
    elif menu == electronPIDmenu.menuDC14:
        return ElectronIsEMMapDC14[quality]
    elif menu == electronPIDmenu.menuTrig2012:
        return TrigElectronIsEMMap2012[quality]
    elif menu == electronPIDmenu.menuTrigDC14:
        return TrigElectronIsEMMapDC14[quality]    
    elif menu == electronPIDmenu.menuH4l2011:
        tuple =(egammaPID.ElectronLoosePP,ElectronIsEMH4l2011SelectorCutDefs.ElectronIsEMH4l2011SelectorConfig)
        return tuple
    else:
        raise ValueError("Requested menu is undefined: %d" % menu)

