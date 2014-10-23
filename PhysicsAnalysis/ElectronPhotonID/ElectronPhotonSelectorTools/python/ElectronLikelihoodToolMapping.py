# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        ElectronLikelihoodToolMapping.py
##
## Author:      Tulay Cuhadar Donszelmann, Jovan Mitrevski, Kurt Brendlinger
## Created:     August 2013
##
## Description: Find mapping of mask and function for ID quality
##=============================================================================

import PyCintex
try :
    PyCintex.loadDictionary('ElectronPhotonSelectorToolsDict')
except :
    pass

from ROOT import LikeEnum

class electronLHmenu:
    menu2012 = 0
    customMenu = 1
    trigger2012 = 2
    trigger2015 = 3
    offline2015 = 4

import ElectronPhotonSelectorTools.ElectronLikelihoodMenuDefs as ElectronLikelihoodMenuDefs

# format - key: (mask, function)

ElectronLHMap2012 = {
    #
    # This menu is exactly like the medium operating point, but it employs the "custom" framework
    #
    LikeEnum.VeryTight: ( LikeEnum.VeryTight, ElectronLikelihoodMenuDefs.ElectronLikelihoodVeryTightConfig2012 ),
    LikeEnum.Tight: ( LikeEnum.Tight, ElectronLikelihoodMenuDefs.ElectronLikelihoodTightConfig2012 ),
    LikeEnum.Medium: ( LikeEnum.Medium, ElectronLikelihoodMenuDefs.ElectronLikelihoodMediumConfig2012 ),
    LikeEnum.Loose: ( LikeEnum.Loose, ElectronLikelihoodMenuDefs.ElectronLikelihoodLooseConfig2012 ),
    LikeEnum.VeryLoose: ( LikeEnum.VeryLoose, ElectronLikelihoodMenuDefs.ElectronLikelihoodVeryLooseConfig2012 ),
    LikeEnum.LooseRelaxed: ( LikeEnum.LooseRelaxed, ElectronLikelihoodMenuDefs.ElectronLikelihoodLooseRelaxedConfig2012 )
    }

ElectronLHMapCustom = {
    LikeEnum.Medium:( LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodCUSTOMMediumConfig )
    }

ElectronLHMapTrigger2012 = {
    LikeEnum.Medium:( LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodCUSTOMMediumNoGSFConfig )
}

ElectronLHMapTrigger2015 = {
    LikeEnum.Loose: ( LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodLooseTriggerConfig2015 ),
    LikeEnum.Medium: ( LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodMediumTriggerConfig2015 ),
    LikeEnum.Tight: ( LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodTightTriggerConfig2015 ),
    }

ElectronLHMapOffline2015 = {
    LikeEnum.Loose: ( LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodLooseOfflineConfig2015 ),
    LikeEnum.Medium: ( LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodMediumOfflineConfig2015 ),
    LikeEnum.Tight: ( LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodTightOfflineConfig2015 ),
    }

def ElectronLikelihoodMap(quality, menu):
    if menu == electronLHmenu.menu2012:
        return ElectronLHMap2012[quality]
    elif menu == electronLHmenu.customMenu:
        return ElectronLHMapCustom[quality]
    elif menu == electronLHmenu.trigger2012:
        return ElectronLHMapTrigger2012[quality]
    elif menu == electronLHmenu.trigger2015:
        return ElectronLHMapTrigger2015[quality]
    elif menu == electronLHmenu.offline2015:
        return ElectronLHMapOffline2015[quality]
    else:
        raise ValueError("Requested menu is undefined: %d" % menu)
