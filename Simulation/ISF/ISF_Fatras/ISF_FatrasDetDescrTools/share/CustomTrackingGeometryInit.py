# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#########################################################
#
# Initialization of the CustomTrackingGeometry
#
#########################################################

printfunc ("Starting CustomTrackingGeometry Initialization...")

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

printfunc ("CustomTrackingGeometry initialization finished!")
