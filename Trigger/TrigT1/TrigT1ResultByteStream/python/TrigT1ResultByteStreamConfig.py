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

from TrigT1ResultByteStream.TrigT1ResultByteStreamConf import ExampleL1TriggerByteStreamTool as _ExampleL1TriggerByteStreamTool
class ExampleL1TriggerByteStreamTool(_ExampleL1TriggerByteStreamTool):
  def __init__(self, name, writeBS=False, *args, **kwargs):
    super(ExampleL1TriggerByteStreamTool, self).__init__(name, *args, **kwargs)
    from libpyeformat_helper import SourceIdentifier,SubDetector
    muctpi_moduleid = 1
    muctpi_robid = int(SourceIdentifier(SubDetector.TDAQ_MUON_CTP_INTERFACE, muctpi_moduleid))
    self.MUCTPIModuleId = muctpi_moduleid
    self.ROBIDs = [muctpi_robid]
    if writeBS:
      # write BS == read xAOD
      self.MuonRoIContainerReadKey="LVL1MuonRoIs"
      self.MuonRoIContainerWriteKey=""
    else:
      # read BS == write xAOD
      self.MuonRoIContainerReadKey=""
      self.MuonRoIContainerWriteKey="LVL1MuonRoIs"

def L1TriggerByteStreamDecoderCfg(flags):
  from TrigT1ResultByteStream.TrigT1ResultByteStreamConf import L1TriggerByteStreamDecoderAlg
  from TrigT1ResultByteStream.TrigT1ResultByteStreamConfig import ExampleL1TriggerByteStreamTool

  # Placeholder for real decoder tools - now it's just an example
  exampleTool = ExampleL1TriggerByteStreamTool(name="L1MuonBSDecoderTool", writeBS=False)
  decoderTools = [exampleTool]

  decoderAlg = L1TriggerByteStreamDecoderAlg(name="L1TriggerByteStreamDecoder",
                                             DecoderTools=decoderTools)

  from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
  acc = ComponentAccumulator()
  acc.addEventAlgo(decoderAlg)
  return acc

def L1TriggerByteStreamEncoderCfg(flags):
  from TrigT1ResultByteStream.TrigT1ResultByteStreamConfig import ExampleL1TriggerByteStreamTool
  exampleTool = ExampleL1TriggerByteStreamTool(name="L1MuonBSEncoderTool", writeBS=True)

  from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
  acc = ComponentAccumulator()
  acc.addPublicTool(exampleTool)
  return acc

def L1ByteStreamDecodersRecExSetup(enableRun2L1=True, enableRun3L1=True):
  # Use new-style config from the above functions and import into old-style JO
  from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  if enableRun2L1:
    CAtoGlobalWrapper(RoIBResultDecoderCfg,ConfigFlags)
  if enableRun3L1:
    CAtoGlobalWrapper(L1TriggerByteStreamDecoderCfg,ConfigFlags)

def L1ByteStreamEncodersRecExSetup():
  # Use new-style config from the above functions and import into old-style JO
  from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  CAtoGlobalWrapper(L1TriggerByteStreamEncoderCfg,ConfigFlags)
