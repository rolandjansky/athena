# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# Create NSWAGDDTool reading xml from database
#
from MuonAGDD.MuonAGDDConfig import NSWAGDDTool
NSWAGDDDumperFromDB = NSWAGDDTool(name="NSWAGDDDumperFromDB")
NSWAGDDDumperFromDB.ReadAGDD=True
NSWAGDDDumperFromDB.DumpAGDD=True # write blob from DB in Generated_NSWD_pool.txt
NSWAGDDDumperFromDB.Locked=False
NSWAGDDDumperFromDB.Volumes=["NewSmallWheel"] # for the NSWAGDDTool volumes must always be given ("NewSmallWheel" usually includes everything)
NSWAGDDDumperFromDB.DefaultDetector="Muon"

from AGDD2GeoSvc.AGDD2GeoSvcConf import AGDDtoGeoSvc
from AthenaCommon.AppMgr import ServiceMgr, theApp

#
# Create AGDDtoGeoSvc for NSWAGDDTool reading from database
#
AGDD2Geo = AGDDtoGeoSvc()
AGDD2Geo.Builders += [NSWAGDDDumperFromDB]
theApp.CreateSvc += ["AGDDtoGeoSvc"]
ServiceMgr += AGDD2Geo
