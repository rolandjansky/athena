# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import jobproperties
from AthenaCommon.DetFlags      import DetFlags
from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags
from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags

if ( jobproperties.Global.DetGeo() == "ctbh8" or jobproperties.Global.DetGeo() == "ctbh6" ):
    print " CTB layout for Muon Spectrometer is not supported anymore"

elif ( DetFlags.detdescr.Muon_on() ):
    from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
    GeoModelSvc = GeoModelSvc()

    from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
    GeoModelSvc.DetectorTools += [ MuonDetectorTool(HasCSC=MuonGeometryFlags.hasCSC(),
                                                    HasSTgc=(CommonGeometryFlags.Run() in ["RUN3", "RUN4"]),
                                                    HasMM=(CommonGeometryFlags.Run() in ["RUN3", "RUN4"])) ]
    import os
    GeoModelSvc.DetectorTools[ "MuonDetectorTool" ].BuildFromNova = 0
    if ( ( not DetFlags.simulate.any_on() or DetFlags.overlay.any_on() ) and "AthSimulation_DIR" not in os.environ ):
      GeoModelSvc.DetectorTools[ "MuonDetectorTool" ].TheMuonAlignmentTool = "MuonAlignmentDbTool/MGM_AlignmentDbTool"
    else:
      GeoModelSvc.DetectorTools[ "MuonDetectorTool" ].TheMuonAlignmentTool = ""
      GeoModelSvc.DetectorTools[ "MuonDetectorTool" ].UseConditionDb = 0
      GeoModelSvc.DetectorTools[ "MuonDetectorTool" ].UseAsciiConditionData = 0
