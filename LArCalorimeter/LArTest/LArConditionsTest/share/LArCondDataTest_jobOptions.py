###############################################################
#
# Job options file
#
#==============================================================
#use McEventSelector

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

EventSelector = Service( "EventSelector" )
EventSelector.RunNumber=1
EventSelector.EventsPerRun=10;
EventSelector.FirstEvent=1

theApp.Dlls += [ "GaudiAud" ] 
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors =  [ "ChronoAuditor" ] 

# DetDescrVersion = "DC2" 
# DetDescrVersion = "Rome-Initial" 
DetDescrVersion = "ATLAS-DC3-02"


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

include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )

#UseLArCoolPool  = True
#UseFullLArShape = True

#include( "LArCondCnv/LArCondCnv_G4_jobOptions.py" )
#include( "LArCondCnv/LArCondCnv_IdMapAtlas_jobOptions.py" )

# include( "LArRawConditions/LArRawConditions_MC_jobOptions.py" )
#include( "LArRawConditions/LArIdMap_ATLAS_jobOptions.py" )

include( "LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py" )
include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )


#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
#load relevant libraries
theApp.Dlls += [ "LArRawUtils" ]
theApp.Dlls += [ "LArTools" ]
theApp.Dlls += [ "LArConditionsTest" ]
# include the converters
include( "LArCondAthenaPool/LArCondAthenaPool_joboptions.py" )
include ("LArRawConditions/LArRawConditionsDict_joboptions.py")

theApp.TopAlg = [ "LArCondDataTest" ]

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 2
MessageSvc.debugLimit       = 1000000
MessageSvc.infoLimit        = 1000000
LArCondDataTest = Algorithm( "LArCondDataTest" )

#--------------------------------------------------------------
#

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
# Load POOL support
#--------------------------------------------------------------
# Make sure AthenaPoolServices is loaded for custom streamer
include ("AthenaPoolServices/AthenaRootStreamerSvc_jobOptions.py")

include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )

EventPersistencySvc = Service( "EventPersistencySvc" )
EventPersistencySvc.CnvServices += [ "AthenaPoolCnvSvc" ]

#Explicitly specify the output file catalog
#PoolSvc = Service( "PoolSvc" )
#PoolSvc.WriteCatalog = "file:Catalog1.xml"

#LArCondDataTest.TBin = 0
#########################################################
#11.5.0 fix for MuonIDFileName
DetDescrCnvSvc.MuonIDFileName = "IdDictMuonSpectrometer_R.01.xml"
if DetDescrVersion =="ATLAS-DC3-05": 
 DetDescrCnvSvc.LArIDFileName = "IdDictLArCalorimeter_DC3-05.xml"

include ("IOVDbSvc/IOVRecExCommon.py")

#ProxyProviderSvc = Service( "ProxyProviderSvc" )
#ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
#CondProxyProvider = Service( "CondProxyProvider" )
# CondProxyProvider.InputCollections +=["LArRawConditions.MC.DC3-B-IdFix1.001.pool.root"]
