###############################################################
#
# Job options file
#
#==============================================================
#use McEventSelector

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
theMessageSvc = svcMgr.MessageSvc
theMessageSvc.OutputLevel      = DEBUG
theMessageSvc.debugLimit       = 100000
theMessageSvc.infoLimit        = 100000


EventSelector = svcMgr.EventSelector
EventSelector.RunNumber=1
EventSelector.EventsPerRun=10;
EventSelector.FirstEvent=1

theApp.Dlls += [ "GaudiAud" ] 
theAuditorSvc = svcMgr.AuditorSvc
theAuditorSvc.Auditors =  [ "ChronoAuditor" ] 

# DetDescrVersion = "DC2" 
# DetDescrVersion = "Rome-Initial" 
#  DetDescrVersion = "ATLAS-DC3-05"

from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-DC3-05"

GlobalFlags.DataSource.set_geant4()
# GlobalFlags.InputFormat.set_bytestream()
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.Luminosity.set_zero()

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

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

include ("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")

# include the converters
include( "LArCondAthenaPool/LArCondAthenaPool_joboptions.py" )
include ("LArRawConditions/LArRawConditionsDict_joboptions.py")
include ("CaloUtils/CaloUtilsDict_joboptions.py")

# Other LAr related 
include( "LArIdCnv/LArIdCnv_joboptions.py" )

include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )

include( "LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py" )
include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
#load relevant libraries
theApp.Dlls += [ "LArRawUtils" ]
theApp.Dlls += [ "LArTools" ]
# theApp.Dlls += [ "LArConditionsTest" ]

include ("CaloTools/CaloNoiseTool_jobOptions.py")

from LArConditionsTest.LArConditionsTestConf import TestLArConditionsTools
topSequence += TestLArConditionsTools()
theCaloNoiseTool.LoadAtBegin= True

theApp.EvtMax = 1

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
# Make sure AthenaPoolServices is loaded for custom streamer
# include ("AthenaPoolServices/AthenaRootStreamerSvc_jobOptions.py")

EventSelector.OverrideRunNumber=False
