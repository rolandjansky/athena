###############################################################
#
# Job options file
#
#==============================================================
#use McEventSelector
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

theApp.Dlls += [ "GaudiAud" ] 
#theAuditorSvc = AuditorSvc()
#theAuditorSvc.Auditors =  [ "ChronoAuditor" ] 


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

from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value_and_Lock('geant4')
globalflags.DetGeo.set_Value_and_Lock('atlas')
include ("AtlasGeoModel/SetGeometryVersion.py")
from AtlasGeoModel import GeoModelInit
include ("LArRawConditions/LArIdMap_ATLAS_jobOptions.py")

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


#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 3
MessageSvc.debugLimit       = 100000
MessageSvc.infoLimit        = 100000

from LArConditionsTest import LArConditionsTestConf
LArConditionsTestAlg = LArConditionsTestConf.LArConditionsTestAlg()
LArConditionsTestAlg.OutputLevel    = 2

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += LArConditionsTestAlg

#--------------------------------------------------------------
#

# Test condition objects - created on the fly
LArConditionsTestAlg.TestCondObjs     = True;
# Read conditions objects for detector store
#LArConditionsTestAlg.ReadCondObjs     = True;
# Write conditions objects to detector store
LArConditionsTestAlg.WriteCondObjs    = True;
# Write correction
LArConditionsTestAlg.WriteCorrections = True;
# Apply corrections
LArConditionsTestAlg.ApplyCorrections = True;
#LArConditionsTestAlg.TestFill         = True;
#LArConditionsTestAlg.TestReadDBDirect = True;
#LArConditionsTestAlg.TestHV           = True;
#LArConditionsTestAlg.Testbeam         = True;


#--------------------------------------------------------------
# Set the following when reading back to adjust the run/event numbers
#   (don't forget ApplicationMgr.EvtMax)
EventSelector = svcMgr.EventSelector
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

#EventPersistencySvc = Service( "EventPersistencySvc" )
#EventPersistencySvc.CnvServices += [ "AthenaPoolCnvSvc" ]

# Explicitly specify the output file catalog
# to avoid races when running tests in parallel.
PoolSvc = Service( "PoolSvc" )
PoolSvc.WriteCatalog = "file:Catalog1.xml"

#ProxyProviderSvc = Service( "ProxyProviderSvc" )
#ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]

#CondProxyProvider = Service( "CondProxyProvider" )
#CondProxyProvider.InputCollections = ["SimpleCoolPoolFile.root"]
#CondProxyProvider.InputCollections = ["LArRawCond_dc2.pool.root"]
#CondProxyProvider.InputCollections = ["LArRawCond_dc2_a.pool.root"]

conddb.setGlobalTag("OFLCOND-CSC-00-01-00")

# Suppress useless GeoModelSvc messages.
from AthenaCommon import Constants
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc().OutputLevel=Constants.WARNING
