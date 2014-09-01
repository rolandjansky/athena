###############################################################
#
# Job options file
#
## @file TrkTrackReader_jobOptions.py
##
## @brief For Athena POOL test: read back TrkTrack containers
##
## @author RD Schaffer <R.D.Schaffer@cern.ch>
#
#==============================================================
#
# MC Event Selector
# include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

# For ddexamples and ddcnvsvc
include( "AthenaCommon/Atlas.UnixStandardJob.py" )

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

#--------------------------------------------------------------
# Set flags and load det descr
#--------------------------------------------------------------
# For general flags
doAOD     = False
doTrigger = False
DetDescrVersion = "ATLAS-CSC-01-01-00"
include( "RecExCond/RecExCommon_flags.py" )


# Set local flags - only need InDet DetDescr
DetFlags.detdescr.Calo_setOff()
# DetFlags.detdescr.Muon_setOff()
      
# set up all detector description description 
include ("RecExCond/AllDet_detDescr.py")

# Turn on the reading of the dictionary
# DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
# DetDescrCnvSvc.DecodeIdDict = TRUE

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 100

#EventSelector.InputCollection        = "NewPoolTry";
#EventSelector = Service( "EventSelector" )
ServiceMgr.EventSelector.InputCollections        = [ "SimplePoolFile.root" ]

#--------------------------------------------------------------
# Application: AthenaPoolTest InDetRawData options
#--------------------------------------------------------------
theApp.Dlls     += [ "AthenaPoolTest" ]
theApp.TopAlg   += [ "TrkTrackFakeReader" ]
#--------------------------------------------------------------
# Input options
#--------------------------------------------------------------

# Get convertors
include ( "InDetEventCnvTools/InDetEventCnvTools_jobOptions.py" )
include( "InDetEventAthenaPool/InDetEventAthenaPool_joboptions.py" )
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )
include ( "TrkEventAthenaPool/TrkEventAthenaPool_joboptions.py" )


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 3
MessageSvc.setVerbose += [ "TrackCollectionCnv" ]

MessageSvc.debugLimit  = 100000
ClassIDSvc = Service( "ClassIDSvc" )
ClassIDSvc.OutputLevel = 3
TrkTrackFakeReader = Algorithm( "TrkTrackFakeReader" )
TrkTrackFakeReader.OutputLevel = 2
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.Dump = TRUE 

# Temporarily turn off debug for the eventselector
ServiceMgr.EventSelector.OutputLevel = 3
#==============================================================
#
# End of job options file
#
###############################################################
