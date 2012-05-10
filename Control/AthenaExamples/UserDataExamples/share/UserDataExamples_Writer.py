 #use McEventSelector
import AthenaCommon.AtlasUnixGeneratorJob

# Get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
# Get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence.StopOverride = False


svcMgr.EventSelector.RunNumber         = 100
svcMgr.EventSelector.EventsPerRun      = 1000
svcMgr.EventSelector.FirstEvent        = 1
svcMgr.EventSelector.EventsPerLB       = 100
svcMgr.EventSelector.FirstLB           = 1
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 5
theApp.EvtMax                          = 20


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#-------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = INFO 
#increase the number of letter reserved to the alg/tool name from 18 to 30
svcMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M" 
svcMgr.MessageSvc.defaultLimit = 9999999  # all messages
svcMgr.MessageSvc.useColors = False
svcMgr.MessageSvc.defaultLimit=100000000

from AthenaServices.TheUserDataSvc import TheUserDataSvc
svcMgr += TheUserDataSvc("UserDataSvc")
#svcMgr.UserDataSvc.OutputLevel=DEBUG

from UserDataExamples.UserDataExamplesConf import UserDataExamples__CreateDataWithUserData as UserDataSvcWriter 
theUserDataSvcWriter=UserDataSvcWriter("UserDataWriter")
theUserDataSvcWriter.ContainerName="CaloCalTopoCluster"
#theUserDataStoreWriter.OutputLevel=DEBUG
topSequence+=theUserDataSvcWriter

from UserDataExamples.UserDataExamplesConf import UserDataExamples__ReadDataWithUserData as UserDataStoreReader
theUserDataStoreReader=UserDataStoreReader("UserDataReader")
theUserDataStoreReader.ContainerName="CaloCalTopoCluster"
theUserDataStoreReader.OutputLevel=DEBUG
topSequence+=theUserDataStoreReader

# create stream
from AthenaPoolCnvSvc import WriteAthenaPool


from AthenaServices.AthenaServicesConf import AthenaOutputStream
streamOut = AthenaOutputStream("StreamOut")
streamOut.WritingTool = "AthenaPoolOutputStreamTool"
streamOut.EvtConversionSvc = "AthenaPoolCnvSvc" 
streamOut.OutputFile    = "UserDataIncluded.pool.root"
#streamOut.ForceRead = True
streamOut.ItemList+=["EventInfo#*","CaloClusterContainer#*","UserDataStore#*"]


topSequence+=streamOut
