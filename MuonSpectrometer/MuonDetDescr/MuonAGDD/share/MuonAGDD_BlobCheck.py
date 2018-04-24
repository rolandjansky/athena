from MuonAGDD.MuonAGDDConfig import MuonAGDDTool
MuonAGDDDumperFromAMDBFile = MuonAGDDTool(name="AGDDDumperFromAMDBFile")
MuonAGDDDumperFromAMDBFile.WriteAGDDFile=True
MuonAGDDDumperFromAMDBFile.BuildNSW=False
MuonAGDDDumperFromAMDBFile.Locked=True
ToolSvc += MuonAGDDDumperFromAMDBFile

# write blob from DB in generated_amdb_simrec_pool.txt
from MuonAGDD.MuonAGDDConfig import MuonAGDDTool
MuonAGDDDumperFromDB = MuonAGDDTool(name="AGDDDumperFromDB")
MuonAGDDDumperFromDB.DumpAGDD=True
MuonAGDDDumperFromDB.BuildNSW=False
MuonAGDDDumperFromDB.Locked=True
ToolSvc += MuonAGDDDumperFromDB

from AGDD2GeoSvc.AGDD2GeoSvcConf import AGDDtoGeoSvc
AGDD2Geo = AGDDtoGeoSvc()
AGDD2Geo.Builders += ["MuonAGDDTool/AGDDDumperFromAMDBFile"]
AGDD2Geo.Builders += ["MuonAGDDTool/AGDDDumperFromDB"]
theApp.CreateSvc += ["AGDDtoGeoSvc"]
ServiceMgr += AGDD2Geo

