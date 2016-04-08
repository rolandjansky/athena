# $Id: xAODSlimming_jobOptions.py 583869 2014-02-18 11:31:09Z krasznaa $
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

# Make sure that the event format metadata is created:
from AthenaCommon.AppMgr import theApp
theApp.CreateSvc += [ "xAODMaker::EventFormatSvc" ]

# Create a derived xAOD file:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
derivedStream = MSMgr.NewPoolStream( "StreamAOD", "DxAOD.pool.root" )

# Set up its contents:
derivedStream.AddItem( "xAOD::EventInfo_v1#*" )
derivedStream.AddItem( "xAOD::CaloClusterContainer_v1#*" )

derivedStream.AddItem( "xAOD::AuxContainerBase#*" )

# This causes a crash at the moment... :-(
#derivedStream.AddItem( "xAOD::AuxInfoBase#EventInfoAux." )

derivedStream.AddMetaDataItem( "xAOD::EventFormat_v1#*" )

# Set up the file writing's details:
derivedStream.Stream.WritingTool.SubLevelBranchName = "<key>"

# Some final tweaking:
theApp.EvtMax = 10
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 10000
#ServiceMgr.StoreGateSvc.Dump = True
