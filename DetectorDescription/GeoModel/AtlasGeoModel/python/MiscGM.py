# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# This contains material with no specific home:
#
from AthenaCommon.DetFlags      import DetFlags

if ( DetFlags.detdescr.ID_on() ):
    from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
    GeoModelSvc = GeoModelSvc()

    from BeamPipeGeoModel.BeamPipeGeoModelConf import BeamPipeDetectorTool
    GeoModelSvc.DetectorTools += [ BeamPipeDetectorTool() ]

