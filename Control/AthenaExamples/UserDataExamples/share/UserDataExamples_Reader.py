PoolESDInput=['UserDataIncluded.pool.root']

# Get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
# Get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence.StopOverride = False

#Pool input
import AthenaPoolCnvSvc.ReadAthenaPool
# EventInfo Converters
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

# Pool input (Change this to use a different file)
from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, 'EventSelector'):
   import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = PoolESDInput

from RecExConfig.ObjKeyStore import objKeyStore
objKeyStore.readInputFile('RecExPers/OKS_streamESD.py')


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#-------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = INFO
#increase the number of letter reserved to the alg/tool name from 18 to 30
svcMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M" 
svcMgr.MessageSvc.defaultLimit = 9999999  # all messages
svcMgr.MessageSvc.useColors = False
svcMgr.MessageSvc.defaultLimit=1000000000



#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1000
theApp.EvtSel = "EventSelector"

from UserDataExamples.UserDataExamplesConf import UserDataExamples__ReadDataWithUserData as UserDataStoreReader
theUserDataStoreReader=UserDataStoreReader("UserDataReader")
theUserDataStoreReader.ContainerName="CaloCalTopoCluster"
theUserDataStoreReader.OutputLevel=DEBUG
topSequence+=theUserDataStoreReader


#theAuditorSvc = svcMgr.AuditorSvc
#theAuditorSvc.Auditors  += [ "ChronoAuditor"]
#theApp.AuditAlgorithms = True
