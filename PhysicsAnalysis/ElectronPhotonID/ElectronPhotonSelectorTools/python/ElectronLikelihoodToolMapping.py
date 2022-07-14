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
    offlineMC20 = 5
    offlineMC21 = 6


# format - key: (mask, function)

ElectronLHMapTrigger2015 = {
    LikeEnum.Loose: (LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodLooseTriggerConfig2015),
    LikeEnum.Medium: (LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodMediumTriggerConfig2015),
    LikeEnum.Tight: (LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodTightTriggerConfig2015),
}

ElectronLHMapOfflineMC20 = {
    LikeEnum.VeryLoose: (LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodVeryLooseOfflineConfigMC20),
    LikeEnum.Loose: (LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodLooseOfflineConfigMC20),
    LikeEnum.LooseBL: (LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodLooseBLOfflineConfigMC20),
    LikeEnum.Medium: (LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodMediumOfflineConfigMC20),
    LikeEnum.Tight: (LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodTightOfflineConfigMC20),
}

ElectronLHMapOfflineMC21 = {
    LikeEnum.VeryLoose: (LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodVeryLooseOfflineConfigMC21),
    LikeEnum.Loose: (LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodLooseOfflineConfigMC21),
    LikeEnum.LooseBL: (LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodLooseBLOfflineConfigMC21),
    LikeEnum.Medium: (LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodMediumOfflineConfigMC21),
    LikeEnum.Tight: (LikeEnum.CustomOperatingPoint, ElectronLikelihoodMenuDefs.ElectronLikelihoodTightOfflineConfigMC21),
}

def ElectronLikelihoodMap(quality, menu):
    if menu == electronLHmenu.trigger2015:
        return ElectronLHMapTrigger2015[quality]
    elif menu == electronLHmenu.offlineMC20:
        return ElectronLHMapOfflineMC20[quality]
    elif menu == electronLHmenu.offlineMC21:
        return ElectronLHMapOfflineMC21[quality]
    else:
        raise ValueError("Requested menu is undefined: %d" % menu)
