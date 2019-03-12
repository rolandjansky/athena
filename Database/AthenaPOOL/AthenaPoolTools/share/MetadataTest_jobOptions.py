## @file EventCount_jobOptions.py
## @brief JobOptions: Defaults.
## @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
## $Id: EventCount_jobOptions.py,v 1.6 2007-10-18 13:46:20 gemmeren Exp $
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr

from CLIDComps.CLIDCompsConf import ClassIDSvc
svcMgr += ClassIDSvc()

from AthenaCommon.Constants   import INFO
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AlgSequence import AthSequencer
athOutSeq = AthSequencer("AthOutSeq")

# include MetaDataSvc and initialize properties
include("EventSelectorAthenaPool/MetaDataSvc_jobOptions.py")
from AthenaServices.AthenaServicesConf import MetaDataSvc
svcMgr += MetaDataSvc()
svcMgr.MetaDataSvc.OutputLevel = 2
svcMgr.MetaDataSvc.MetaDataContainer = "MetaDataHdr"
svcMgr.ProxyProviderSvc.ProviderNames += [ "MetaDataSvc" ]

# include some tools
from EventBookkeeperTools.CutFlowHelpers import CreateBookkeeperTool
CreateBookkeeperTool()

from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamMetadataTool
svcMgr.MetaDataSvc.MetaDataTools += [ "ByteStreamMetadataTool" ]

#print "SGInputLoader init"
#from SGComps.SGCompsConf import SGInputLoader
#sgin = SGInputLoader()
#sgin.Load = [('EventInfo','ByteStreamEventInfo')]

# Tell it that there is data in the input file
from IOVSvc.IOVSvcConf import MetaInputLoader
metain = MetaInputLoader()
metain.Dump = True
metain.OutputLevel = INFO
metain.Load = [('EventStreamInfo','StreamAOD'),('xAOD::CutBookkeeperContainer','CutBookkeepers'),('xAOD::CutBookkeeperAuxContainer','CutBookkeepersAux.'),('ByteStreamMetadataContainer','ByteStreamMetadata'),('xAOD::CutBookkeeperContainer','IncompleteCutBookkeepers'),('xAOD::CutBookkeeperAuxContainer','IncompleteCutBookkeepersAux.')]
topSequence += metain

include ("AthenaPoolTools/EventCount_jobOptions.py")

from AthenaPoolTools.AthenaPoolToolsConf import MetadataTest
topSequence += MetadataTest( OutputLevel = INFO, InputStream="StreamAOD" )

import PyUtils.AthFile as af
from EventBookkeeperTools.CutFlowHelpers import CreateCutFlowSvc
CreateCutFlowSvc( svcName="CutFlowSvc", athFile=af, seq=topSequence, addMetaDataToAllOutputFiles=False )

import AthenaPoolCnvSvc.WriteAthenaPool

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
DataStream = AthenaPoolOutputStream( "StreamTest" , "MetadataTest.root", True, noTag=True )
DataStream.TakeItemsFromInput = True
DataStream.ItemList   += ["EventInfo#ByteStreamEventInfo"]
DataStream.MetadataItemList   += ["xAOD::CutBookkeeperContainer#CutBookkeepers"]
DataStream.MetadataItemList   += ["xAOD::CutBookkeeperAuxContainer#CutBookkeepersAux."]
DataStream.MetadataItemList   += ["xAOD::CutBookkeeperContainer#IncompleteCutBookkeepers"]
DataStream.MetadataItemList   += ["xAOD::CutBookkeeperAuxContainer#IncompleteCutBookkeepersAux."]
DataStream.MetadataItemList   += ["ByteStreamMetadataContainer#ByteStreamMetadata"]
DataStream.OutputLevel = DEBUG

#==============================================================
#
# End of job options file
#
###############################################################
