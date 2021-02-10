# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

######################################################
# ConfiguredHGTDTrackingGeometryBuilder module
#
# it inherits from HGTD_TrackingGeometryBuilder and performs 
# standard configuration
#
######################################################

# import the include statement
from AthenaCommon.Include import Include, IncludeError, include

# import the Extrapolator configurable
from HGTD_TrackingGeometry.HGTD_TrackingGeometryConf import HGTDet__HGTD_TrackingGeometryBuilder

# define the class
class ConfiguredHGTDTrackingGeometryBuilder( HGTDet__HGTD_TrackingGeometryBuilder ):
    # constructor
    def __init__(self,name = 'HGTDTrackingGeometryBuilder'):
      from AthenaCommon.AppMgr import ToolSvc
      from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags      
      from HGTD_TrackingGeometry.HGTD_TrackingGeometryConf import HGTDet__HGTD_LayerBuilder
      # HGTD building
      HGTD_LayerBuilder = HGTDet__HGTD_LayerBuilder(name='HGTD_LayerBuilder')
      HGTD_LayerBuilder.Identification                  = 'HGTD'
      HGTD_LayerBuilder.SetLayerAssociation           = True        
      # output level                                 
      #HGTD_LayerBuilder.OutputLevel                   = TrkDetFlags.InDetBuildingOutputLevel() 
      # the binning type of the layer     
      HGTD_LayerBinning = 2
      # HGTD -> ToolSvc                             
      ToolSvc += HGTD_LayerBuilder                       
      
      HGTDCylinderVolumeCreator = ToolSvc.InDetCylinderVolumeCreator
      
      HGTDet__HGTD_TrackingGeometryBuilder.__init__(self,name,
                                                    LayerBuilder = HGTD_LayerBuilder,
                                                    TrackingVolumeCreator=ToolSvc.InDetCylinderVolumeCreator)
