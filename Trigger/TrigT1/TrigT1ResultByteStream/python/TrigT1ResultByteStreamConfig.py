#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

def L1TriggerResultMakerCfg(flags):
  from TrigT1ResultByteStream.TrigT1ResultByteStreamConf import L1TriggerResultMaker,ExampleL1TriggerByteStreamTool
  from libpyeformat_helper import SourceIdentifier,SubDetector

  # Placeholder for real decoder tools - now it's just an example
  muctpi_moduleid = 1
  muctpi_robid = int(SourceIdentifier(SubDetector.TDAQ_MUON_CTP_INTERFACE, muctpi_moduleid))
  exampleTool = ExampleL1TriggerByteStreamTool(ROBIDs=[muctpi_robid],
                                               MUCTPIModuleId=muctpi_moduleid,
                                               MuonRoIContainerWriteKey="LVL1MuonRoIs",
                                               LinkName="mu_roi")

  decoderTools = [exampleTool]

  decoderAlg = L1TriggerResultMaker(name="L1TriggerResultMaker",
                                    L1TriggerResultWHKey="L1TriggerResult",
                                    DecoderTools=decoderTools)

  from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
  acc = ComponentAccumulator()
  acc.addEventAlgo(decoderAlg)
  return acc

def L1ByteStreamDecodersRecExSetup(enableRun2L1=True, enableRun3L1=True):
  # Use new-style config from the above functions and import into old-style JO
  from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  if enableRun2L1:
    CAtoGlobalWrapper(RoIBResultDecoderCfg,ConfigFlags)
  if enableRun3L1:
    CAtoGlobalWrapper(L1TriggerResultMakerCfg,ConfigFlags)
