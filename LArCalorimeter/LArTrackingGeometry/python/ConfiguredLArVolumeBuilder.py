# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

######################################################
# ConfiguredLArVolumeBuilder module
#
# it inherits from LArVolumeBuilder and performs 
# standard configuration
#
######################################################

# import the Extrapolator configurable
from LArTrackingGeometry.LArTrackingGeometryConf import LAr__LArVolumeBuilder

# define the class
class ConfiguredLArVolumeBuilder( LAr__LArVolumeBuilder ):
    # constructor
    def __init__(self,name = 'LArVolumeBuilder'):

        #from CaloMaterialEffectsOnTrackProvider.ConfCaloMaterialEffectsOnTrackProvider import ConfCaloMaterialEffectsOnTrackProvider as ConfMEOTP
        
        from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags

        # import the ToolSvc if necessary
        from AthenaCommon.AppMgr import ToolSvc
        if 'ToolSvc' not in dir():
          ToolSvc = ToolSvc()
        
        meotps = []
        #if TrkDetFlags.LArUseMaterialEffectsOnTrackProvider() :

          #from CaloMaterialEffectsOnTrackProvider.ConfCaloMaterialEffectsOnTrackProvider import ConfCaloMaterialEffectsOnTrackProvider as ConfMEOTP
        
          #MEOTP = ConfMEOTP('CaloMaterialEffectsOnTrackProvider')
          #ToolSvc += MEOTP
          #meotps += [ MEOTP ]

        # The volume helper
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__TrackingVolumeHelper
        LArTrackingVolumeHelper = Trk__TrackingVolumeHelper(name='TrackingVolumeHelper')
        ToolSvc += LArTrackingVolumeHelper 
          
        LAr__LArVolumeBuilder.__init__(self,
                                       name,
                                       MaterialEffectsOnTrackProviders = meotps,
                                       UseCaloSurfBuilder = TrkDetFlags.LArUseCaloSurfBuilder(),
                                       TrackingVolumeHelper = LArTrackingVolumeHelper,
                                       BarrelEnvelopeCover  = TrkDetFlags.LArBarrelEnvelopeCover(),
                                       EndcapEnvelopeCover  = TrkDetFlags.LArEndcapEnvelopeCover(),
                                       BarrelMaterialBinsRz = TrkDetFlags.LArBarrelLayerMaterialBinsRz(),   
                                       BarrelMaterialBinsPhi = TrkDetFlags.LArBarrelLayerMaterialBinsPhi(),   
                                       EndcapMaterialBinsRz = TrkDetFlags.LArEndcapLayerMaterialBinsRz(),    
                                       EndcapMaterialBinsPhi = TrkDetFlags.LArEndcapLayerMaterialBinsPhi(),   
                                       EndcapHecMaterialBinsRz = TrkDetFlags.LArEndcapHecLayerMaterialBinsRz(), 
                                       EndcapHecMaterialBinsPhi = TrkDetFlags.LArEndcapHecLayerMaterialBinsPhi(),
                                       EndcapFcalMaterialBinsRz = TrkDetFlags.LArEndcapFcalLayerMaterialBinsRz(),
                                       EndcapFcalMaterialBinsPhi = TrkDetFlags.LArEndcapFcalLayerMaterialBinsPhi(),
                                       EndcapGapMaterialBinsRz = TrkDetFlags.LArEndcapGapLayerMaterialBinsRz(), 
                                       EndcapGapMaterialBinsPhi = TrkDetFlags.LArEndcapGapLayerMaterialBinsPhi(),
                                       OutputLevel = TrkDetFlags.LArBuildingOutputLevel(),
                                       MagneticFieldMode = TrkDetFlags.MagneticFieldMode())
