# setup AMDC for the H8 TestBeam geometry 
include ( "AmdcAth/AmdcAth_jobOptions.py" )
#AmdcsimrecAthenaSvc.NameOfTheAmdbFile = "amdb_simrec.H8_2004_CTB.a.04"


# load the L2 standalone configuration for the H8 RPC geometry
theApp.DLLs += [ "RPCgeometry" ]
theApp.DLLs += [ "RPCcabling" ]

theApp.ExtSvc += [ "RPCgeometrySvc" ]
theApp.ExtSvc += [ "RPCcablingSvc" ]

RPCcablingSvc  = Service ( "RPCcablingSvc" )
RPCgeometrySvc = Service ( "RPCgeometrySvc" )

if ( CombinedRun ):
	RPCcablingSvc.ConfFileName = "TB_25ns_LVL1conf.data"
	RPCcablingSvc.CorrFileName = "TB_25ns_LVL1conf.corr"
else:
	RPCcablingSvc.ConfFileName = "TB_1_LVL1conf.data"
	RPCcablingSvc.CorrFileName = "TB_1_LVL1conf.corr"

RPCgeometrySvc.H8testbeam  = TRUE

#RPCgeometrySvc.DumpRPCStations = TRUE
#RPCgeometrySvc.DumpRPCGeometry = TRUE

# load the L2 standalone configuration for the H8 MDT geometry
theApp.DLLs   += [ "MDTcabling" ]
theApp.ExtSvc += [ "MDTcablingSvc" ]

MDTcablingSvc = Service ( "MDTcablingSvc" )

#==>MDTcablingSvc.RODfile    = "TB_1_RODmap.data" ;
if ( CombinedRun ):
	MDTcablingSvc.RODfile    = "TB_3_RODmap.data"
else:
	MDTcablingSvc.RODfile    = "TB_2_RODmap.data"

MDTcablingSvc.H8testbeam = TRUE

#MDTcablingSvc.MapFlag = TRUE
#MDTcablingSvc.CheFlag = TRUE
#MDTcablingSvc.GeoFlag = TRUE

#load the TGC cabling 
theApp.Dlls += [ "TGCcabling" ]
theApp.ExtSvc += [ "TGCcablingSvc" ]

# Load the DataMonitor Algorithm
theApp.DLLs   += [ "TrigmuFast" ]

# load other L2 services
theApp.ExtSvc += [ "MuRoadsSvc", "MuLUTSvc" ]

theApp.TopAlg += [ "DataMonitor" ]

DataMonitor = Algorithm ( "DataMonitor" )


DataMonitor.OutputLevel = 2;

DataMonitor.MUlvl1INFO  = TRUE;
DataMonitor.MUtrackINFO = TRUE;
DataMonitor.MUroadsINFO = TRUE;
DataMonitor.MUdecoINFO  = TRUE;
DataMonitor.MUcontINFO  = TRUE;
DataMonitor.MUfitINFO   = TRUE;
DataMonitor.MUsagINFO   = TRUE;
DataMonitor.MUptINFO    = TRUE;


#DataMonitor.MdtId = [ 1,2,3,4,5,6,9,10,11,12,13,14 ]
#DataMonitor.MdtId = [ 2,4,6,12 ]

#DataMonitor.RobId = [ 91649,  90378,  90379  ]
#                     RPCROB  MDTROB  MDTROB
DataMonitor.RobId = [ 91649,  90369,  90370,  90883 ]
#                    RPCROB  MDTROB  MDTROB  MDTROB

DataMonitor.OutputLevel = INFO

DataMonitor.MUlvl1INFO = TRUE

DataMonitor.SectorId = 31;

# configure for using MDT calibration
if ( muFastUseCalib ):
	DataMonitor.UseCalibration = True

# Configure the RoI to be processed for the Combined run setup
if ( RoISaleve ):
	DataMonitor.RoiNumber = 30
	DataMonitor.MdtId = [ 1,3,5,12 ]
else:
	DataMonitor.RoiNumber = 6
	DataMonitor.MdtId = [ 2,4,6,12 ]
