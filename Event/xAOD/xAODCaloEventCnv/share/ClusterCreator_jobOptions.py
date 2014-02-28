# $Id: ClusterCreator_jobOptions.py 578532 2014-01-15 14:17:14Z wlampl $

# Set up the reading of the input AOD:
FNAME = "AOD.pool.root"
#include( "AthenaPython/iread_file.py" )

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = [FNAME,]
# Add the xAOD container creator algorithm:
from xAODCaloEventCnv.xAODCaloEventCnvConf import ClusterCreator
alg = ClusterCreator(AODContainerNames=["egClusterCollection"],OutputLevel=DEBUG)
#alg = ClusterCreator(OutputLevel=DEBUG)

theJob += alg

# Create a POOL output file with the StoreGate contents:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolStream( "StreamXAOD", "xAOD.pool.root" )

# compactify calo cell
from CaloTools.CaloToolsConf import CaloCompactCellTool
svcMgr.ToolSvc += CaloCompactCellTool()

# Set up its contents:
xaodStream.AddItem( "xAOD::CaloClusterContainer_v1#*" )
xaodStream.AddItem( "xAOD::CaloClusterAuxContainer_v1#*" )
xaodStream.AddItem( "CaloCellContainer#AODCellContainer") 
xaodStream.AddItem( "CaloClusterCellLinkContainer#*") 

#xaodStream.AddItem( "CaloClusterContainer#*");
xaodStream.AddMetaDataItem( "xAOD::EventFormat_v1#*" )
xaodStream.Print()

# Split all branches:
ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [
    "DEFAULT_SPLITLEVEL='99'" ]

# Force POOL to just simply use the StoreGate keys as branch names:
ServiceMgr.AthenaPoolCnvSvc.SubLevelBranchName = "<key>"

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 5
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000

ServiceMgr.AthenaSealSvc.OutputLevel = VERBOSE

#ServiceMgr.StoreGateSvc.Dump=True
from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
ServiceMgr += AthenaEventLoopMgr(EventPrintoutInterval = 100)

ServiceMgr.AthenaPoolCnvSvc.OutputLevel=VERBOSE

