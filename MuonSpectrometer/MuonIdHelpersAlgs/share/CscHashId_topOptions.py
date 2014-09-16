
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

include( "MuonEventAthenaPool/MuonEventAthenaPool_joboptions.py" )

include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )

# Pool input
EventSelector = Service( "EventSelector" )
EventSelector.InputCollections = [ "castor:/castor/cern.ch/atlas/csc/valiprod/sampleA/mc11/004202.ZmumuJimmy/digit/v11000401/mc11.004202.ZmumuJimmy.digit.RDO.v11000401._00001.pool.root" ]

# Detector Initialization Initialization
DetDescrVersion = "ATLAS-DC3-02"
include ("RecExCommon/RecExCommon_flags.py")
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
include ("RecExCommon/AllDet_detDescr.py")

theApp.Dlls += [ "MuonIdHelpersAlgs" ]
theApp.TopAlg = [ "CscHashId" ]

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc.OutputLevel      = ERROR
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.Dump = FALSE;  #true will dump data store contents
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1
#==============================================================
#
# End of AthExStoreGateExample job options file
#
###############################################################


