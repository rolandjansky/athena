#########################################################
#
# Initialization of the CustomTrackingGeometry
#
#########################################################

print "Starting CustomTrackingGeometry Initialization..."

###### ISF_Fatras XML flags
from ISF_FatrasDetDescrAlpine.CustomInDetTrackingGeometryAlpineProperties import GeometryAlpineLayoutFlags
GeometryAlpineLayoutFlags.PixelLayout.set_Value( "ATLAS" )
GeometryAlpineLayoutFlags.PixelEndcapLayout.set_Value( "ATLAS" )
GeometryAlpineLayoutFlags.SCTLayout.set_Value( "ATLAS" )
GeometryAlpineLayoutFlags.doSCT.set_Value( True )
GeometryAlpineLayoutFlags.IdDictFilename.set_Value("InDetIdDictFiles/IdDictInnerDetector_SLHC-24-24-dev4.xml")

from ISF_FatrasDetDescrAlpine.SetupCustomTrackingGeometryBuilder import SetupCustomTrackingGeometry
CustomTrackingGeometry =  SetupCustomTrackingGeometry(name                = 'CustomTrackingGeometry',
                                                      geometryName        = 'CustomGeometry',
                                                      prefix              = '',
                                                      PixelType           = GeometryAlpineLayoutFlags.PixelLayout.get_Value(),
                                                      PixelEndcapType     = GeometryAlpineLayoutFlags.PixelEndcapLayout.get_Value(),
                                                      SCT_Type            = GeometryAlpineLayoutFlags.SCTLayout.get_Value(),
                                                      doSplitBarrelLayers = GeometryAlpineLayoutFlags.splitBarrelLayers.get_Value(),
                                                      doRingLayout        = GeometryAlpineLayoutFlags.isRingLayout.get_Value(), 
                                                      doPixel             = GeometryAlpineLayoutFlags.doPix.get_Value(),
                                                      do_SCT              = GeometryAlpineLayoutFlags.doSCT.get_Value(),
                                                      dictionaryName      = GeometryAlpineLayoutFlags.IdDictFilename.get_Value(),
                                                      writeXML            = True)

from AthenaCommon.AppMgr import ToolSvc 
ToolSvc += CustomTrackingGeometry.geometryBuilder()

###### Setup ITk XML reader ######
from InDetTrackingGeometryXML.InDetTrackingGeometryXMLConf import InDet__XMLReaderSvc
xmlReader = InDet__XMLReaderSvc(name='InDetXMLReaderSvc')
###### Insert  at the very beginning of Athena Alg list for the dictionary creation ######
from AthenaCommon.AppMgr import theApp
ServiceMgr += xmlReader
theApp.CreateSvc.insert(0,"InDet::XMLReaderSvc/InDetXMLReaderSvc")

print "CustomTrackingGeometry initialization finished!"
