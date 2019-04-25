#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def RoIBResultDecoderCfg(flags):
  from TrigT1ResultByteStream.TrigT1ResultByteStreamConf import RoIBResultByteStreamTool,RoIBResultByteStreamDecoderAlg
  decoderTool = RoIBResultByteStreamTool()
  decoderAlg = RoIBResultByteStreamDecoderAlg(name="RoIBResultByteStreamDecoderAlg",
                                              RoIBResultWHKey="RoIBResult",
                                              DecoderTool=decoderTool)

  from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
  acc = ComponentAccumulator()
  acc.addEventAlgo(decoderAlg)
  return acc