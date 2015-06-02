# $Id: xAODSlimming_jobOptions.py 654409 2015-03-16 15:48:18Z krasznaa $
#
# This jobO is meant to test the capability of Athena to slim the contents
# of an xAOD file.
#

# Read the input file using very basic Athena I/O:
FNAME = "xAOD.pool.root"
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algoritgm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Schedule the auxiliary store wrapper algorithm:
from xAODCoreCnv.xAODCoreCnvConf import xAODMaker__AuxStoreWrapper
alg = xAODMaker__AuxStoreWrapper()
alg.SGKeys = []
alg.OutputLevel = DEBUG
theJob += alg

# Create a derived xAOD file:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
derivedStream = MSMgr.NewPoolRootStream( "StreamAOD", "DxAOD.pool.root" )

# Set up its contents:
derivedStream.AddItem( "xAOD::EventInfo#*" )
derivedStream.AddItem( "xAOD::CaloClusterContainer#*" )

derivedStream.AddItem( "xAOD::AuxInfoBase#*" )
derivedStream.AddItem( "xAOD::AuxContainerBase#*" )

# Some final tweaking:
theApp.EvtMax = 10
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 10000
ServiceMgr.StoreGateSvc.Dump = True
