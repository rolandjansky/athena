# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        TrigEgammaElectronIsEMSelectorMapping.py
##
## Author:      Ryan Mackenzie White 
## Created:     June 2013
## Modified:    June 2014 -- updating with 2014 tunes
## Description: Find mapping of mask and function for ID quality
##=============================================================================

#include TrigEGammaPIDdefs for IsEM definitions
from TrigEGammaPIDdefs import SelectionDefElectron
from TrigEGammaPIDdefs import SelectionDefPhoton
from TrigEGammaPIDdefs import TrigEgammaIDQuality
from TrigEGammaPIDdefsDC14 import TrigEgammaIDQualityDC14
from TrigEGammaPIDdefsDC14 import SelectionDefElectronDC14 

#import TriggerFlags
from TriggerJobOpts.TriggerFlags import TriggerFlags
menu_name = TriggerFlags.triggerMenuSetup()

class electronPIDmenu:
    menuTrig2011 = 0
    menuTrig2012 = 1
    menuTrigDC14 = 2

import TrigEgammaRec.TrigEgammaElectronIsEMCutDefs as TrigEgammaElectronIsEMCutDefs
import TrigEgammaRec.TrigEgammaElectronIsEMCutDefs_loose1 as TrigEgammaElectronIsEMCutDefs_loose1
import TrigEgammaRec.TrigEgammaElectronIsEMCutDefs_medium1 as TrigEgammaElectronIsEMCutDefs_medium1
import TrigEgammaRec.TrigEgammaElectronIsEMCutDefs_medium2 as TrigEgammaElectronIsEMCutDefs_medium2
import TrigEgammaRec.TrigEgammaElectronIsEMCutDefs_tight1 as TrigEgammaElectronIsEMCutDefs_tight1
import TrigEgammaRec.TrigEgammaElectronIsEMCutDefs_CaloCutsOnly as TrigEgammaElectronIsEMCutDefs_CaloCutsOnly

import TrigEgammaRec.TrigEgammaElectronIsEMCutDefs_loose as TrigEgammaElectronIsEMCutDefs_loose
import TrigEgammaRec.TrigEgammaElectronIsEMCutDefs_medium as TrigEgammaElectronIsEMCutDefs_medium
import TrigEgammaRec.TrigEgammaElectronIsEMCutDefs_tight as TrigEgammaElectronIsEMCutDefs_tight
# format - key: (mask, function)

TrigEgammaElectronIsEM2011 = {
        TrigEgammaIDQuality.ElectronIDLoose1: (SelectionDefElectron.ElectronLoose1, TrigEgammaElectronIsEMCutDefs_loose1.TrigEgammaElectronIsEMSelectorConfig2011_loose1),
        TrigEgammaIDQuality.ElectronIDMedium1: (SelectionDefElectron.ElectronMedium1, TrigEgammaElectronIsEMCutDefs_medium1.TrigEgammaElectronIsEMSelectorConfig2011_medium1),
        TrigEgammaIDQuality.PhotonIDLooseEF: (SelectionDefPhoton.PhotonLooseEF, TrigEgammaElectronIsEMCutDefs_CaloCutsOnly.TrigEgammaElectronIsEMSelectorConfig2011_CaloCutsOnly)
        }

TrigEgammaElectronIsEM2012 = {
        TrigEgammaIDQuality.ElectronIDLoose: (SelectionDefElectron.ElectronLoose, TrigEgammaElectronIsEMCutDefs.TrigEgammaElectronIsEMSelectorConfig2012),
        TrigEgammaIDQuality.ElectronIDLoose0: (SelectionDefElectron.ElectronLoose1, TrigEgammaElectronIsEMCutDefs_loose1.TrigEgammaElectronIsEMSelectorConfig2012_looser1),
        TrigEgammaIDQuality.ElectronIDLoose1: (SelectionDefElectron.ElectronLoose1, TrigEgammaElectronIsEMCutDefs_loose1.TrigEgammaElectronIsEMSelectorConfig2012_loose1),
        TrigEgammaIDQuality.ElectronIDMedium1: (SelectionDefElectron.ElectronMedium1, TrigEgammaElectronIsEMCutDefs_medium1.TrigEgammaElectronIsEMSelectorConfig2012_medium1),
        TrigEgammaIDQuality.ElectronIDMedium2: (SelectionDefElectron.ElectronMedium2, TrigEgammaElectronIsEMCutDefs_medium2.TrigEgammaElectronIsEMSelectorConfig2012_medium2),
        TrigEgammaIDQuality.ElectronIDTight1: (SelectionDefElectron.ElectronTight1, TrigEgammaElectronIsEMCutDefs_tight1.TrigEgammaElectronIsEMSelectorConfig2012_tight1),
        TrigEgammaIDQuality.PhotonIDLooseEF: (SelectionDefPhoton.PhotonLooseEF, TrigEgammaElectronIsEMCutDefs_CaloCutsOnly.TrigEgammaElectronIsEMSelectorConfig2012_CaloCutsOnly),
        TrigEgammaIDQuality.PhotonIDLooseEFTauMass: (SelectionDefPhoton.PhotonLooseEF, TrigEgammaElectronIsEMCutDefs_CaloCutsOnly.TrigEgammaElectronIsEMSelectorConfig2012_CaloCutsOnly_TauMass),
        TrigEgammaIDQuality.PhotonIDMediumEF: (SelectionDefPhoton.PhotonMediumEF, TrigEgammaElectronIsEMCutDefs_CaloCutsOnly.TrigEgammaElectronIsEMSelectorConfig2012_CaloCutsOnly_medium)
        }

TrigEgammaElectronIsEMDC14 = {
        TrigEgammaIDQualityDC14.ElectronIDLoose: (SelectionDefElectronDC14.ElectronLoose, TrigEgammaElectronIsEMCutDefs_loose.TrigEgammaElectronIsEMSelectorConfigDC14_loose),
        TrigEgammaIDQualityDC14.ElectronIDMedium: (SelectionDefElectronDC14.ElectronMedium, TrigEgammaElectronIsEMCutDefs_medium.TrigEgammaElectronIsEMSelectorConfigDC14_medium),
        TrigEgammaIDQualityDC14.ElectronIDTight: (SelectionDefElectronDC14.ElectronTight, TrigEgammaElectronIsEMCutDefs_tight.TrigEgammaElectronIsEMSelectorConfigDC14_tight),
        }


def TrigEgammaElectronIsEMMap(quality, menu):
    if menu == electronPIDmenu.menuTrig2011:
        return TrigEgammaElectronIsEM2011[quality]
    elif menu == electronPIDmenu.menuTrig2012:
        return TrigEgammaElectronIsEM2012[quality]
    elif menu == electronPIDmenu.menuTrigDC14:
        return TrigEgammaElectronIsEMDC14[quality]
    else:
        raise ValueError("Requested menu is undefined: %d" % menu)
