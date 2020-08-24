# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

include ( "DetDescrCnvSvc/DetStore_joboptions.py" )
theApp.setup( NO_EVSEL )

# Set output level threshold 
#(0=NIL 1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc.OutputLevel = 3

include ( "AtlasGeoModel/SetGeometryVersion.py" )
theApp.Dlls += [ "GeoModelSvc" ]
theApp.ExtSvc += [ "GeoModelSvc"]
include( "AtlasGeoModel/MuonGeoModel.py" )

theApp.EvtSel = "EventSelector";
theApp.EvtMax = 0;

theApp.Dlls += [ "MuonGeoModelTest"]

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

