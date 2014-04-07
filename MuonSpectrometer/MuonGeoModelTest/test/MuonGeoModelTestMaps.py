# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----
include ( "DetDescrCnvSvc/DetStore_joboptions.py" )
theApp.setup( NO_EVSEL )

# Set output level threshold 
#(0=NIL 1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc.OutputLevel = 3

#----
#DetDescrVersion = "Rome-Final"
include ( "AtlasGeoModel/SetGeometryVersion.py" )
include ("RDBAccessSvc/RDBAccessSvcPdb_jobOptions.py")
theApp.Dlls += [ "GeoModelSvc" ]
theApp.ExtSvc += [ "GeoModelSvc"]
include( "AtlasGeoModel/MuonGeoModel.py" )

#include( "NovaCnvSvc/NovaCnvSvc_jobOptions.py" )
#include( "NovaConverters/NovaConverters_jobOptions.py" )
#GeoModelSvc = Service("GeoModelSvc")
#GeoModelSvc.Detectors += [ "MuonDetectorTool" ]
## options 
#GeoModelSvc.MuonDetectorTool.BuildFromNova = 1
#NovaCnvSvc.Version = 1

theApp.EvtSel = "EventSelector";
theApp.EvtMax = 0;
#EventSelector = Service ("EventSelector");
#EventSelector.firstRun=1;
#EventSelector.lastRun=10000;

theApp.DLLs += [ "MuonGeoModelTest"]

#---- display top algorithm 
theApp.TopAlg         =        [ "MuonGMCheck" ]
MuonGMCheck           = Service( "MuonGMCheck" )
MuonGMCheck.print_level =  0;
MuonGMCheck.print_mdt_good_hits = 0;
MuonGMCheck.print_rpc_good_hits = 0;
# to print Reference readout channel maps
MuonGMCheck.check_mdt = 1;
MuonGMCheck.check_rpc = 1;
MuonGMCheck.check_tgc = 1;
MuonGMCheck.check_csc = 1;

