#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

# Parse option to specify output item list
ItemList = []
if 'OutputItemList' in globals().keys():
    ItemList = globals()['OutputItemList']

# Set message limit to unlimited when general DEBUG is requested
msgSvc = theApp.service("MessageSvc")
if msgSvc.OutputLevel<=DEBUG :
    msgSvc.defaultLimit = 0
    msgSvc.enableSuppression = False

# Define top sequence and output sequence
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
topSequence = AlgSequence()
outSequence = AthSequencer("AthOutSeq")

# Set input file to new-style flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaConfiguration.AllConfigFlags import ConfigFlags
ConfigFlags.Input.Files = athenaCommonFlags.FilesInput()

# Use new-style config of ByteStream reading and import here into old-style JO
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
CAtoGlobalWrapper(ByteStreamReadCfg,ConfigFlags)

# Define the decoding sequence
from TrigHLTResultByteStream.TrigHLTResultByteStreamConf import HLTResultMTByteStreamDecoderAlg
from TrigOutputHandling.TrigOutputHandlingConf import TriggerEDMDeserialiserAlg
from AthenaCommon.CFElements import seqAND
decoder = HLTResultMTByteStreamDecoderAlg()

deserialiser = TriggerEDMDeserialiserAlg("TrigDeserialiser")

costDataDeserialiser = TriggerEDMDeserialiserAlg("CostDataTrigDeserialiser")
from TriggerMenuMT.HLTMenuConfig.Menu import EventBuildingInfo
costDataDeserialiser.ModuleID=EventBuildingInfo.DataScoutingIdentifiers["CostMonDS"]

decodingSeq = seqAND("Decoding")
decodingSeq += decoder
decodingSeq += deserialiser
decodingSeq += costDataDeserialiser
topSequence += decodingSeq

# Create OutputStream for ESD writing
from OutputStreamAthenaPool.CreateOutputStreams import createOutputStream
StreamESD = createOutputStream("StreamESD","ESD.pool.root",True)
topSequence.remove( StreamESD )
outSequence.remove( StreamESD )

# Define what to write into ESD
from TriggerJobOpts.TriggerFlags import TriggerFlags
from TrigEDMConfig.TriggerEDM import getTriggerEDMList
TriggerFlags.EDMDecodingVersion = 3 # currently hard-coded
edmList = getTriggerEDMList(TriggerFlags.ESDEDMSet(), TriggerFlags.EDMDecodingVersion())
if len(ItemList) == 0:
    for edmType, edmKeys in edmList.iteritems():
        for key in edmKeys:
            ItemList.append(edmType+'#'+key)
    ItemList += [ "xAOD::EventInfo#EventInfo", "xAOD::EventAuxInfo#EventInfoAux." ]
    ItemList += [ 'xAOD::TrigCompositeContainer#*' ]
    ItemList += [ 'xAOD::TrigCompositeAuxContainer#*' ]
StreamESD.ItemList = list(set(ItemList))
outSequence += StreamESD
