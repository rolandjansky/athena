#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

# Set input file to new-style flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaConfiguration.AllConfigFlags import ConfigFlags
ConfigFlags.Input.Files = athenaCommonFlags.FilesInput()

# Use new-style config of ByteStream reading and import here into old-style JO
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
CAtoGlobalWrapper(ByteStreamReadCfg,ConfigFlags)

# Define the decoding/analysis sequence
from TrigHLTResultByteStream.TrigHLTResultByteStreamConf import HLTResultMTByteStreamDecoderAlg
from TrigOutputHandling.TrigOutputHandlingConf import TriggerEDMDeserialiserAlg, TruncationAnalysisAlg
from AthenaCommon.CFElements import seqAND
decoder = HLTResultMTByteStreamDecoderAlg()
deserialiser = TriggerEDMDeserialiserAlg("TrigDeserialiser")
deserialiser.ExtraOutputs = [('xAOD::TrigCompositeContainer', 'StoreGateSvc+TruncationDebugInfo')]
truncationAna = TruncationAnalysisAlg("TruncationAnalysis")

decodingSeq = seqAND("Decoding")
decodingSeq += decoder
decodingSeq += deserialiser
decodingSeq += truncationAna

# Add decoding/analysis sequence to topSequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += decodingSeq
