# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

######################################################
# UserExtrapolator module
#
# it inherits from AtlasExtrapolator and allows configuring
# properties of the Extrapolator that are coupled to additions
# to the TrackingGeometry
#
######################################################

# import the Extrapolator configurable
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator

# define the class
class UserExtrapolator( AtlasExtrapolator ):
    # constructor
    def __init__(self,name = 'UserExtrapolator'):

       # call the base class constructor
       AtlasExtrapolator.__init__(self,name)
       self.thisIndex = -1
    def configureCaloMaterialEffects(self):
        #retrieve ToolSvc and find the TileVolumeBuilder there
        from __main__ import ToolSvc
        TheTileVolumeBuilder = ToolSvc.TileVolumeBuilder
        TheLArVolumeBuilder = ToolSvc.LArVolumeBuilder
        #if no MEOTP has been configured for this extrapolator, configure
        #it. Otherwise return the configured one
        from CaloMaterialEffectsOnTrackProvider.ConfCaloMaterialEffectsOnTrackProvider import ConfCaloMaterialEffectsOnTrackProvider as ConfMEOTP
        meotpName = self.getName()+'CaloMEOTP'
        MEOTP = ConfMEOTP(meotpName)
        if self.thisIndex is -1:
            ToolSvc += MEOTP

            meotps = TheTileVolumeBuilder.MaterialEffectsOnTrackProviders
            size = len(meotps)
            meotps += [ MEOTP ]
            TheTileVolumeBuilder.MaterialEffectsOnTrackProviders = meotps

            meotps2 = TheLArVolumeBuilder.MaterialEffectsOnTrackProviders
            size = len(meotps2)
            meotps2 += [ MEOTP ]
            TheLArVolumeBuilder.MaterialEffectsOnTrackProviders = meotps2

            self.MaterialEffectsOnTrackProviderIndex = size
            self.thisIndex = size

        return MEOTP
        
