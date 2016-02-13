""" SLHC_Setup
    Python module to hold storegate keys of InDet objects.
"""

__author__ =   "A. Salzburger"
__version__=   "$Revision: 1.13 $"
__doc__    =   "SLHC_PathSetting"
__all__    = [ "SLHC_PathSetting" ]


from AthenaCommon.AppMgr import ToolSvc

        
# switch the material loading off
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.MaterialSource           = 'None'
TrkDetFlags.MaterialVersion          = 16

#TrkDetFlags.ConfigurationOutputLevel     = VERBOSE #INFO #VERBOSE
#TrkDetFlags.PixelBuildingOutputLevel     = VERBOSE #INFO #VERBOSE
#TrkDetFlags.SCT_BuildingOutputLevel      = VERBOSE #INFO #VERBOSE
#TrkDetFlags.InDetBuildingOutputLevel     = VERBOSE #INFO #VERBOSE
#TrkDetFlags.ConfigurationOutputLevel     = VERBOSE #INFO #VERBOSE

TrkDetFlags.SLHC_Geometry = True
TrkDetFlags.XMLFastCustomGeometry = True
TrkDetFlags.InDetTrackingGeometryBuilderName = 'InDetTrackingGeometryBuilder'


# init FastGeoModel geometry builder
from InDetTrackingGeometryXML.ConfiguredSLHC_InDetTrackingGeometryXMLBuilder import ConfiguredSLHC_InDetTrackingGeometryXMLBuilder
GeometryBuilder = ConfiguredSLHC_InDetTrackingGeometryXMLBuilder(name='InDetTrackingGeometryBuilder')
ToolSvc += GeometryBuilder

# load the tracking geometry service
from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
