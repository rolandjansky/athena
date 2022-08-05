# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from L1TopoSimulation.L1TopoSimulationConf import LVL1__L1TopoSimulation, LVL1__RoiB2TopoInputDataCnv
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import Format

class L1TopoSimulation ( LVL1__L1TopoSimulation ):

    def __init__( self, name = "L1TopoSimulation" ):
        super( L1TopoSimulation, self ).__init__( name )

        enableDebugOutput = False
        if enableDebugOutput:
            from AthenaCommon.Constants import DEBUG
            self.OutputLevel = DEBUG
            self.TopoOutputLevel = DEBUG
            self.TopoSteeringOutputLevel = DEBUG

class RoiB2TopoInputDataCnv ( LVL1__RoiB2TopoInputDataCnv ):

    def __init__( self, name = "RoiB2TopoInputDataCnv" ):
        super( RoiB2TopoInputDataCnv, self ).__init__( name )

def L1LegacyTopoSimulationCfg(flags):
    
    acc = ComponentAccumulator()
    
    emtauProvider = CompFactory.LVL1.EMTauInputProvider("EMTauInputProvider")

    topoSimAlg = CompFactory.LVL1.L1TopoSimulation("L1LegacyTopoSimulation",
                                                    EMTAUInputProvider = emtauProvider,
                                                    IsLegacyTopo = True,
                                                    InputDumpFile = "inputdump_legacy.txt",
                                                    EnableInputDump = flags.Trigger.enableL1TopoDump,
                                                    UseBitwise = flags.Trigger.enableL1TopoBWSimulation,
                                                    MonHistBaseDir = "L1/L1LegacyTopoAlgorithms",
                                                    FillHistoBasedOnHardware = (flags.Input.Format is Format.BS)
                                                   )

    # No muon inputs to legacy Topo
    topoSimAlg.MuonInputProvider.ROIBResultLocation = ""
    topoSimAlg.MuonInputProvider.MuonROILocation = ""
    topoSimAlg.MuonInputProvider.locationMuCTPItoL1Topo = ""
    topoSimAlg.MuonInputProvider.ROIBResultLocation = ""

    acc.addEventAlgo(topoSimAlg)
    return acc

def L1TopoSimulationCfg(flags):

    acc = ComponentAccumulator()

    #Configure the MuonInputProvider
    muProvider = CompFactory.LVL1.MuonInputProvider("MuonInputProvider",
                                                    ROIBResultLocation = "", #disable input from RoIBResult
                                                    MuonROILocation = "",
                                                    MuonEncoding = 1)

    #Configure the MuonRoiTools for the MIP
    from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConfig import getRun3RPCRecRoiTool, getRun3TGCRecRoiTool
    muProvider.RecRpcRoiTool = getRun3RPCRecRoiTool("RPCRecRoiTool", useRun3Config = True)
    muProvider.RecTgcRoiTool = getRun3TGCRecRoiTool("TGCRecRoiTool", useRun3Config = True)

    emtauProvider = CompFactory.LVL1.eFexInputProvider("eFexInputProvider")
    jetProvider = CompFactory.LVL1.jFexInputProvider("jFexInputProvider")
    energyProvider = CompFactory.LVL1.gFexInputProvider("gFexInputProvider")
    if not flags.Trigger.enableL1CaloPhase1:
        emtauProvider.eFexEMRoIKey = ""
        emtauProvider.eFexTauRoIKey = ""
        jetProvider.jFexSRJetRoIKey = ""
        jetProvider.jFexLRJetRoIKey = ""
        jetProvider.jFexEMRoIKey = ""
        jetProvider.jFexTauRoIKey = ""
        jetProvider.jFexXERoIKey = ""
        jetProvider.jFexTERoIKey = ""
        energyProvider.gFexSRJetRoIKey = ""
        energyProvider.gFexLRJetRoIKey = ""
        energyProvider.gFexXEJWOJRoIKey = ""
        energyProvider.gFexXENCRoIKey = ""
        energyProvider.gFexXERHORoIKey = ""
        energyProvider.gFexMHTRoIKey = ""
        energyProvider.gFexTERoIKey = ""

    topoSimAlg = CompFactory.LVL1.L1TopoSimulation("L1TopoSimulation",
                                                    MuonInputProvider = muProvider,
                                                    EMTAUInputProvider = emtauProvider,
                                                    JetInputProvider = jetProvider,
                                                    EnergyInputProvider = energyProvider,
                                                    IsLegacyTopo = False,
                                                    EnableInputDump = flags.Trigger.enableL1TopoDump,
                                                    UseBitwise = flags.Trigger.enableL1TopoBWSimulation,
                                                    FillHistoBasedOnHardware = (flags.Input.Format is Format.BS)
                                                    )

    acc.addEventAlgo(topoSimAlg)
    
    from L1TopoOnlineMonitoring import L1TopoOnlineMonitoringConfig as TopoMonConfig
    acc.addEventAlgo(TopoMonConfig.getL1TopoPhase1OnlineMonitor(flags,'L1/L1TopoSimDecisions'))
    
    return acc

def L1TopoSimulationOldStyleCfg(flags, isLegacy):
    from L1TopoSimulation.L1TopoSimulationConfig import L1TopoSimulation
    key = 'Legacy' if isLegacy else 'Phase1'
    topoSimSeq = L1TopoSimulation('L1'+key+'TopoSimulation')
    topoSimSeq.UseBitwise = False # Need to switch true (probably will change the counts)
    topoSimSeq.InputDumpFile = 'inputdump_' + key.lower() + '.txt'
    topoSimSeq.EnableInputDump = flags.Trigger.enableL1TopoDump
    topoSimSeq.IsLegacyTopo = isLegacy
    topoSimSeq.MonHistBaseDir = 'L1/L1'+key+'TopoAlgorithms'

    # Calo inputs
    if flags.Trigger.enableL1CaloPhase1 and not isLegacy:
        topoSimSeq.EMTAUInputProvider = 'LVL1::eFexInputProvider/eFexInputProvider'
        # Need further test from inputs.
        topoSimSeq.JetInputProvider = 'LVL1::jFexInputProvider/jFexInputProvider'
        # Need further test from inputs. Reverting back to Run 2 MET 
        topoSimSeq.EnergyInputProvider = 'LVL1::gFexInputProvider/gFexInputProvider'

    # Muon inputs only for phase-1 Topo
    if isLegacy:
        topoSimSeq.MuonInputProvider.ROIBResultLocation = ""
        topoSimSeq.MuonInputProvider.MuonROILocation = ""
        topoSimSeq.MuonInputProvider.locationMuCTPItoL1Topo = ""
        topoSimSeq.MuonInputProvider.ROIBResultLocation = ""
    else:
        if flags.Trigger.doLVL1:
            topoSimSeq.MuonInputProvider.ROIBResultLocation = "" #disable input from RoIBResult

        from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConfig import getRun3RPCRecRoiTool, getRun3TGCRecRoiTool
        topoSimSeq.MuonInputProvider.RecRpcRoiTool = getRun3RPCRecRoiTool(useRun3Config=True)
        topoSimSeq.MuonInputProvider.RecTgcRoiTool = getRun3TGCRecRoiTool(useRun3Config=True)
        topoSimSeq.MuonInputProvider.MuonROILocation = ""
        topoSimSeq.MuonInputProvider.MuonEncoding = 1

    return topoSimSeq

def L1TopoSimulationStandaloneCfg(flags, outputEDM=[]):

    acc = ComponentAccumulator()

    efex_provider_attr = ['eFexEMRoI','eFexTauRoI']
    jfex_provider_attr = ['jFexSRJetRoI','jFexLRJetRoI','jFexEMRoI','jFexTauRoI','jFexXERoI','jFexTERoI']
    gfex_provider_attr = ['gFexSRJetRoI','gFexLRJetRoI','gFexXEJWOJRoI','gFexXENCRoI','gFexXERHORoI','gFexMHTRoI','gFexTERoI']
   
    #Configure the MuonInputProvider
    muProvider = CompFactory.LVL1.MuonInputProvider("MuonInputProvider",
                                                    ROIBResultLocation = "", #disable input from RoIBResult
                                                    MuonROILocation = "",
                                                    MuonEncoding = 1)

    #Configure the MuonRoiTools for the MIP
    from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConfig import getRun3RPCRecRoiTool, getRun3TGCRecRoiTool
    muProvider.RecRpcRoiTool = getRun3RPCRecRoiTool("RPCRecRoiTool", useRun3Config = True)
    muProvider.RecTgcRoiTool = getRun3TGCRecRoiTool("TGCRecRoiTool", useRun3Config = True)


    efexProvider = CompFactory.LVL1.eFexInputProvider("eFexInputProvider")
    jfexProvider = CompFactory.LVL1.jFexInputProvider("jFexInputProvider")
    gfexProvider = CompFactory.LVL1.gFexInputProvider("gFexInputProvider")

    for attr in efex_provider_attr:
        res = [x for x in outputEDM if attr in x]
        if len(res)>0:
            key = res[0].split('#')[1]
            print (f'Key found for eFEX: {key}')
            setattr(efexProvider,attr+'Key',key)
        else:
            setattr(efexProvider,attr+'Key','')

    for attr in jfex_provider_attr:
        res = [x for x in outputEDM if attr in x]
        if len(res)>0:
            key = res[0].split('#')[1]
            print (f'Key found for jFEX: {key}')
            setattr(jfexProvider,attr+'Key',key)
        else:
            setattr(jfexProvider,attr+'Key','')

    for attr in gfex_provider_attr:
        res = [x for x in outputEDM if attr in x]
        if len(res)>0:
            key = res[0].split('#')[1]
            print (f'Key found for gFEX: {key}')
            setattr(gfexProvider,attr+'Key',key)
        else:
            setattr(gfexProvider,attr+'Key','')

    topoSimAlg = CompFactory.LVL1.L1TopoSimulation("L1TopoSimulation",
                                                    MuonInputProvider = "",
                                                    EMTAUInputProvider = efexProvider,
                                                    JetInputProvider = jfexProvider,
                                                    EnergyInputProvider = gfexProvider,
                                                    IsLegacyTopo = False,
                                                    EnableInputDump = True,
                                                    UseBitwise = True,
                                                    FillHistoBasedOnHardware = False
                                                    )

    acc.addEventAlgo(topoSimAlg)
    
    return acc


if __name__ == '__main__':
  from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
  from AthenaCommon.Logging import logging
  from AthenaCommon.Constants import VERBOSE,DEBUG,WARNING
  import argparse
  from argparse import RawTextHelpFormatter
  import sys

  log = logging.getLogger('runL1TopoSim')
  log.setLevel(DEBUG)
  algLogLevel = DEBUG

  parser = argparse.ArgumentParser("Running L1TopoSimulation standalone for the BS input", formatter_class=RawTextHelpFormatter)
  parser.add_argument("-i","--inputs",nargs='*',action="store", dest="inputs", help="Inputs will be used in commands", required=True)
  parser.add_argument("-m","--module",action="store", dest="module", help="Input modules wants to be simulated.",default="", required=False)
  parser.add_argument("-l","--logLevel",action="store", dest="log", help="Log level.",default="warning", required=False)
  parser.add_argument("-n","--nevent", type=int, action="store", dest="nevent", help="Maximum number of events will be executed.",default=0, required=False)
  parser.add_argument("-s","--skipEvents", type=int, action="store", dest="skipEvents", help="How many events will be skipped.",default=0, required=False)
  
  args = parser.parse_args()

  supportedSubsystems = ['Muons','jFex','eFex','gFex','Topo']
  args_subsystem = args.module.split(',')
  subsystem = list( set(args_subsystem) & set(supportedSubsystems) )
  filename = args.inputs

  if len(subsystem)==0:
      log.warning(f'subsystem not given or the given subsystem not supported with one of the: {supportedSubsystems}')


  
  if args.log == 'warning': algLogLevel = WARNING
  if args.log == 'debug': algLogLevel = DEBUG
  if args.log == 'verbose': algLogLevel = VERBOSE
  
  if "data22" in filename:
    flags.Trigger.triggerConfig='DB'
  flags.Exec.OutputLevel = WARNING
  if(args.nevent > 0):
    flags.Exec.MaxEvents = args.nevent
  flags.Trigger.triggerMenuSetup = 'PhysicsP1_pp_run3_v1'
  flags.Input.Files = args.inputs
  flags.Concurrency.NumThreads = 1
  flags.Concurrency.NumConcurrentEvents = 1
  flags.Exec.SkipEvents = args.skipEvents
  flags.Output.AODFileName = 'AOD.pool.root'
  flags.GeoModel.AtlasVersion = 'ATLAS-R3S-2021-01-00-02'
  flags.IOVDb.GlobalTag = 'CONDBR2-BLKPA-2018-13'
  flags.Muon.enableAlignment = True
  flags.lock()

  from AthenaConfiguration.MainServicesConfig import MainServicesCfg
  acc = MainServicesCfg(flags)

  from TriggerJobOpts.TriggerByteStreamConfig import ByteStreamReadCfg
  acc.merge(ByteStreamReadCfg(flags, type_names=['CTP_RDO/CTP_RDO']))

  # Generate run3 L1 menu
  from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg,generateL1Menu
  acc.merge(L1ConfigSvcCfg(flags))
  if "data22" not in filename:   
    generateL1Menu(flags)
  
  # Produce xAOD L1 RoIs from RoIBResult
  from AnalysisTriggerAlgs.AnalysisTriggerAlgsCAConfig import RoIBResultToxAODCfg
  xRoIBResultAcc, xRoIBResultOutputs = RoIBResultToxAODCfg(flags)
  from TrigT1ResultByteStream.TrigT1ResultByteStreamConfig import L1TriggerByteStreamDecoderCfg
  acc.merge(L1TriggerByteStreamDecoderCfg(flags))
  acc.merge(xRoIBResultAcc)  
  
  decoderTools = []
  outputEDM = []
  def addEDM(edmType, edmName):
    auxType = edmType.replace('Container','AuxContainer')
    return [f'{edmType}#{edmName}',
            f'{auxType}#{edmName}Aux.']

  outputEDM += ['CTP_RDO#*']
  outputEDM += ['ROIB::RoIBResult#*']

  outputEDM += addEDM('xAOD::JetEtRoI'         , 'LVL1JetEtRoI')
  outputEDM += addEDM('xAOD::JetRoIContainer'  , 'LVL1JetRoIs')
  outputEDM += addEDM('xAOD::EmTauRoIContainer', 'LVL1EmTauRoIs')
  outputEDM += addEDM('xAOD::EnergySumRoI'     , 'LVL1EnergySumRoI')

  loadFromSG = [('xAOD::EventInfo', 'StoreGateSvc+EventInfo')]
 
  if 'Muons' in subsystem:
      loadFromSG += [( 'RpcPadContainer' , 'StoreGateSvc+RPCPAD_L1' ),
                     ( 'TgcRdoContainer' , 'StoreGateSvc+TGCRDO_L1' )]
      from MuonConfig.MuonCablingConfig import MuonCablingConfigCfg
      acc.merge(MuonCablingConfigCfg(flags))
      
      from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
      acc.merge(MuonGeoModelCfg(flags))

      from TrigT1ResultByteStream.TrigT1ResultByteStreamConfig import MuonRoIByteStreamToolCfg
      muonRoiTool = MuonRoIByteStreamToolCfg(name="L1MuonBSDecoderTool",flags=flags,writeBS=False)
      decoderTools += [muonRoiTool]

      from TriggerJobOpts.Lvl1MuonSimulationConfig import MuonBytestream2RdoConfig
      acc.merge(MuonBytestream2RdoConfig(flags))

  if 'jFex' in subsystem:
      from L1CaloFEXByteStream.L1CaloFEXByteStreamConfig import jFexRoiByteStreamToolCfg
      jFexTool = jFexRoiByteStreamToolCfg('jFexBSDecoder', flags, writeBS=False)
      decoderTools += [jFexTool]
      outputEDM += addEDM('xAOD::jFexSRJetRoIContainer', jFexTool.jJRoIContainerWriteKey.Path)
      outputEDM += addEDM('xAOD::jFexLRJetRoIContainer', jFexTool.jLJRoIContainerWriteKey.Path)
      outputEDM += addEDM('xAOD::jFexTauRoIContainer'  , jFexTool.jTauRoIContainerWriteKey.Path)
      outputEDM += addEDM('xAOD::jFexFwdElRoIContainer', jFexTool.jEMRoIContainerWriteKey.Path)
      outputEDM += addEDM('xAOD::jFexSumETRoIContainer', jFexTool.jTERoIContainerWriteKey.Path)
      outputEDM += addEDM('xAOD::jFexMETRoIContainer'  , jFexTool.jXERoIContainerWriteKey.Path)
  #
  if 'eFex' in subsystem:
      from L1CaloFEXByteStream.L1CaloFEXByteStreamConfig import eFexByteStreamToolCfg
      eFexTool = eFexByteStreamToolCfg('eFexBSDecoder', flags, writeBS=False)
      decoderTools += [eFexTool]
      outputEDM += addEDM('xAOD::eFexEMRoIContainer', eFexTool.eEMContainerWriteKey.Path)
      outputEDM += addEDM('xAOD::eFexTauRoIContainer', eFexTool.eTAUContainerWriteKey.Path)

  if 'Topo' in subsystem:
      from L1TopoByteStream.L1TopoByteStreamConfig import L1TopoPhase1ByteStreamToolCfg
      l1topoBSTool = L1TopoPhase1ByteStreamToolCfg("L1TopoBSDecoderTool",flags)
      decoderTools += [l1topoBSTool]
      outputEDM += addEDM('xAOD::L1TopoRawDataContainer', l1topoBSTool.L1TopoPhase1RAWDataWriteContainer.Path)

  
  decoderAlg = CompFactory.L1TriggerByteStreamDecoderAlg(name="L1TriggerByteStreamDecoder",
                                                         DecoderTools=decoderTools, OutputLevel=algLogLevel)

  
  acc.addEventAlgo(decoderAlg, sequenceName='AthAlgSeq')
  
  acc.addEventAlgo(CompFactory.SGInputLoader(Load=loadFromSG), sequenceName="AthAlgSeq")

  roib2topo = CompFactory.LVL1.RoiB2TopoInputDataCnv(name='RoiB2TopoInputDataCnv')
  roib2topo.OutputLevel = algLogLevel
  acc.addEventAlgo(roib2topo, sequenceName="AthAlgSeq")
  from L1TopoByteStream.L1TopoByteStreamConfig import L1TopoByteStreamCfg
  acc.merge(L1TopoByteStreamCfg(flags), sequenceName='AthAlgSeq')
  outputEDM += addEDM('xAOD::L1TopoRawDataContainer', 'L1TopoRawData')
  acc.merge(L1LegacyTopoSimulationCfg(flags), sequenceName='AthAlgSeq')

  acc.merge(L1TopoSimulationStandaloneCfg(flags,outputEDM), sequenceName='AthAlgSeq')
  outputEDM += ['xAOD::L1TopoSimResultsContainer#L1_TopoSimResults']

  # phase1 mon
  from L1TopoOnlineMonitoring import L1TopoOnlineMonitoringConfig as TopoMonConfig
  acc.addEventAlgo(
      TopoMonConfig.getL1TopoPhase1OnlineMonitor(flags,'L1/L1TopoOffline',True,True,False,True,algLogLevel),
      sequenceName="AthAlgSeq"
  )
  # legacy mon
  acc.addEventAlgo(TopoMonConfig.getL1TopoLegacyOnlineMonitor(flags,'L1/L1LegacyTopoOffline',algLogLevel),
                   sequenceName="AthAlgSeq")


  from GaudiSvc.GaudiSvcConf import THistSvc # noqa: F401
  histSvc = CompFactory.THistSvc(Output = ["EXPERT DATAFILE='expert-monitoring-l1topo.root', OPT='RECREATE'"])
  acc.addService(histSvc)

  from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
  log.debug('Adding the following output EDM to ItemList: %s', outputEDM)
  acc.merge(OutputStreamCfg(flags, 'AOD', ItemList=outputEDM))

  if args.log == 'verbose':
      acc.printConfig(withDetails=True, summariseProps=True, printDefaults=True)
  
  if acc.run().isFailure():
    sys.exit(1)
