from MuonAGDD.MuonAGDDConfig import MuonAGDDTool
MuonAGDDDumperFromAMDBFile = MuonAGDDTool(name="AGDDDumperFromAMDBFile")
MuonAGDDDumperFromAMDBFile.WriteAGDDFile=True
MuonAGDDDumperFromAMDBFile.BuildNSW=False
MuonAGDDDumperFromAMDBFile.Locked=True

# write blob from DB in Generated_AMDB_pool.txt
from MuonAGDD.MuonAGDDConfig import MuonAGDDTool
MuonAGDDDumperFromDB = MuonAGDDTool(name="AGDDDumperFromDB")
MuonAGDDDumperFromDB.DumpAGDD=True
MuonAGDDDumperFromDB.BuildNSW=False
MuonAGDDDumperFromDB.Locked=True

from AGDD2GeoSvc.AGDD2GeoSvcConf import AGDDtoGeoSvc
AGDD2Geo = AGDDtoGeoSvc()
AGDD2Geo.Builders += [MuonAGDDDumperFromAMDBFile]
AGDD2Geo.Builders += [MuonAGDDDumperFromDB]
theApp.CreateSvc += ["AGDDtoGeoSvc"]
ServiceMgr += AGDD2Geo

