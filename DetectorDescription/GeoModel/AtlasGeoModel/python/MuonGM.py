# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import jobproperties
from AthenaCommon.DetFlags      import DetFlags
from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
from AthenaCommon import Logging

if ( jobproperties.Global.DetGeo() == "ctbh8" or jobproperties.Global.DetGeo() == "ctbh6" ):
    Logging.log.warning(" CTB layout for Muon Spectrometer is not supported anymore")

elif ( DetFlags.detdescr.Muon_on() ):
    from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
    GeoModelSvc = GeoModelSvc()

    from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
    GeoModelSvc.DetectorTools += [ MuonDetectorTool(HasCSC=MuonGeometryFlags.hasCSC(),
                                                    HasSTgc=MuonGeometryFlags.hasSTGC(),
                                                    HasMM=MuonGeometryFlags.hasMM()) ]
    import os
    GeoModelSvc.DetectorTools[ "MuonDetectorTool" ].BuildFromNova = 0
    if ( ( not DetFlags.simulate.any_on() or DetFlags.overlay.any_on() ) and "AthSimulation_DIR" not in os.environ ):
#      GeoModelSvc.DetectorTools[ "MuonDetectorTool" ].TheMuonAlignmentTool = "MuonAlignmentDbTool/MGM_AlignmentDbTool"
        pass
    else:
      GeoModelSvc.DetectorTools[ "MuonDetectorTool" ].UseConditionDb = 0
      GeoModelSvc.DetectorTools[ "MuonDetectorTool" ].UseAsciiConditionData = 0
