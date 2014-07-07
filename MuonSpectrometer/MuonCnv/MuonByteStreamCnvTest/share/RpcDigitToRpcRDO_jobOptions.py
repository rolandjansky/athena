# the source of the digits
include( "MuonDigitZebraCnv/G3MuonDigit_jobOptions.py" )

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# use detector store conversion service
include( "MuonDetMgrDetDescrCnv/MuonDetMgrDetDescrCnv_joboptions.py" )

# Turn on the reading of the dictionary
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DecodeIdDict = TRUE
DetDescrCnvSvc.MuonIDFileName  = "IdDictMuonSpectrometer_P.03.xml"
# amdb
include( "AmdbAth/AmdbAth_jobOptions.py" )

#load relevant libraries
theApp.Dlls += [ "RPCgeometry" ]
theApp.Dlls += [ "RPCcabling" ]
ApplicationMdr = Algorithm( "ApplicationMdr" )
ApplicationMdr.Dlls += [ "TrigT1RPChardware" ]
theApp.Dlls += [ "TrigT1RPCsteering" ]
theApp.Dlls += [ "TrigT1RPCmonitoring" ]
theApp.Dlls += [ "MuonByteStreamCnvTest" ]
theApp.ExtSvc += [ "RPCgeometrySvc" ]
theApp.ExtSvc += [ "RPCcablingSvc" ]
#top algorithms to be run
theApp.TopAlg += [ "RpcDigitToRpcRDO" ]
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc.OutputLevel      = 2
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.Dump = FALSE;  #true will dump data store contents
#--------------------------------------------------------------
# Define Printout,Debug and Dump flags for RPC geometry service
#--------------------------------------------------------------
RPCgeometrySvc = Service( "RPCgeometrySvc" )
#RPCgeometrySvc.DumpRPCGeometry = FALSE
#RPCgeometrySvc.DumpRPCStations = FALSE
#RPCgeometrySvc.Debug           = FALSE
#RPCgeometrySvc.DumpLVL1conf    = FALSE
#RPCgeometrySvc.DumpFileName    = "LVL1conf.dump"; 
#RPCgeometrySvc.DataDetail      = FALSE
#--------------------------------------------------------------
# Define configuration file for cabling service
#--------------------------------------------------------------
RPCcablingSvc = Service( "RPCcablingSvc" )
RPCcablingSvc.ConfFileName    = "LVL1conf.data"
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 200
#==============================================================
#
# End of AthExStoreGateExample job options file
#
###############################################################


