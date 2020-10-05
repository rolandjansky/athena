# $Id: ClusterDumper_jobOptions.py 589289 2014-03-25 14:41:48Z wlampl $

# Set up the reading of the input AOD:
FNAME = "xAOD.pool.root"
#include( "AthenaPython/iread_file.py" )

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = [FNAME,]



from xAODCaloEventCnv.xAODCaloEventCnvConf import ClusterDumper
#theJob+=ClusterDumper("EMDumper",ContainerName="LArClusterEM7_11Nocorr",FileName="SWclusterFromFile.txt")
theJob+=ClusterDumper("TopoDumper",ContainerName="CaloCalTopoCluster",FileName="TopClusterFromFile.txt")


# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 5
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000

ServiceMgr.StoreGateSvc.Dump=True
from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
ServiceMgr += AthenaEventLoopMgr(EventPrintoutInterval = 1)



