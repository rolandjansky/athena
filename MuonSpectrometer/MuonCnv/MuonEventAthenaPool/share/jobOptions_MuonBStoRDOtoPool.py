#--------------------------------------------------------------
# jobOptions for creating bare Muon RDOs from BS, then write to POOL
#--------------------------------------------------------------
# Input
include( "ByteStreamCnvSvc/ByteStreamSelector_jobOptions.py" )

include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )

# configure Muon system
include( "MuonEventAthenaPool/ConfigureMuons.py" )

# read Muon RDOs by CBNT
include( "MuonEventAthenaPool/ReadMuonRDO.py" )

theApp.Dlls += [ "MuonByteStream" ]
theApp.ExtSvc += [ "ByteStreamCnvSvc" ] 
#--------------------------------------------------------------
# write out Transient Event Store content in POOL
#--------------------------------------------------------------
include( "MuonEventAthenaPool/MuonRdoToPool_jobOptions.py" )

#--------------------------------------------------------------
# Event parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 100000
#--------------------------------------------------------------
# Output levels
#--------------------------------------------------------------
# Set output level threshold 
MessageSvc.OutputLevel  = 3 
# switch down Svc output level
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.OutputLevel = 3 
ClassIDSvc = Service( "ClassIDSvc" )
ClassIDSvc.OutputLevel = 4 
DllClassManager = Algorithm( "DllClassManager" )
DllClassManager.OutputLevel = 4 
#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='muonBStoPOOL.root' OPT='NEW'" ]
# Histogram output, if any. 
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "histo.hbook"
#////// FIXME
# remove this line in the future
ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += [ "CscRawDataContainer/CSCRDO" ]
