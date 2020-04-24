#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def RoIBResultByteStreamToolCfg(name, writeBS=False):
  tool = CompFactory.RoIBResultByteStreamTool(name)
  if writeBS:
    # write BS == read RDO
    tool.RoIBResultReadKey="RoIBResult"
    tool.RoIBResultWriteKey=""
  else:
    # read BS == write RDO
    tool.RoIBResultReadKey=""
    tool.RoIBResultWriteKey="RoIBResult"
  return tool

def ExampleL1TriggerByteStreamToolCfg(name, writeBS=False):
  tool = CompFactory.ExampleL1TriggerByteStreamTool(name)
  from libpyeformat_helper import SourceIdentifier,SubDetector
  muctpi_moduleid = 1
  muctpi_robid = int(SourceIdentifier(SubDetector.TDAQ_MUON_CTP_INTERFACE, muctpi_moduleid))
  tool.MUCTPIModuleId = muctpi_moduleid
  tool.ROBIDs = [muctpi_robid]
  if writeBS:
    # write BS == read xAOD
    tool.MuonRoIContainerReadKey="LVL1MuonRoIs"
    tool.MuonRoIContainerWriteKey=""
  else:
    # read BS == write xAOD
    tool.MuonRoIContainerReadKey=""
    tool.MuonRoIContainerWriteKey="LVL1MuonRoIs"
  return tool

def L1TriggerByteStreamDecoderCfg(flags):
  decoderTools = []
  if flags.Trigger.decodeLegacyL1:
    roibResultTool = RoIBResultByteStreamToolCfg(name="RoIBResultBSDecoderTool", writeBS=False)
    decoderTools += [roibResultTool]

  if flags.Trigger.decodePhaseIL1:
    # Placeholder for real decoder tools - now it's just an example
    exampleTool = ExampleL1TriggerByteStreamToolCfg(name="L1MuonBSDecoderTool", writeBS=False)
    decoderTools += [exampleTool]

  decoderAlg = CompFactory.L1TriggerByteStreamDecoderAlg(name="L1TriggerByteStreamDecoder",
                                                         DecoderTools=decoderTools)

  acc = ComponentAccumulator()
  acc.addEventAlgo(decoderAlg)
  return acc

def L1TriggerByteStreamEncoderCfg(flags):
  from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
  acc = ComponentAccumulator()

  if flags.Trigger.decodeLegacyL1:  # TODO: Maybe have separate encode*L1 flags?
    roibResultTool = RoIBResultByteStreamToolCfg(name="RoIBResultBSEncoderTool", writeBS=True)
    acc.addPublicTool(roibResultTool)

  if flags.Trigger.decodePhaseIL1:
    # Placeholder for real encoder tools - now it's just an example
    exampleTool = ExampleL1TriggerByteStreamToolCfg(name="L1MuonBSEncoderTool", writeBS=True)
    acc.addPublicTool(exampleTool)

  return acc

def L1ByteStreamDecodersRecExSetup(enableRun2L1=True, enableRun3L1=True):
  # Use new-style config from the above functions and import into old-style JO
  from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  ConfigFlags.Trigger.decodeLegacyL1 = enableRun2L1
  ConfigFlags.Trigger.decodePhaseIL1 = enableRun3L1
  CAtoGlobalWrapper(L1TriggerByteStreamDecoderCfg,ConfigFlags)

def L1ByteStreamEncodersRecExSetup():
  # Use new-style config from the above functions and import into old-style JO
  from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  CAtoGlobalWrapper(L1TriggerByteStreamEncoderCfg,ConfigFlags)
