# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# InDet GeoModel initialization for CTB
#

from AthenaCommon.DetFlags      import DetFlags

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()

if ( DetFlags.detdescr.pixel_on()):
    from PixelTestBeamDetDescr.PixelTestBeamDetDescrConf import PixelTBDetectorTool
    GeoModelSvc.DetectorTools += [ PixelTBDetectorTool() ]

if ( DetFlags.detdescr.SCT_on()):
    from SCT_TestBeamDetDescr.SCT_TestBeamDetDescrConf import SCT_TBDetectorTool
    GeoModelSvc.DetectorTools += [ SCT_TBDetectorTool() ]
    
if ( DetFlags.detdescr.TRT_on()):
    from TRT_TestBeamDetDescr.TRT_TestBeamDetDescrConf import TRT_TBDetectorTool
    GeoModelSvc.DetectorTools += [ TRT_TBDetectorTool() ]


