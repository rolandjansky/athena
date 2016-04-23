# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        ElectronLikelihoodToolMapping.py
##
## Author:      Tulay Cuhadar Donszelmann, Jovan Mitrevski, Kurt Brendlinger
## Created:     August 2013
##
## Description: Find mapping of mask and function for ID quality
##=============================================================================

import cppyy
try :
    cppyy.loadDictionary('ElectronPhotonSelectorToolsDict')
except :
    pass

from ROOT import LikeEnum

class electronLHmenu:
    customMenu = 0
    trigger2012 = 1
    trigger2015 = 2
    offline2015 = 3

import ElectronPhotonSelectorTools.ElectronLikelihoodMenuDefs as ElectronLikelihoodMenuDefs

# format - key: (mask, function)
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
    LikeEnum.VeryLoose: ( LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodVeryLooseOfflineConfig2015 ),
    LikeEnum.Loose: ( LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodLooseOfflineConfig2015 ),
    LikeEnum.Medium: ( LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodMediumOfflineConfig2015 ),
    LikeEnum.Tight: ( LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodTightOfflineConfig2015 ),
    }

def ElectronLikelihoodMap(quality, menu):
    if menu == electronLHmenu.customMenu:
        return ElectronLHMapCustom[quality]
    elif menu == electronLHmenu.trigger2012:
        return ElectronLHMapTrigger2012[quality]
    elif menu == electronLHmenu.trigger2015:
        return ElectronLHMapTrigger2015[quality]
    elif menu == electronLHmenu.offline2015:
        return ElectronLHMapOffline2015[quality]
    else:
        raise ValueError("Requested menu is undefined: %d" % menu)
