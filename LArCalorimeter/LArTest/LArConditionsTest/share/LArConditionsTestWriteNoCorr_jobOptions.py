###############################################################
#
# Job options file
#
#==============================================================
#use McEventSelector
theApp.setup( MONTECARLO )

theApp.Dlls += [ "GaudiAud" ] 
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors =  [ "ChronoAuditor" ] 


#--------------------------------------------------------------
# Detector Description
#--------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

# Other LAr related 
include( "LArIdCnv/LArIdCnv_joboptions.py" )
#include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_joboptions.py" )

include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )



#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
#load relevant libraries
theApp.Dlls += [ "LArRawUtils" ]
theApp.Dlls += [ "LArTools" ]
theApp.Dlls += [ "LArConditionsTest" ]
# include the converters
include( "LArCondAthenaPool/LArCondAthenaPool_joboptions.py" )


theApp.TopAlg = [ "LArConditionsTestAlg" ]

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 2
MessageSvc.debugLimit       = 100000
MessageSvc.infoLimit        = 100000
LArConditionsTestAlg = Algorithm( "LArConditionsTestAlg" )
LArConditionsTestAlg.OutputLevel    = 2

#--------------------------------------------------------------
#

# Test condition objects - created on the fly
LArConditionsTestAlg.TestCondObjs     = True;
# Read conditions objects for detector store
#LArConditionsTestAlg.ReadCondObjs     = True;
# Write conditions objects to detector store
LArConditionsTestAlg.WriteCondObjs    = True;
# Write correction
#LArConditionsTestAlg.WriteCorrections = True;
# Apply corrections
#LArConditionsTestAlg.ApplyCorrections = True;
#LArConditionsTestAlg.TestFill         = True;
#LArConditionsTestAlg.TestReadDBDirect = True;
#LArConditionsTestAlg.TestHV           = True;
#LArConditionsTestAlg.Testbeam         = True;


#--------------------------------------------------------------
# Set the following when reading back to adjust the run/event numbers
#   (don't forget ApplicationMgr.EvtMax)
EventSelector = Service( "EventSelector" )
#EventSelector.RunNumber         = 1
#EventSelector.EventsPerRun      = 5
#EventSelector.FirstEvent        = 1
#EventSelector.InitialTimeStamp  = 0
#EventSelector.TimeStampInterval = 5
# For the moment, only read one event to get conditions
theApp.EvtMax = 1

#--------------------------------------------------------------
#  Use OutputConditionsAlg to write
#--------------------------------------------------------------

# include the basic setup of the algorithm
include("RegistrationServices/OutputConditionsAlg_jobOptions.py")
# set options specific to my use case
ToolSvc = Service("ToolSvc")
ToolSvc.ConditionsAlgStream.OutputFile="LarCondTestNoCorr.root"
OutputConditionsAlg.ObjectList =  [ "LArRampMC#/LArCalorimeter/LArTests/LArRampsSingleGroup" ]
OutputConditionsAlg.ObjectList += [ "LArRampMC#/LArCalorimeter/LArTests/LArRampsSubDetectorGrouping" ]
OutputConditionsAlg.ObjectList += [ "LArRampMC#/LArCalorimeter/LArTests/LArRampsFeedThroughGrouping" ]
OutputConditionsAlg.WriteIOV=True
#OutputConditionsAlg.Run1=3
#OutputConditionsAlg.Event1=4
#OutputConditionsAlg.Run2=9999
#OutputConditionsAlg.Event2=999999
#OutputConditionsAlg.IOVTagList=["mytag"]
# specify the COOL datatbase connection string (this one is development account)

#--------------------------------------------------------------
# Access to IOVSvc and IOVDbSvc
# Must list the folders to be used for reading
#--------------------------------------------------------------
include ( "IOVDbSvc/IOVDbSvc_jobOptions.py" )
IOVDbSvc = Service( "IOVDbSvc" )
IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;devdb10:LARCORR:ATLAS_COOLTEST:cool4devdb"

#--------------------------------------------------------------
# Options for IOVRegistrationSvc
#--------------------------------------------------------------
#include( "RegistrationServices/IOVRegistrationSvc_jobOptions.py" )
#regSvc = Service( "IOVRegistrationSvc" )
# Select the following to delete and recreate the folders. Default is
# NOT to recreate
#regSvc.RecreateFolders = True
