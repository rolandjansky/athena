# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# =============================================================================
# Name:        ElectronLikelihoodToolMapping.py
#
# Author:      Tulay Cuhadar Donszelmann, Jovan Mitrevski, Kurt Brendlinger
# Created:     August 2013
#
# Description: Find mapping of mask and function for ID quality
# =============================================================================


from ElectronPhotonSelectorTools.LikelihoodEnums import LikeEnum
import ElectronPhotonSelectorTools.ElectronLikelihoodMenuDefs as ElectronLikelihoodMenuDefs


class electronLHmenu:
    trigger2015 = 1
    offlineMC16 = 3


# format - key: (mask, function)

ElectronLHMapTrigger2015 = {
    LikeEnum.Loose: (LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodLooseTriggerConfig2015),
    LikeEnum.Medium: (LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodMediumTriggerConfig2015),
    LikeEnum.Tight: (LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodTightTriggerConfig2015),
}

ElectronLHMapOffline2016 = {
    LikeEnum.VeryLoose: (LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodVeryLooseOfflineConfig2016),
    LikeEnum.Loose: (LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodLooseOfflineConfig2016),
    LikeEnum.Medium: (LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodMediumOfflineConfig2016),
    LikeEnum.Tight: (LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodTightOfflineConfig2016),
}


def ElectronLikelihoodMap(quality, menu):
    if menu == electronLHmenu.trigger2015:
        return ElectronLHMapTrigger2015[quality]
    elif menu == electronLHmenu.offlineMC16:
        return ElectronLHMapOffline2016[quality]
    else:
        raise ValueError("Requested menu is undefined: %d" % menu)
