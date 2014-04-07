# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import jobproperties
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()

RemoveOldSmallWheels=False

if jobproperties.Global.DetDescrVersion=="ATLAS-GEOTEST" or jobproperties.Global.DetDescrVersion=="ATLAS-GEO-21-00-01":
    RemoveOldSmallWheels = True
else:    
    if hasattr(GeoModelSvc, 'MuonVersionOverride') and GeoModelSvc.MuonVersionOverride=="MuonSpectrometer-R.06.03-NSW.01.64":
        RemoveOldSmallWheels = True
        

if RemoveOldSmallWheels:
    #***************************************************** HERE setup MuonDetectorManager
    print 'MuonGeoSetup: NSW inconsistent geometry tag - removing stations in old small wheels'
    from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
    MuonDetectorTool = MuonDetectorTool()
    MuonDetectorTool.StationSelection  = 2
    MuonDetectorTool.SelectedStations  = [ "EIL1" ]
    MuonDetectorTool.SelectedStations  += [ "EIL2" ]
    MuonDetectorTool.SelectedStations  += [ "EIS*" ]
    MuonDetectorTool.SelectedStations  += [ "EIL10" ]
    MuonDetectorTool.SelectedStations  += [ "EIL11" ]
    MuonDetectorTool.SelectedStations  += [ "CSS*" ]
    MuonDetectorTool.SelectedStations  += [ "CSL*" ]
    MuonDetectorTool.SelectedStations  += [ "T4E*" ]
    MuonDetectorTool.SelectedStations  += [ "T4F*" ]

from AtlasGeoModel import SetupRecoGeometry
