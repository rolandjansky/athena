# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# use this file as postInclude to initialize a muon amdb geometry from a local amdb file

try:
    if input_amdb_simrec: print ('INFO: initializing geometry from local amdb file %s'%input_amdb_simrec)
except NameError:
    print ('WARNING: You need to specify a amdb file to read from (relative path) as "input_amdb_simrec" variable')
    import sys
    sys.exit(0)

# Configuring MuonGeoModel to use AmdcDb instead of RDBAccessSvc: 
from AthenaCommon.AppMgr import ToolSvc, ServiceMgr

if hasattr(ServiceMgr,"GeoModelSvc"):
    for t in getattr(svcMgr,"GeoModelSvc").DetectorTools:
        if t.name()=="MuonDetectorTool":
            t.BuildFromAmdcDb=1

# Configuring AmdcAth to pick up the local file which should be used later on by AmdcDb 
from AmdcAth.AmdcAthConf import AmdcsimrecAthenaSvc
ServiceMgr += AmdcsimrecAthenaSvc("MyAmdbSvc", NameOfTheSource="ASCII", NameOfTheAmdbFile=input_amdb_simrec)

try:
    if amdbTag: print ('INFO: using AMDBtag %s'%amdbTag)
except NameError:
    amdbTag="MuonSpectrometer-R.08.01"

# Configuring AmdcDb the way like the tables for the database are produced
from AmdcDb.AmdcDbConf import AmdcDb
ServiceMgr += AmdcDb(
    AmdcsimrecAthenaSvc="MyAmdbSvc",
    AMDBtag=amdbTag,
    UglyCodeOn = 0,
    FromAmdcEpsLengthMM = 4,
    FromAmdcEpsLengthCM = 4,
    FromAmdcEpsAngle    = 6,
    FromRDBEpsLengthMM = 4,
    FromRDBEpsLengthCM = 4,
    FromRDBEpsAngle    = 6,
    TabFromRDBEpsLengthMM  = [ "AMDC" ,"DBAM" ,"WCSC" ,"WMDT" , ],
    ValFromRDBEpsLengthMM  = [ 6      ,6      ,6      ,6      , ],
    TabFromRDBEpsLengthCM  = [ "AMDC" ,"DBAM" ,"WCSC" ,"WMDT" , ],
    ValFromRDBEpsLengthCM  = [ 6      ,6      ,6      ,6      , ],
    TabFromAmdcEpsLengthMM = [ "AMDC" ,"DBAM" ,"WCSC" ,"WMDT" , ],
    ValFromAmdcEpsLengthMM = [ 6      ,6      ,6      ,6      , ],
    TabFromAmdcEpsLengthCM = [ "AMDC" ,"DBAM" ,"WCSC" ,"WMDT" , ],
    ValFromAmdcEpsLengthCM = [ 6      ,6      ,6      ,6      , ]
)

