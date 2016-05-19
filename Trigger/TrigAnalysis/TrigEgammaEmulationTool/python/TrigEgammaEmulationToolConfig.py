# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#********************************************************************************
# TrigEgammaEmulationTool python configuration
# Author: Joao Victor da Fonseca Pinto <jodafons@cern.ch>
# Contributor: Jorge Andres Lopez <jorge.lopez@cern.ch>
#********************************************************************************
def create_egamma_emulation_tool(trigList, **kw):

  OutputLevel = kw.pop('OutputLevel',0)
  from AthenaCommon.Logging import logging
  logger = logging.getLogger("TrigEgammaEmulationToolConfig")

  from AthenaCommon         import CfgMgr
  from AthenaCommon.AppMgr  import ToolSvc
  from egammaRec.Factories  import ToolFactory,FcnWrapper,AlgFactory, getPropertyValue
  import PyUtils.RootUtils as ru
  ROOT = ru.import_root()
  import PyCintex
  PyCintex.loadDictionary('ElectronPhotonSelectorToolsDict')
  from ROOT import LikeEnum
  from ROOT import egammaPID
  
  # Following loads the online selectors
  from TrigEgammaHypo.TrigEgammaPidTools                            import ElectronPidTools
  from TrigEgammaHypo.TrigEgammaPidTools                            import PhotonPidTools
  from TrigEgammaHypo.TrigEgammaPidTools                            import ElectronToolName
  from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf  import AsgElectronIsEMSelector
  from ElectronPhotonSelectorTools.ElectronIsEMSelectorMapping      import ElectronIsEMMap,electronPIDmenu
  
  ElectronPidTools()
  PhotonPidTools()
  
  # Offline selectors -- taken from latest conf  
  LooseElectronSelector = CfgMgr.AsgElectronIsEMSelector("LooseElectronSelector")
  MediumElectronSelector= CfgMgr.AsgElectronIsEMSelector("MediumElectronSelector")
  TightElectronSelector = CfgMgr.AsgElectronIsEMSelector("TightElectronSelector")
  LooseLHSelector       = CfgMgr.AsgElectronLikelihoodTool("LooseLHSelector")
  MediumLHSelector      = CfgMgr.AsgElectronLikelihoodTool("MediumLHSelector")
  TightLHSelector       = CfgMgr.AsgElectronLikelihoodTool("TightLHSelector")
  
  LooseElectronSelector.ConfigFile  = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMLooseSelectorCutDefs.conf"
  MediumElectronSelector.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMMediumSelectorCutDefs.conf"
  TightElectronSelector.ConfigFile  = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMTightSelectorCutDefs.conf"
  LooseLHSelector.ConfigFile        = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronLikelihoodLooseOfflineConfig2015.conf"
  MediumLHSelector.ConfigFile       = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronLikelihoodMediumOfflineConfig2015.conf"
  TightLHSelector.ConfigFile        = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronLikelihoodTightOfflineConfig2015.conf"
  
  ToolSvc+=LooseElectronSelector
  ToolSvc+=MediumElectronSelector
  ToolSvc+=TightElectronSelector
  ToolSvc+=LooseLHSelector
  ToolSvc+=MediumLHSelector
  ToolSvc+=TightLHSelector
 
  # Trigger online selectors -- for mc15_20160222 -- J. Lopez, 15.03.2016
  # TrigVLooseLHSelector = CfgMgr.AsgElectronLikelihoodTool("TrigVLooseLHSelector")
  TrigLooseLHSelector  = CfgMgr.AsgElectronLikelihoodTool("TrigLooseLHSelector")
  TrigMediumLHSelector = CfgMgr.AsgElectronLikelihoodTool("TrigMediumLHSelector")
  TrigTightLHSelector  = CfgMgr.AsgElectronLikelihoodTool("TrigTightLHSelector")
  # TrigVLooseLHSelector.ConfigFile = "ElectronPhotonSelectorTools/trigger/mc15_20160222/ElectronLikelihoodVeryLooseTriggerConfig2016.conf"
  TrigLooseLHSelector.ConfigFile  = "ElectronPhotonSelectorTools/trigger/mc15_20160222/ElectronLikelihoodLooseTriggerConfig2016.conf"
  TrigMediumLHSelector.ConfigFile = "ElectronPhotonSelectorTools/trigger/mc15_20160222/ElectronLikelihoodMediumTriggerConfig2016.conf"
  TrigTightLHSelector.ConfigFile  = "ElectronPhotonSelectorTools/trigger/mc15_20160222/ElectronLikelihoodTightTriggerConfig2016.conf"
  # ToolSvc += TrigVLooseLHSelector
  ToolSvc += TrigLooseLHSelector
  ToolSvc += TrigMediumLHSelector
  ToolSvc += TrigTightLHSelector
  
  # Track isolation -- remember to add TrackIsolation as a property of the class
  from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool,xAOD__TrackIsolationTool
  TrackIsolationTool = ToolFactory(xAOD__TrackIsolationTool, name = 'TrigEgammaTrackIsolationTool')
  TrkIsoCfg = CfgMgr.xAOD__TrackIsolationTool('TrigEgammaTrackIsolationTool')
  TrkIsoCfg.TrackSelectionTool.maxZ0SinTheta = 3.
  TrkIsoCfg.TrackSelectionTool.minPt = 1000.
  TrkIsoCfg.TrackSelectionTool.CutLevel = "Loose"

  #*****************************************************************************
  #from TrigEgammaMatchingTool.TrigEgammaMatchingToolConf import Trig__TrigEgammaMatchingTool
  #EgammaMatchTool = Trig__TrigEgammaMatchingTool("MatchingTool")
  #ToolSvc += EgammaMatchTool
    
  #*****************************************************************************
  from egammaMVACalib.egammaMVACalibConf import egammaMVATool
  mvatool = egammaMVATool("mvatool",folder="egammaMVACalib/v1")
  ToolSvc += mvatool
  
  #*****************************************************************************
  from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
  ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
  ToolSvc.TrigDecisionTool.TrigDecisionKey='xTrigDecision'

  #*****************************************************************************
  # L1Calo
  from TrigEgammaEmulationTool.TrigEgammaEmulationToolConf import Trig__TrigEgammaL1SelectorTool
  EgammaL1Emulator = ToolFactory( Trig__TrigEgammaL1SelectorTool,
                                  name="TrigEgammaL1Emulator",OutputLevel=OutputLevel)
  
  #*****************************************************************************
  # L2 staff

  from TrigEgammaEmulationTool.TrigEgammaEmulationToolConf import Trig__TrigEgammaL2SelectorTool
  from TrigEgammaEmulationTool.TrigEgammaL2EmulationFactory import TrigEgammaL2EmulationFactory

  L2Factory = TrigEgammaL2EmulationFactory( trigList, OutputLevel = OutputLevel).retrieve()
  
  EgammaL2Emulator = ToolFactory(Trig__TrigEgammaL2SelectorTool,
                                          name                   = "L2HelperEmulatorTool",
                                          OutputLevel            = OutputLevel,
                                          CaloSelectors          = L2Factory["Trig__TrigEgammaL2CaloSelectorTool"],
                                          CaloRingerSelectors    = L2Factory["Trig__TrigEgammaL2CaloRingerSelectorTool"],
                                          ElectronSelectors      = L2Factory["Trig__TrigEgammaL2ElectronSelectorTool"],
                                        )

  #*****************************************************************************
  # EFCalo
  from TrigEgammaEmulationTool.TrigEgammaEmulationToolConf import Trig__TrigEgammaEFCaloSelectorTool
  EgammaEFCaloEmulator = ToolFactory( Trig__TrigEgammaEFCaloSelectorTool,
                                      name="TrigEgammaEFCaloEmulator",
                                      OutputLevel=OutputLevel,
                                      ElectronCaloPPSelector = [ ToolSvc.AsgElectronIsEMTightCaloHypoSelector,
                                                                 ToolSvc.AsgElectronIsEMMediumCaloHypoSelector,
                                                                 ToolSvc.AsgElectronIsEMLooseCaloHypoSelector,
                                                                 ToolSvc.AsgElectronIsEMVLooseCaloHypoSelector],
                                      ElectronCaloLHSelector = [ ToolSvc.AsgElectronLHTightCaloSelector,
                                                                 ToolSvc.AsgElectronLHMediumCaloSelector,
                                                                 ToolSvc.AsgElectronLHLooseCaloSelector,
                                                                 ToolSvc.AsgElectronLHVLooseCaloSelector])

  #*****************************************************************************
  # EF Electron
  from TrigEgammaEmulationTool.TrigEgammaEmulationToolConf import Trig__TrigEgammaEFElectronSelectorTool
  EgammaEFElectronEmulator = ToolFactory( Trig__TrigEgammaEFElectronSelectorTool,
                                          name="TrigEgammaEFElectronEmulator",
                                          OutputLevel=OutputLevel,
                                          TrackIsolationTool     = TrackIsolationTool,
                                          ElectronOnlPPSelector  = [ ToolSvc.AsgElectronIsEMTightHypoSelector,
                                                                     ToolSvc.AsgElectronIsEMMediumHypoSelector,
                                                                     ToolSvc.AsgElectronIsEMLooseHypoSelector,
                                                                     ToolSvc.AsgElectronIsEMVLooseHypoSelector],
                                          ElectronOnlLHSelector  = [ ToolSvc.AsgElectronLHTightSelector,
                                                                     ToolSvc.AsgElectronLHMediumSelector,
                                                                     ToolSvc.AsgElectronLHLooseSelector,
                                                                     ToolSvc.AsgElectronLHVLooseSelector])

  #*****************************************************************************
  # EF Photon
  from TrigEgammaEmulationTool.TrigEgammaEmulationToolConf import Trig__TrigEgammaEFPhotonSelectorTool
  EgammaEFPhotonEmulator = ToolFactory( Trig__TrigEgammaEFPhotonSelectorTool,
                                        name="TrigEgammaEFPhotonEmulator",
                                        OutputLevel=OutputLevel,
                                        PhotonOnlPPSelector    = [ ToolSvc.AsgPhotonIsEMTightSelector,
                                                                   ToolSvc.AsgPhotonIsEMMediumSelector,
                                                                   ToolSvc.AsgPhotonIsEMLooseSelector,
                                                                   #ToolSvc.AsgPhotonIsEMVLooseHypoSelector
                                                                   ])
 
  #*****************************************************************************

  #Emulator tool
  from TrigEgammaEmulationTool.TrigEgammaEmulationToolConf import Trig__TrigEgammaEmulationTool
  logger.info("Creating TrigEgammaEmulationTool")
  emulator = ToolFactory( Trig__TrigEgammaEmulationTool, 
                          name = "TrigEgammaEmulationTool",
                          OutputLevel             = OutputLevel,
                          TrigList                = trigList,
                          L1SelectorTool          = EgammaL1Emulator(),
                          L2SelectorTool          = EgammaL2Emulator(),
                          EFCaloSelectorTool      = EgammaEFCaloEmulator(),
                          EFElectronSelectorTool  = EgammaEFElectronEmulator(),
                          EFPhotonSelectorTool    = EgammaEFPhotonEmulator()
                          )
                                            
  return emulator()



###############################################################################################

