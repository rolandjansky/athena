###############################################################
#
# Job options file
#
#==============================================================
#
AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )

include( "PartPropSvc/PartPropSvc.py" )

#* load relevant libraries
theApp.Dlls += [ "GaudiAlg" ]
theApp.Dlls += [ "GaudiAud" ]
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors  = [ "ChronoAuditor" ]
theAuditorSvc.Auditors += [ "MemStatAuditor" ]
MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )
ChronoAuditor  = theAuditorSvc.auditor( "ChronoAuditor" )

# import the data types 
import EventKernel.ParticleDataType

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed
if not 'EVTMAX' in dir() :
    EVTMAX = -1
    pass
theApp.EvtMax = EVTMAX

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
PoolSvc = Service( "PoolSvc" )

#--------------------------------------------------------------
# Define the InputCollections
#   There are three types:
#     ImplicitROOT (default) - just file with event objects
#     ExplicitROOT           - ROOT-based  collections
#     ExplicitMySQL          - MySQL-based collections
# E.g. one needs for explicit collection:
# EventSelector.CollectionType  =  "ExplicitROOT" 
# This example uses ImplicitROOT 
#--------------------------------------------------------------
EventSelector = Service( "EventSelector" )

EventSelector.InputCollections = [ "assocs.pool.root" ]

#PoolSvc.ReadCatalog = [ 'xmlcatalog_file:TestCatalog.xml' ]
#PoolSvc.WriteCatalog = 'xmlcatalog_file:TestCatalog.xml'

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
theApp.ExtSvc += ["AtRndmGenSvc"]

#####################################################
# ReadINav4MomAssocs
# --> Load and read the INav4MomAssocs content
#####################################################
theApp.Dlls += ["AnalysisTest" ]
theApp.TopAlg += [ "ReadINav4MomAssocs/ReadINav4MomAssocs" ]
ReadINav4MomAssocs = Algorithm( "ReadINav4MomAssocs" )

ReadINav4MomAssocs.Assocs = "INav4MomAssocs"
ReadINav4MomAssocs.OutputLevel = VERBOSE

#--------------------------------------------------------------
# POOL Persistency
#--------------------------------------------------------------
# POOL converters
include( "AtlfastAthenaPool/AtlfastAthenaPool_joboptions.py" )
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py" )

PoolSvc = Service( "PoolSvc" )

# Run OutputStream as an algorithm
theApp.TopAlg   += [ "AthenaOutputStream/Stream1" ]
# Must make sure that no OutStream's have been declared
theApp.OutStream     = []; 
# Stream's output file
Stream1                  =  Algorithm( "Stream1" )
Stream1.EvtConversionSvc = "AthenaPoolCnvSvc"; 
Stream1.OutputFile = "assocs.readback.pool.root"

# Event Info
Stream1.ItemList += [ "EventInfo#*" ]

PoolSvc.OutputLevel        = WARNING

##############################################################
#
#            Customise OutputLevel
#
##############################################################

# OUTPUT PRINTOUT LEVEL
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.useColors                 = False
MessageSvc.OutputLevel               = 3
MessageSvc.defaultLimit              = 4000000
AthenaEventLoopMgr.OutputLevel	     = 4

MemStatAuditor.OutputLevel           = 4 
ChronoAuditor.OutputLevel            = 4 

StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = True;
StoreGateSvc.OutputLevel=3
#==============================================================
#
# End of job options file
#
###############################################################



# athena AnalysisExamples/produceGenEvents_jobOptions.py && athena -c 'INPUT=["McEvent.root"]; OUTPUT="AOD.pool.root"; EVTMAX=10; ' AssocsRTT/FastSimToAOD_topOptions.py && athena -c 'EVTMAX=10' AnalysisTest/WriteAodAssocs_jobOptions.py && athena AnalysisTest/ReadAodAssocs_jobOptions.py
