###############################################################
#
# Job options file for LArAlignDbAlg:  see doc in README
#
#==============================================================
#use McEventSelector
theApp.setup( MONTECARLO )

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
#load relevant libraries
theApp.Dlls += [ "LArAlignmentAlgs" ]

# include the converters
include ( "RecAthenaPool/RecAthenaPool_joboptions.py" )
include( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )

theApp.TopAlg = [ "LArAlignDbAlg" ]

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 2
MessageSvc.debugLimit       = 100000
MessageSvc.infoLimit        = 100000
LArAlignDbAlg = Algorithm( "LArAlignDbAlg" )
LArAlignDbAlg.OutputLevel    = 2

#--------------------------------------------------------------
# Choose the following to write out cond objects . 
LArAlignDbAlg.WriteCondObjs   = False
# Choose the following to register cond objects in the IOVDB. 
LArAlignDbAlg.RegisterIOV     = False
# Do not register AttributeList - only for cool version
LArAlignDbAlg.ReadWriteCool   = False
# Do not check AttributeList for two-step write/reg
LArAlignDbAlg.TwoStepWriteReg = True


#--------------------------------------------------------------
# The following turns off the ability to set EventSelector parameters
# below
#ApplicationMgr.EvtSel = "NONE"
# Number of events to be processed (default is 10)
#ApplicationMgr.EvtMax = 1
#--------------------------------------------------------------
# Set the following when reading back to adjust the run/event numbers
#   (don't forget ApplicationMgr.EvtMax)
EventSelector = Service( "EventSelector" )
EventSelector.RunNumber         = 1
EventSelector.EventsPerRun      = 5
EventSelector.FirstEvent        = 1
EventSelector.InitialTimeStamp  = 0
EventSelector.TimeStampInterval = 5
theApp.EvtMax = 20

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
theApp.Dlls += [ "PoolSvc", "AthenaPoolCnvSvc", "AthenaPoolCnvSvcPoolCnv", "EventAthenaPoolPoolCnv", "EventSelectorAthenaPool" ]

EventPersistencySvc = Service( "EventPersistencySvc" )
EventPersistencySvc.CnvServices += [ "AthenaPoolCnvSvc" ]

#Explicitly specify the output file catalog
#PoolSvc = Service( "PoolSvc" )
#PoolSvc.WriteCatalog = "file:Catalog1.xml"

ProxyProviderSvc = Service( "ProxyProviderSvc" )
ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]

CondProxyProvider = Service( "CondProxyProvider" )
CondProxyProvider.InputCollection = "LArAlignmentFile.root"

#Explicitly specify the output file catalog
#PoolSvc = Service( "PoolSvc" )
#PoolSvc.ReadCatalog = [ "file:Catalog1.xml" ]
