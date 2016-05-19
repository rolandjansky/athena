""" SLHC_Setup_ITk_TrackingGeometry
    Python module to configure the TrackingGeometry for FastGeoModel implementation
    
"""

from AthenaCommon.AppMgr import ToolSvc
        
# switch the material loading off
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.XMLFastCustomGeometry = True
TrkDetFlags.InDetTrackingGeometryBuilderName = 'InDetTrackingGeometryBuilder'

# init FastGeoModel geometry builder
from InDetTrackingGeometryXML.ConfiguredSLHC_InDetTrackingGeometryXMLBuilder import ConfiguredSLHC_InDetTrackingGeometryXMLBuilder
GeometryBuilder = ConfiguredSLHC_InDetTrackingGeometryXMLBuilder(name='InDetTrackingGeometryBuilder')
ToolSvc += GeometryBuilder

# load the tracking geometry service
from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
