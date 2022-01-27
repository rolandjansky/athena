#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator, CAtoGlobalWrapper
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import Format
from libpyeformat_helper import SourceIdentifier, SubDetector

#Muon RecRoiTools
from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConfig import getRun3RPCRecRoiTool
from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConfig import getRun3TGCRecRoiTool

#Muon trigger threshold tool
from TrigT1MuctpiPhase1.TrigT1MuctpiPhase1Config import getTrigThresholdDecisionTool

def RoIBResultByteStreamToolCfg(name, flags, writeBS=False):
  tool = CompFactory.RoIBResultByteStreamTool(name)

  if not flags.Trigger.L1.doCTP:
    # disable CTP ByteStream decoding/encoding as part of RoIBResult
    tool.CTPModuleId = 0xFF

  if flags.Trigger.enableL1MuonPhase1 or not flags.Trigger.L1.doMuon:
    # disable legacy MUCTPI ByteStream decoding/encoding as part of RoIBResult
    tool.MUCTPIModuleId = 0xFF

  if not flags.Trigger.enableL1CaloLegacy or not flags.Trigger.L1.doCalo:
    # disable legacy L1Calo ByteStream decoding/encoding as part of RoIBResult
    tool.JetModuleIds = []
    tool.EMModuleIds = []

  if flags.Trigger.EDMVersion == 1 or not flags.Trigger.L1.doTopo:
    # disable legacy L1Topo ByteStream decoding/encoding as part of RoIBResult
    tool.L1TopoModuleIds = []

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

def MuonRoIByteStreamToolCfg(name, flags, daq=False, writeBS=False):
  tool_name = name+"DAQ" if daq else name
  tool = CompFactory.MuonRoIByteStreamTool(tool_name)
  muctpi_moduleid = 0 if daq else 1
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
    tool.MuonRoIContainerWriteKey = "LVL1MuonRoIsDAQ" if daq else "LVL1MuonRoIs"

  tool.UseRun3Config = flags.Trigger.enableL1MuonPhase1
  tool.RPCRecRoiTool = getRun3RPCRecRoiTool(name="RPCRecRoiTool",useRun3Config=flags.Trigger.enableL1MuonPhase1)
  tool.TGCRecRoiTool = getRun3TGCRecRoiTool(name="TGCRecRoiTool",useRun3Config=flags.Trigger.enableL1MuonPhase1)
  tool.TrigThresholdDecisionTool = getTrigThresholdDecisionTool(name="TrigThresholdDecisionTool")

  return tool

def doRoIBResult(flags):
  '''
  Helper function returning a logic combination of flags deciding
  whether the RoIBResult decoding/encoding is required in the job
  '''
  if flags.Trigger.L1.doCalo and flags.Trigger.enableL1CaloLegacy:
    # Only needed for legacy (Run-2) L1Calo system
    return True
  if flags.Trigger.L1.doMuon and not flags.Trigger.enableL1MuonPhase1:
    # Only needed for legacy (Run-2) MUCTPI data
    return True
  if flags.Trigger.L1.doTopo:
    # Currently only RoIBResult path implemented for L1Topo
    return True
  if flags.Trigger.L1.doCTP:
    # Currently only RoIBResult path implemented for CTP
    return True
  # Otherwise don't need RoIBResult
  return False

def L1TriggerByteStreamDecoderCfg(flags):
  from AthenaCommon.Configurable import Configurable
  cb = Configurable.configurableRun3Behavior
  Configurable.configurableRun3Behavior += 1

  decoderTools = []
  maybeMissingRobs = []

  # Current status of L1 ByteStream decoding as of July 2021:
  # L1Muon - possible to run legacy path via RoIBResult or Run-3 path depending on flags
  # L1Calo - possible to run legacy system decoding via RoIBResult, Run-3 system decoding not yet implemented
  # L1Topo - possible to run legacy system decoding via RoIBResult, Run-3 system decoding not yet implemented
  # CTP - only decoding via RoIBResult implemented, xAOD EDM and decoding not yet implemented

  # Legacy decoding via RoIBResult
  if not flags.Trigger.doLVL1: #if we rerun L1, don't decode the original RoIBResult
    if doRoIBResult(flags):
      roibResultTool = RoIBResultByteStreamToolCfg(name="RoIBResultBSDecoderTool", flags=flags, writeBS=False)
      decoderTools += [roibResultTool]
      if flags.Trigger.EDMVersion == 2:
        # L1Topo may be missing in some runs of Run 2 when it was under commissioning
        for module_id in roibResultTool.L1TopoModuleIds:
          maybeMissingRobs.append(int(SourceIdentifier(SubDetector.TDAQ_CALO_TOPO_PROC, module_id)))
      if flags.Trigger.EDMVersion == 2 and not flags.Trigger.doHLT:
        # L1Calo occasional readout errors weren't caught by HLT in 2015 - ignore these in offline reco, see ATR-24493
        for module_id in roibResultTool.JetModuleIds:
          maybeMissingRobs.append(int(SourceIdentifier(SubDetector.TDAQ_CALO_JET_PROC_ROI, module_id)))
        for module_id in roibResultTool.EMModuleIds:
          maybeMissingRobs.append(int(SourceIdentifier(SubDetector.TDAQ_CALO_CLUSTER_PROC_ROI, module_id)))

  # Run-3 L1Muon decoding
  if flags.Trigger.enableL1MuonPhase1:
    muonRoiTool = MuonRoIByteStreamToolCfg(name="L1MuonBSDecoderTool",
                                           flags=flags,
                                           daq=(not flags.Trigger.doHLT),  # RoIB ROB for HLT, DAQ ROB for offline
                                           writeBS=False)
    decoderTools += [muonRoiTool]

  # TODO: Run-3 L1Calo, L1Topo, CTP

  decoderAlg = CompFactory.L1TriggerByteStreamDecoderAlg(name="L1TriggerByteStreamDecoder",
                                                         DecoderTools=decoderTools,
                                                         MaybeMissingROBs=maybeMissingRobs)

  acc = ComponentAccumulator()
  acc.addEventAlgo(decoderAlg, primary=True)

  # The decoderAlg needs to load ByteStreamMetadata for the detector mask
  from TriggerJobOpts.TriggerByteStreamConfig import ByteStreamReadCfg
  readBSAcc = ByteStreamReadCfg(flags)
  readBSAcc.getEventAlgo('SGInputLoader').Load += [
    ('ByteStreamMetadataContainer', 'InputMetaDataStore+ByteStreamMetadata')]
  acc.merge(readBSAcc)

  Configurable.configurableRun3Behavior = cb
  return acc

def L1TriggerByteStreamEncoderCfg(flags):
  acc = ComponentAccumulator()

  # Legacy encoding via RoIBResult
  if doRoIBResult(flags):
    roibResultTool = RoIBResultByteStreamToolCfg(name="RoIBResultBSEncoderTool", flags=flags, writeBS=True)
    acc.addPublicTool(roibResultTool)
    # Special - in BS->BS job without L1Sim, need to decode extra data from input
    # for encoding the CTP information back to BS
    if flags.Input.Format is Format.BS and not flags.Trigger.doLVL1 and roibResultTool.CTPModuleId != 0xFF:
      from TriggerJobOpts.TriggerByteStreamConfig import ByteStreamReadCfg
      acc.merge(ByteStreamReadCfg(flags, type_names=['CTP_RDO/CTP_RDO']))

  # Run-3 L1Muon encoding
  if flags.Trigger.L1.doMuon and flags.Trigger.enableL1MuonPhase1:
    # Write to both RoIB and DAQ ROBs
    for encode_daq in [False, True]:
      muonRoiTool = MuonRoIByteStreamToolCfg(name="L1MuonBSEncoderTool",
                                             flags=flags,
                                             daq=encode_daq,
                                             writeBS=True)
      acc.addPublicTool(muonRoiTool)

  # TODO: Run-3 L1Calo, L1Topo, CTP

  return acc

def L1ByteStreamDecodersRecExSetup():
  # Use new-style config from the above functions and import into old-style JO
  CAtoGlobalWrapper(L1TriggerByteStreamDecoderCfg,ConfigFlags)

def L1ByteStreamEncodersRecExSetup():
  # Use new-style config from the above functions and import into old-style JO
  CAtoGlobalWrapper(L1TriggerByteStreamEncoderCfg,ConfigFlags)
