#########################################################
#
# Initialization of the CustomTrackingGeometry
#
#########################################################

print "Starting CustomTrackingGeometry Initialization..."

from ISF_FatrasDetDescrTools.SetupCustomTrackingGeometryBuilder import SetupCustomTrackingGeometry
CustomTrackingGeometry = SetupCustomTrackingGeometry(name = 'CustomTrackingGeometry',
                                                     geometryName     = 'CustomGeometry',
                                                     prefix           = '',
                                                     customPixel      = True,
                                                     customSCT        = True,
                                                     customTRT        = False,
                                                     writeXML         = True,
                                                     layerAssociation = True,
                                                     TRTstrawLayers   = False)


from AthenaCommon.AppMgr import ToolSvc 
ToolSvc += CustomTrackingGeometry.geometryBuilder()

print "CustomTrackingGeometry initialization finished!"
