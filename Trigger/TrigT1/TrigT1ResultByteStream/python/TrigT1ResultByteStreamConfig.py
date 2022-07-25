#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import Format
from TrigEDMConfig.TriggerEDMRun3 import recordable
from libpyeformat_helper import SourceIdentifier, SubDetector

from L1CaloFEXByteStream.L1CaloFEXByteStreamConfig import eFexByteStreamToolCfg, jFexRoiByteStreamToolCfg, jFexInputByteStreamToolCfg
from L1TopoByteStream.L1TopoByteStreamConfig import L1TopoPhase1ByteStreamToolCfg
from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConfig import getRun3RPCRecRoiTool
from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConfig import getRun3TGCRecRoiTool
from TrigT1MuctpiPhase1.TrigT1MuctpiPhase1Config import getTrigThresholdDecisionTool

# Phase-1 L1Calo/L1Topo converters

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
  muctpi_moduleid = 0
  muctpi_robid = int(SourceIdentifier(SubDetector.TDAQ_MUON_CTP_INTERFACE, muctpi_moduleid))
  tool.ROBIDs = [muctpi_robid]
  if writeBS:
    # write BS == read xAOD
    tool.MuonRoIContainerReadKey="LVL1MuonRoIs"
    tool.MuonRoIContainerWriteKey=""
  else:
    # read BS == write xAOD
    tool.MuonRoIContainerReadKey=""
    tool.MuonRoIContainerWriteKey=recordable("LVL1MuonRoIs")
  return tool

def MuonRoIByteStreamToolCfg(name, flags, writeBS=False):
  tool = CompFactory.MuonRoIByteStreamTool(name)
  muctpi_moduleid = 0  # No RoIB in Run 3, we always read the DAQ ROB
  muctpi_robid = int(SourceIdentifier(SubDetector.TDAQ_MUON_CTP_INTERFACE, muctpi_moduleid)) # 0x760000
  tool.ROBIDs = [muctpi_robid]

  from TrigT1ResultByteStream.TrigT1ResultByteStreamMonitoring import L1MuonBSConverterMonitoring
  tool.MonTool = L1MuonBSConverterMonitoring(name, writeBS)

  # Build container names for each bunch crossing in the maximum readout window (size 5)
  containerBaseName = "LVL1MuonRoIs"
  containerNames = [
    containerBaseName + "BCm2",
    containerBaseName + "BCm1",
    containerBaseName,
    containerBaseName + "BCp1",
    containerBaseName + "BCp2",
  ]

  if writeBS:
    # write BS == read xAOD
    tool.MuonRoIContainerReadKeys += containerNames
  else:
    # read BS == write xAOD
    tool.MuonRoIContainerWriteKeys += [recordable(c) for c in containerNames]

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

  decoderTools = []
  maybeMissingRobs = []

  ########################################
  # Legacy decoding via RoIBResult
  ########################################
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

  ########################################
  # Run-3 L1Muon decoding (only when running HLT - offline we read it from HLT result)
  ########################################
  if flags.Trigger.L1.doMuon and flags.Trigger.enableL1MuonPhase1 and flags.Trigger.doHLT:
    muonRoiTool = MuonRoIByteStreamToolCfg(name="L1MuonBSDecoderTool",
                                           flags=flags,
                                           writeBS=False)
    decoderTools += [muonRoiTool]

  ########################################
  # Run-3 L1Calo decoding
  ########################################
  if flags.Trigger.L1.doCalo and flags.Trigger.enableL1CaloPhase1 and flags.Trigger.doHLT:
    eFexByteStreamTool = eFexByteStreamToolCfg("eFexBSDecoderTool",
                                               flags=flags,
                                               writeBS=False,
                                               multiSlice=False)
    jFexRoiByteStreamTool = jFexRoiByteStreamToolCfg("jFexBSDecoderTool",
                                               flags=flags,
                                               writeBS=False)
    decoderTools += [eFexByteStreamTool, jFexRoiByteStreamTool]
    # During commissioning of the phase-1 L1Calo (2022), allow the data to be missing
    maybeMissingRobs += eFexByteStreamTool.ROBIDs
    maybeMissingRobs += jFexRoiByteStreamTool.ROBIDs

  ########################################
  # Run-3 L1Topo decoding
  ########################################
  if flags.Trigger.L1.doTopo and flags.Trigger.enableL1CaloPhase1 and flags.Trigger.doHLT:
    topoByteStreamTool = L1TopoPhase1ByteStreamToolCfg("L1TopoBSDecoderTool",
                                                       flags=flags,
                                                       writeBS=False)
    decoderTools += [topoByteStreamTool]
    # During commissioning of the phase-1 L1Topo (2022), allow the data to be missing
    maybeMissingRobs += topoByteStreamTool.ROBIDs

  # TODO: gFex, CTP

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
    muonRoiTool = MuonRoIByteStreamToolCfg(name="L1MuonBSEncoderTool",
                                           flags=flags,
                                           writeBS=True)
    acc.addPublicTool(muonRoiTool)

  # TODO: Run-3 L1Calo, L1Topo, CTP

  return acc


if __name__ == '__main__':
  from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
  from AthenaCommon.Logging import logging
  from AthenaCommon.Constants import VERBOSE,DEBUG,WARNING
  import sys

  log = logging.getLogger('TrigT1ResultByteStreamConfig')
  log.setLevel(DEBUG)

  algLogLevel = DEBUG

  if len(sys.argv) < 4:
    log.error('usage: python -m TrigT1ResultByteStream.TrigT1ResultByteStreamConfig subsystem file nevents')
    sys.exit(1)
  supportedSubsystems = ['inputjFex','jFex','eFex', 'allFex','Topo']
  subsystem = sys.argv[1]
  filename = sys.argv[2]
  events = int(sys.argv[3])

  if len(sys.argv) > 4 and sys.argv[4] == 'verbose': algLogLevel = VERBOSE

  if subsystem not in supportedSubsystems:
    log.error(f'subsystem "{subsystem}" not one of supported subsystems: {supportedSubsystems}')
    sys.exit(1)

  if "data22" in filename:
    flags.Trigger.triggerConfig='DB'

  flags.Exec.OutputLevel = WARNING
  if(events > 0):
    flags.Exec.MaxEvents = events

  flags.Input.Files = [filename]
  flags.Concurrency.NumThreads = 1
  flags.Concurrency.NumConcurrentEvents = 1
  flags.Output.AODFileName = 'AOD.pool.root'
  flags.lock()

  from AthenaConfiguration.MainServicesConfig import MainServicesCfg
  acc = MainServicesCfg(flags)

  from TriggerJobOpts.TriggerByteStreamConfig import ByteStreamReadCfg
  acc.merge(ByteStreamReadCfg(flags))

  # Generate run3 L1 menu
  from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg,generateL1Menu
  acc.merge(L1ConfigSvcCfg(flags))

  if "data22" not in filename:
    generateL1Menu(flags)
  
  # Produce xAOD L1 RoIs from RoIBResult, commented for now in the testing process
  # from AnalysisTriggerAlgs.AnalysisTriggerAlgsCAConfig import RoIBResultToxAODCfg
  # xRoIBResultAcc, xRoIBResultOutputs = RoIBResultToxAODCfg(flags)
  # acc.merge(L1TriggerByteStreamDecoderCfg(flags))
  # acc.merge(xRoIBResultAcc)  

  decoderTools = []
  outputEDM = []
  maybeMissingRobs = []

  def addEDM(edmType, edmName):
    auxType = edmType.replace('Container','AuxContainer')
    return [f'{edmType}#{edmName}',
            f'{auxType}#{edmName}Aux.']

  outputEDM += addEDM('xAOD::JetEtRoI'         , 'LVL1JetEtRoI')
  outputEDM += addEDM('xAOD::JetRoIContainer'  , 'LVL1JetRoIs')
  outputEDM += addEDM('xAOD::EmTauRoIContainer', 'LVL1EmTauRoIs')
  outputEDM += addEDM('xAOD::EnergySumRoI'     , 'LVL1EnergySumRoI')

  if subsystem in ['jFex','allFex'] :
    jFexTool = jFexRoiByteStreamToolCfg('jFexBSDecoder', flags)
    for module_id in jFexTool.ROBIDs:
        maybeMissingRobs.append(module_id)
        
    decoderTools += [jFexTool]
    outputEDM += addEDM('xAOD::jFexSRJetRoIContainer', jFexTool.jJRoIContainerWriteKey.Path)
    outputEDM += addEDM('xAOD::jFexLRJetRoIContainer', jFexTool.jLJRoIContainerWriteKey.Path)
    outputEDM += addEDM('xAOD::jFexTauRoIContainer'  , jFexTool.jTauRoIContainerWriteKey.Path)
    outputEDM += addEDM('xAOD::jFexFwdElRoIContainer', jFexTool.jEMRoIContainerWriteKey.Path)
    outputEDM += addEDM('xAOD::jFexSumETRoIContainer', jFexTool.jTERoIContainerWriteKey.Path)
    outputEDM += addEDM('xAOD::jFexMETRoIContainer'  , jFexTool.jXERoIContainerWriteKey.Path)

  if subsystem in ['inputjFex'] :
    inputjFexTool = jFexInputByteStreamToolCfg('jFexInputBSDecoder', flags)
    for module_id in inputjFexTool.ROBIDs:
        maybeMissingRobs.append(module_id) 
        
    decoderTools += [inputjFexTool]
    # example if saving/adding the xAOD container, implement in the future
    # outputEDM += addEDM('xAOD::jFexSRJetRoIContainer', jFexTool.jJRoIContainerWriteKey.Path)

  if subsystem in ['eFex','allFex'] :
    eFexTool = eFexByteStreamToolCfg('eFexBSDecoder', flags)
    eFexTool_xTOBs = eFexByteStreamToolCfg('eFexBSDecoder_xTOBs', flags,xTOBs=True)
    decoderTools += [eFexTool,eFexTool_xTOBs]

    for module_id in eFexTool.ROBIDs:
        maybeMissingRobs.append(module_id)

    # TOB containers
    outputEDM += addEDM('xAOD::eFexEMRoIContainer', eFexTool.eEMContainerWriteKey.Path)
    outputEDM += addEDM('xAOD::eFexTauRoIContainer', eFexTool.eTAUContainerWriteKey.Path)
    # xTOB containers
    outputEDM += addEDM('xAOD::eFexEMRoIContainer', eFexTool_xTOBs.eEMContainerWriteKey.Path)
    outputEDM += addEDM('xAOD::eFexTauRoIContainer', eFexTool_xTOBs.eTAUContainerWriteKey.Path)

  if subsystem in 'Topo':
    l1topoBSTool = L1TopoPhase1ByteStreamToolCfg()
    decoderTools += [l1topoBSTool]
    outputEDM += addEDM('xAOD::L1TopoRawDataContainer', l1topoBSTool.L1TopoPhase1RAWDataWriteContainer.Path)

  decoderAlg = CompFactory.L1TriggerByteStreamDecoderAlg(name="L1TriggerByteStreamDecoder",
                                                         DecoderTools=decoderTools, OutputLevel=algLogLevel, MaybeMissingROBs=maybeMissingRobs)
                                                         
  acc.addEventAlgo(decoderAlg, sequenceName='AthAlgSeq')

  from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
  log.debug('Adding the following output EDM to ItemList: %s', outputEDM)
  acc.merge(OutputStreamCfg(flags, 'AOD', ItemList=outputEDM))

  if acc.run().isFailure():
    sys.exit(1)
