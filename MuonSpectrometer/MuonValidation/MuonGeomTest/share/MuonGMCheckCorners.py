#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
# - Select detectors 
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOn()
#--------------------------------------------------------------
DetFlags.simulate.Truth_setOn()
#--------------------------------------------------------------
DetDescrVersion = "ATLAS-DC3-06"
include( "AtlasGeoModel/SetGeometryVersion.py" )
include( "AtlasGeoModel/GeoModelInit.py" )
GeoModelSvc = Service( "GeoModelSvc")
GeoModelSvc.MuonVersionOverride='MuonSpectrometer-R.01.01.Initial.Light_Egg_Rndm'
#--------------------------------------------------------------
#include ( "DetDescrCnvSvc/DetStore_joboptions.py" )
theApp.setup( NO_EVSEL )
#--------------------------------------------------------------
# Set output level threshold 
#(0=NIL 1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc.OutputLevel = 3
#--------------------------------------------------------------
theApp.Dlls += [ 'RootHistCnv' ]
theApp.HistogramPersistency = 'ROOT'
NtupleSvc = Service( "NtupleSvc" )
#ntuple output name
NtupleSvc.Output = [ "FILE1 DATAFILE='MuonGMCheckCorners.root' OPT='NEW'" ]
#--------------------------------------------------------------

# Load RDBAcessSvc (Access to parameter Database)
include ("RDBAccessSvc/RDBAccessSvcPdb_jobOptions.py")
theApp.Dlls += [ "GeoModelSvc" ]
theApp.CreateSvc += [ "GeoModelSvc"]
include( "AtlasGeoModel/MuonGeoModel.py" )    
#--------------------------------------------------------------
theApp.EvtSel = "EventSelector";
theApp.EvtMax = 1;
EventSelector = Service ("EventSelector");
EventSelector.firstRun=1;
EventSelector.lastRun=10000;
#--------------------------------------------------------------
#if not 'MuonGeomTest' in theApp.Dlls:
theApp.Dlls += [ "MuonGeomTest" ]
#--------------------------------------------------------------
#---- display top algorithm 
theApp.TopAlg         =        [ "MuonGMCheckCorners" ]
MuonGMCheckCorners           = Service( "MuonGMCheckCorners" )
MuonGMCheckCorners.print_level =  0;
MuonGMCheckCorners.print_mdt_good_hits = 0;
MuonGMCheckCorners.print_rpc_good_hits = 0;
# to print Reference readout channel maps
MuonGMCheckCorners.check_mdt = 1;
MuonGMCheckCorners.check_rpc = 0;
MuonGMCheckCorners.check_tgc = 0;
MuonGMCheckCorners.check_csc = 0;
# to build region selector maps 
MuonGMCheckCorners.buildMdtRegionSelectorMap = 0;
MuonGMCheckCorners.buildRpcRegionSelectorMap = 0;
#--------------------------------------------------------------
#Station indices:
#-------------------------------------------------------------- 
# BIL: 0, BIS: 1 
# BML: 2, BMS: 3 
# BOL: 4, BOS: 5 
# BEE: 6, BIR: 7 
# BMF: 8, BOF: 9, BOG: 10
# EIL: 13, EML: 17, EMS: 18 
# EOL: 20, EOS: 21, EIS: 22, BIM:23
#--------------------------------------------------------------
MuonGMCheckCorners.station_index = 17;
