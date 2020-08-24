# Based on ClusterCreator_jobOptions.py

# Set up the reading of the input AOD:
FNAME = "AOD.pool.root"
#include( "AthenaPython/iread_file.py" )

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

#cont="egClusterCollection"
cont="CaloCalTopoCluster"

import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = [FNAME,]
# Add the xAOD container creator algorithm:
from xAODCaloEventCnv.xAODCaloEventCnvConf import ClusterCreator
alg = ClusterCreator(AODContainerName=cont)
#alg.OutputLevel = DEBUG
theJob += alg

import JetRec.JetEventSetup # IMPORTANT ! this setups a necessary alg.
from xAODJetCnv.xAODJetCnvConf import xAODMaker__JetCnvAlg
jcnv = xAODMaker__JetCnvAlg(OutputLevel=2, ConstituentContainer="Clusters")
theJob += jcnv






# Create a POOL output file with the StoreGate contents:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolStream( "StreamXAOD", "xAOD.pool.root" )

# Set up its contents:
xaodStream.AddItem( "xAOD::CaloClusterContainer_v1#*" )
xaodStream.AddItem( "xAOD::CaloClusterAuxContainer_v1#*" )
xaodStream.AddItem( "CaloCellContainer#AODCellContainer") 
xaodStream.AddItem( "CaloClusterContainer#"+cont);
xaodStream.AddItem( "xAOD::JetContainer_v1#*" )
xaodStream.AddItem( "xAOD::JetAuxContainer_v1#*" )

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

#ServiceMgr.StoreGateSvc.Dump=True
from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
ServiceMgr += AthenaEventLoopMgr(EventPrintoutInterval = 100)

