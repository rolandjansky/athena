#!/usr/bin/env python
#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

if __name__=='__main__':

  import argparse
  import sys,os


  parser = argparse.ArgumentParser(description = '', add_help = False)
  parser = argparse.ArgumentParser()

  #
  # File configuration
  #

  parser.add_argument('-i','--inputFiles', action='store', dest='inputFiles', required = False,
                      help = "Comma-separated list of input files (alias for --filesInput)")

  parser.add_argument('-o','--outputFile', action='store', dest='outputFile', required = False,
                      default = 'TrigEgammaMonitorOutput.root',
                      help = "Histogram output.")

  parser.add_argument('--nov','--numberOfEvents', action='store', dest='numberOfEvents', required = False, 
                      type=int, default=-1,
                      help = "The number of events to run.")

  #
  # Selector calib paths
  #

  parser.add_argument('--pidConfigPath', action='store', required = False, 
                      type=str, default='ElectronPhotonSelectorTools/trigger/rel22_20210611',
                      help = "Electron Likelihood and Photon CB config path to use in emulated precision step.")

  parser.add_argument('--dnnConfigPath', action='store', required = False, 
                      type=str, default='ElectronPhotonSelectorTools/offline/mc16_20210430',
                      help = "Electron DNN config path to use in emulated precision step.")

  parser.add_argument('--ringerConfigPath', action='store', required = False, 
                      type=str, default='RingerSelectorTools/TrigL2_20210702_r4',
                      help = "Electron ringer config path to use in emulated fast calo step.")



  parser.add_argument('--preExec', help='Code to execute before locking configs', default=None)
  parser.add_argument('--postExec', help='Code to execute after setup', default=None)
  parser.add_argument('--emulate', help='emulate the HLT e/g sequence', action='store_true')
  parser.add_argument('--debug', help='debug mode', action='store_true')


  if len(sys.argv)==1:
    parser.print_help()
    sys.exit(1)

  args = parser.parse_args()




  # ATR-11839 to fix the egammaPid import
  from PyUtils.Helpers import ROOT6Setup
  ROOT6Setup()
  # Setup the Run III behavior
  from AthenaCommon.Configurable import Configurable
  Configurable.configurableRun3Behavior = 1
  # Setup logs
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import INFO
  log.setLevel(INFO)


  # Set the Athena configuration flags
  from AthenaConfiguration.AllConfigFlags import ConfigFlags

  if args.inputFiles is not None:
    ConfigFlags.Input.Files = args.inputFiles.split(',')
  ConfigFlags.Input.isMC = True
  ConfigFlags.Output.HISTFileName = args.outputFile


  if args.preExec:
    # bring things into scope
    from AthenaMonitoring.DQConfigFlags import allSteeringFlagsOff
    exec(args.preExec)

  ConfigFlags.lock()


  # Initialize configuration object, add accumulator, merge, and run.
  from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
  from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
  cfg = MainServicesCfg(ConfigFlags)
  cfg.merge(PoolReadCfg(ConfigFlags))

  emulator=None
  if args.emulate:
    # create and configure emulator tool
    from TrigEgammaEmulationTool.TrigEgammaEmulationToolConfigMT import TrigEgammaEmulationToolConfig
    emulator = TrigEgammaEmulationToolConfig("EgammaEmulation") 

    # configure all selectors
    emulator.PhotonCBConfigFilePath    = args.pidConfigPath # Cut-based
    emulator.ElectronLHConfigFilePath  = args.pidConfigPath # LH
    emulator.ElectronCBConfigFilePath  = args.pidConfigPath # Cut-based
    emulator.ElectronDNNConfigFilePath = args.dnnConfigPath # DNN
    emulator.FastCaloConfigFilePath    = args.ringerConfigPath # NN
    emulator.configure()



  # create the e/g monitoring tool and add into the component accumulator
  from TrigEgammaMonitoring.TrigEgammaMonitorAlgorithm import TrigEgammaMonConfig
  trigEgammaMonitorAcc = TrigEgammaMonConfig(ConfigFlags, emulator=emulator)
  cfg.merge(trigEgammaMonitorAcc)


  # any last things to do?
  if args.postExec:
    exec(args.postExec)

  # If you want to turn on more detailed messages ...
  if args.debug:
    trigEgammaMonitorAcc.getEventAlgo('EgammaEmulation').OutputLevel = 2 # DEBUG



  cfg.printConfig(withDetails=False) # set True for exhaustive info
  sc = cfg.run(args.numberOfEvents) #use cfg

  sys.exit(0 if sc.isSuccess() else 1)

