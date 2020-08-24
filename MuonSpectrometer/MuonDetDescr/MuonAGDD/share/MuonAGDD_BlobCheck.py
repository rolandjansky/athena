# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# Create MuonAGDDTool from local amdb file (input_amdb_simrec) containing AGDD xml block
#
from MuonAGDD.MuonAGDDConfig import MuonAGDDTool
MuonAGDDDumperFromAMDBFile = MuonAGDDTool(name="AGDDDumperFromAMDBFile")
MuonAGDDDumperFromAMDBFile.WriteAGDDFile=True # creates Out.AmdcOracle.AM.AGDDtemp.data and Out.AmdcOracle.AM.AGDD.PREsql
MuonAGDDDumperFromAMDBFile.BuildNSW=False
MuonAGDDDumperFromAMDBFile.Locked=True

#
# Create MuonAGDDTool reading AGDD from database
#
from MuonAGDD.MuonAGDDConfig import MuonAGDDTool
MuonAGDDDumperFromDB = MuonAGDDTool(name="AGDDDumperFromDB")
MuonAGDDDumperFromDB.DumpAGDD=True # write blob from DB in Generated_AGDD_pool.txt
MuonAGDDDumperFromDB.BuildNSW=False
MuonAGDDDumperFromDB.Locked=True

from AGDD2GeoSvc.AGDD2GeoSvcConf import AGDDtoGeoSvc
AGDD2Geo = AGDDtoGeoSvc()
AGDD2Geo.Builders += [MuonAGDDDumperFromAMDBFile]
AGDD2Geo.Builders += [MuonAGDDDumperFromDB]
theApp.CreateSvc += ["AGDDtoGeoSvc"]
ServiceMgr += AGDD2Geo
