###############################################################
#
# Job options file
#
#==============================================================
#use McEventSelector

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

EventSelector = svcMgr.EventSelector
EventSelector.RunNumber=1
EventSelector.EventsPerRun=10;
EventSelector.FirstEvent=1

theApp.Dlls += [ "GaudiAud" ] 
theAuditorSvc = svcMgr.AuditorSvc
theAuditorSvc.Auditors =  [ "ChronoAuditor" ] 


DetDescrVersion="ATLAS-DC3-05"

from AthenaCommon.GlobalFlags import GlobalFlags
#GlobalFlags.DataSource.set_data()
#GlobalFlags.InputFormat.set_bytestream()
GlobalFlags.DetGeo.set_atlas()

#--------------------------------------------------------------
# Detector Description
#--------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion="ATLAS-DC3-05"

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

include ("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")

# include the converters
include( "LArCondAthenaPool/LArCondAthenaPool_joboptions.py" )
include ("LArRawConditions/LArRawConditionsDict_joboptions.py")

# Other LAr related 
include( "LArIdCnv/LArIdCnv_joboptions.py" )

include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )

UseLArCoolPool  = True
UseFullLArShape = True

# include( "LArCondCnv/LArCondCnv_G4_jobOptions.py" )
# include( "LArCondCnv/LArCondCnv_IdMapAtlas_jobOptions.py" )

include( "LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py" )
include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
#load relevant libraries
theApp.Dlls += [ "LArRawUtils" ]
theApp.Dlls += [ "LArTools" ]
theApp.Dlls += [ "LArConditionsTest" ]

from LArConditionsTest.LArConditionsTestConf import LArConditionsTestAlg
topSequence += LArConditionsTestAlg()

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 2
MessageSvc.debugLimit       = 100000
MessageSvc.infoLimit        = 100000
LArConditionsTestAlg = Algorithm( "LArConditionsTestAlg" )

#--------------------------------------------------------------
#

# Test condition objects - created on the fly
LArConditionsTestAlg.TestMC     = True;

#--------------------------------------------------------------
# Set the following when reading back to adjust the run/event numbers
#   (don't forget ApplicationMgr.EvtMax)
#EventSelector.RunNumber         = 1
#EventSelector.EventsPerRun      = 5
#EventSelector.FirstEvent        = 1
#EventSelector.InitialTimeStamp  = 0
#EventSelector.TimeStampInterval = 5
# For the moment, only read one event to get conditions
theApp.EvtMax = 1

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
# Make sure AthenaPoolServices is loaded for custom streamer
include ("AthenaPoolServices/AthenaRootStreamerSvc_jobOptions.py")

