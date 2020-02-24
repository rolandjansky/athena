#!/usr/bin/env python
#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

if __name__=='__main__':
  import sys
  from argparse import ArgumentParser
  parser = ArgumentParser()
  parser.add_argument('--disableHistograms', action='store_false', help='Turn off histograming')
  parser.add_argument('--disableGlobalGroups', action='store_false', help='Turn off global groups')
  parser.add_argument('--disableTriggerGroups', action='store_false', help='Turn off per-trigger groups')
  parser.add_argument('--disableExpressGroup', action='store_false', help='Turn off express stream rates')
  parser.add_argument('--disableUniqueRates', action='store_false', help='Turn off unique rates (much faster!)')
  parser.add_argument('--disableLumiExtrapolation', action='store_false', help='Turn off luminosity extrapolation')
  #
  parser.add_argument('--doRatesVsPositionInTrain', action='store_true', help='Study rates vs BCID position in bunch train')
  parser.add_argument('--vetoStartOfTrain', default=0, type=int, help='Number of BCIDs at the start of the train to veto, implies doRatesVsPositionInTrain')
  #
  parser.add_argument('--outputHist', default='RatesHistograms.root', type=str, help='Histogram output ROOT file')
  parser.add_argument('--inputPrescalesXML', default='', type=str, help='XML of prescales to simulate applying when computing rates')
  #
  parser.add_argument('--targetLuminosity', default=2e34, type=float)
  #
  parser.add_argument('--MCDatasetName', default='', type=str, help='For MC input: Name of the dataset, can be used instead of MCCrossSection, MCFilterEfficiency')
  parser.add_argument('--MCCrossSection', default=0.0, type=float, help='For MC input: Cross section of process in nb')
  parser.add_argument('--MCFilterEfficiency', default=1.0, type=float, help='For MC input: Filter efficiency of any MC filter (0.0 - 1.0)')
  parser.add_argument('--MCKFactor', default=1.0, type=float, help='For MC input: Additional multiplicitive fudge-factor to the supplied cross section.')
  parser.add_argument('--MCIgnoreGeneratorWeights', action='store_true', help='For MC input: Flag to disregard any generator weights.')
  #
  parser.add_argument('--maxEvents', type=int, help='Maximum number of events to process')
  parser.add_argument('--loglevel', type=int, default=3, help='Verbosity level')
  parser.add_argument('flags', nargs='*', help='Config flag overrides')
  args = parser.parse_args()

  # Setup the Run III behavior
  from AthenaCommon.Configurable import Configurable
  Configurable.configurableRun3Behavior = 1

  # Set the Athena configuration flags
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  from AthenaConfiguration.AutoConfigFlags import GetFileMD

  # Set the Athena configuration flags
  ConfigFlags.Input.Files = ["root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/rucio/data16_13TeV/8d/de/AOD.10654269._000566.pool.root.1"]
  ConfigFlags.fillFromArgs(args.flags)
  from PyUtils import AthFile
  af = AthFile.fopen(ConfigFlags.Input.Files[0]) 
  isMC = ('IS_SIMULATION' in af.fileinfos['evt_type'])
  runNumber = af.fileinfos['run_number'][0]

  ConfigFlags.Input.isMC = isMC
  useBunchCrossingTool = (args.doRatesVsPositionInTrain or args.vetoStartOfTrain > 0)

  ConfigFlags.lock()

  # Initialize configuration object, add accumulator, merge, and run.
  from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
  from AthenaConfiguration.ComponentFactory import CompFactory

  from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
  cfg = MainServicesSerialCfg()
  cfg.merge(PoolReadCfg(ConfigFlags))

  from GaudiSvc.GaudiSvcConf import THistSvc
  histSvc = THistSvc()
  histSvc.Output += ["RATESTREAM DATAFILE='" + args.outputHist + "' OPT='RECREATE'"]
  cfg.addService(histSvc)

  # Minimal config needed to read metadata: MetaDataSvc & ProxyProviderSvc
  from AthenaServices.MetaDataSvcConfig import MetaDataSvcCfg
  cfg.merge(MetaDataSvcCfg(ConfigFlags))

  cfgsvc = CompFactory.TrigConf__xAODConfigSvc('xAODConfigSvc')
  cfg.addService(cfgsvc)

  from TrigEDMConfig.TriggerEDM import EDMLibraries
  tdt = CompFactory.Trig__TrigDecisionTool('TrigDecisionTool')
  tdt.TrigConfigSvc = cfgsvc
  tdt.NavigationFormat = "TrigComposite"
  tdt.Navigation.Dlls = [e for e in  EDMLibraries if 'TPCnv' not in e]
  cfg.addPublicTool(tdt)

  # If the dataset name is in the input files path, then it will be fetched from there
  # Note to enable autolookup, first run "lsetup pyami; voms-proxy-init -voms atlas" and enter your grid pass phrase
  xsec = args.MCCrossSection
  fEff = args.MCFilterEfficiency
  dset = args.MCDatasetName
  if isMC and xsec == 0: # If the input file is MC then make sure we have the needed info
    from .RatesGetCrossSectionMC import GetCrossSectionAMI
    amiTool = GetCrossSectionAMI()
    if dset == "": # Can we get the dataset name from the input file path?
      dset = amiTool.getDatasetNameFromPath(ConfigFlags.Input.Files[0])
    amiTool.queryAmi(dset)
    xsec = amiTool.getCrossSection()
    fEff = amiTool.getFilterEfficiency()

  from EnhancedBiasWeighter.EnhancedBiasWeighterConf import EnhancedBiasWeighter
  ebw = EnhancedBiasWeighter('EnhancedBiasRatesTool')
  ebw.RunNumber = runNumber
  ebw.UseBunchCrossingTool = useBunchCrossingTool
  ebw.IsMC = isMC
  # The following three are only needed if isMC == true
  ebw.MCCrossSection = xsec
  ebw.MCFilterEfficiency = fEff
  ebw.MCKFactor = args.MCKFactor
  ebw.MCIgnoreGeneratorWeights = args.MCIgnoreGeneratorWeights
  cfg.addPublicTool(ebw)

  from RatesAnalysis.RatesAnalysisConf import FullMenu
  rates = FullMenu()
  rates.PrescaleXML = args.inputPrescalesXML
  rates.DoTriggerGroups = args.disableTriggerGroups
  rates.DoGlobalGroups = args.disableGlobalGroups
  rates.DoExpressRates = args.disableExpressGroup
  rates.DoUniqueRates = args.disableUniqueRates
  rates.DoHistograms = args.disableHistograms
  rates.UseBunchCrossingTool = useBunchCrossingTool
  rates.TargetLuminosity = args.targetLuminosity
  rates.VetoStartOfTrain = args.vetoStartOfTrain
  rates.EnableLumiExtrapolation = args.disableLumiExtrapolation
  rates.EnhancedBiasRatesTool = ebw
  rates.TrigDecisionTool = tdt
  rates.TrigConfigSvc = cfgsvc
  cfg.addEventAlgo(rates)

  # Setup for accessing bunchgroup data from the DB
  if useBunchCrossingTool:
    from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
    if isMC:
      cfg.addPublicTool(BunchCrossingTool("MC"))
    else:
      cfg.addPublicTool(BunchCrossingTool("LHC"))

  from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
  eventLoop = AthenaEventLoopMgr()
  eventLoop.EventPrintoutInterval = 1000
  cfg.addService(eventLoop)

  # If you want to turn on more detailed messages ...
  # exampleMonitorAcc.getEventAlgo('ExampleMonAlg').OutputLevel = 2 # DEBUG
  cfg.printConfig(withDetails=False) # set True for exhaustive info

  sc = cfg.run(args.maxEvents, args.loglevel)
  sys.exit(0 if sc.isSuccess() else 1)
