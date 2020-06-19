
#include( "AthenaPython/iread_file.py" )


# Configure athena common flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags

athenaCommonFlags.EvtMax = 2
athenaCommonFlags.SkipEvents = 0
#athenaCommonFlags.OutputLevel = 2
athenaCommonFlags.AllowIgnoreConfigError = False

FNAME = "ESD.pool.root"
athenaCommonFlags.FilesInput = [FNAME]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = [FNAME,]

#from ParticleBuilderOptions.AODFlags import AODFlags
#AODFlags.FastSimulation = False

from JetValidation.RTTConfig import scheduleRTTJetTests

scheduleRTTJetTests()



# Create a POOL output file with the StoreGate contents:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolStream( "StreamXAOD", "xAOD.Out.pool.root" )

# Set up its contents:
xaodStream.AddItem( "xAOD::CaloClusterContainer_v1#*" )
xaodStream.AddItem( "xAOD::CaloClusterAuxContainer_v1#*" )
## #xaodStream.AddItem( "CaloCellContainer#AODCellContainer") 
## #xaodStream.AddItem( "CaloClusterContainer#"+cont);
xaodStream.AddItem( "xAOD::JetContainer_v1#AntiKt*" )
xaodStream.AddItem( "xAOD::JetAuxContainer_v1#AntiKt*" )

xaodStream.AddMetaDataItem( "xAOD::EventFormat_v1#*" )
#xaodStream.Print()

# Split all branches:
ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [
    "DEFAULT_SPLITLEVEL='99'" ]

# Force POOL to just simply use the StoreGate keys as branch names:
ServiceMgr.AthenaPoolCnvSvc.SubLevelBranchName = "<key>"


# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp

ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000

#ServiceMgr.StoreGateSvc.Dump=True
from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
ServiceMgr += AthenaEventLoopMgr(EventPrintoutInterval = 100)

MessageSvc.Format = "% F%30W%S%7W%R%T %0W%M"



    



