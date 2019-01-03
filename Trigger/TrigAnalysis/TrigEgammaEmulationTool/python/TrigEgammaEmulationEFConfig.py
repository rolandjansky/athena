# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon                                                 import CfgMgr
from AthenaCommon.AppMgr                                          import ToolSvc
from egammaRec.Factories                                          import ToolFactory
#from TrigEgammaHypo.TrigEgammaPidTools                            import ElectronToolName
#from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf  import AsgElectronIsEMSelector
#from ElectronPhotonSelectorTools.ElectronIsEMSelectorMapping      import ElectronIsEMMap,electronPIDmenu
from TrigEgammaEmulationTool.TrigEgammaEmulationToolConfig        import OutputLevel


#***********************************************************************
# Check if we have all AsgSelectorTools inside of ToolSvc
from TrigEgammaHypo.TrigEgammaPidTools import ElectronPidTools
from TrigEgammaHypo.TrigEgammaPidTools import PhotonPidTools
ElectronPidTools()
PhotonPidTools()
#***********************************************************************


#TrkIsoCfg = CfgMgr.xAOD__TrackIsolationTool('TrigEgammaTrackIsolationTool')
#TrkIsoCfg.TrackSelectionTool.maxZ0SinTheta = 3.
#TrkIsoCfg.TrackSelectionTool.minPt = 1000.
#TrkIsoCfg.TrackSelectionTool.CutLevel = "Loose"

#**********************************************************************
# EFCalo
from TrigEgammaEmulationTool.TrigEgammaEmulationToolConf import Trig__TrigEgammaEFCaloSelectorTool
EgammaEFCaloDefaultEmulator = ToolFactory( Trig__TrigEgammaEFCaloSelectorTool,
                                           name                   = "TrigEgammaEFCaloEmulator",
                                           OutputLevel            = OutputLevel,
                                           ElectronCaloPPSelector = [ ToolSvc.AsgElectronIsEMTightCaloSelector,
                                                                      ToolSvc.AsgElectronIsEMMediumCaloSelector,
                                                                      ToolSvc.AsgElectronIsEMLooseCaloSelector,
                                                                      ToolSvc.AsgElectronIsEMVLooseCaloSelector],
                                           ElectronCaloLHSelector = [ ToolSvc.AsgElectronLHTightCaloSelector,
                                                                      ToolSvc.AsgElectronLHMediumCaloSelector,
                                                                      ToolSvc.AsgElectronLHLooseCaloSelector,
                                                                      ToolSvc.AsgElectronLHVLooseCaloSelector])

# EF Electron
from TrigEgammaEmulationTool.TrigEgammaEmulationIsolationConfig import createIsoToolElectronSelector
IsolationSelectors =  createIsoToolElectronSelector()

from TrigEgammaEmulationTool.TrigEgammaEmulationToolConf import Trig__TrigEgammaEFElectronSelectorTool
EgammaEFElectronDefaultEmulator = ToolFactory( Trig__TrigEgammaEFElectronSelectorTool,
                                               name                   = "TrigEgammaEFElectronDefaultEmulator",
                                               OutputLevel            = OutputLevel,
                                               ElectronOnlPPSelector  = [ ToolSvc.AsgElectronIsEMTightSelector,
                                                                          ToolSvc.AsgElectronIsEMMediumSelector,
                                                                          ToolSvc.AsgElectronIsEMLooseSelector,
                                                                          ToolSvc.AsgElectronIsEMVLooseSelector],
                                               ElectronOnlLHSelector  = [ ToolSvc.AsgElectronLHTightSelector,
                                                                          ToolSvc.AsgElectronLHMediumSelector,
                                                                          ToolSvc.AsgElectronLHLooseSelector,
                                                                          ToolSvc.AsgElectronLHVLooseSelector,
                                                                          ],
                                               IsolationSelector          = IsolationSelectors,
                                               )

# The following can be left as an example, please remove the remaining ones.
# THe emulator configuration should change if we change what we want to emulator
# i.e. the seletion for EF electrons. In this case, change the tools which are used
# Do not extend the emulator to do "everything"
EgammaEFElectronCutD0DphiDetaEmulator = EgammaEFElectronDefaultEmulator.copy(
                                        name                   = "TrigEgammaEFElectronCutD0DphiDetaEmulator",
                                        LikelihoodInfo         = "cutd0dphideta",
                                        OutputLevel            = OutputLevel,
                                        ElectronOnlLHSelector  = [ ToolSvc.AsgElectronLHTightCutD0DphiDetaSelector,
                                                                   ToolSvc.AsgElectronLHMediumCutD0DphiDetaSelector,
                                                                   ToolSvc.AsgElectronLHLooseCutD0DphiDetaSelector,
                                                                   ToolSvc.AsgElectronLHVeryLooseCutD0DphiDetaSelector,
                                                                   ])
EgammaEFElectronNoD0Emulator = EgammaEFElectronDefaultEmulator.copy(
                               name="TrigEgammaEFElectronNoD0Emulator",
                               LikelihoodInfo = "nod0",
                               OutputLevel            = OutputLevel,
                               ElectronOnlLHSelector  = [ ToolSvc.AsgElectronLHTightNoD0Selector,
                                                          ToolSvc.AsgElectronLHMediumNoD0Selector,
                                                          ToolSvc.AsgElectronLHLooseNoD0Selector,
                                                          ToolSvc.AsgElectronLHVeryLooseNoD0Selector,
                                                          ])
EgammaEFElectronNoDetaEmulator = EgammaEFElectronDefaultEmulator.copy(
                                 name="TrigEgammaEFElectronNoDetaEmulator",
                                 LikelihoodInfo = "deta",
                                 OutputLevel            = OutputLevel,
                                 ElectronOnlLHSelector  = [ ToolSvc.AsgElectronLHTightNoDetaSelector,
                                                            ToolSvc.AsgElectronLHMediumNoDetaSelector,
                                                            ToolSvc.AsgElectronLHLooseNoDetaSelector,
                                                            ToolSvc.AsgElectronLHVeryLooseNoDetaSelector,
                                                            ])
EgammaEFElectronNoDphiResEmulator = EgammaEFElectronDefaultEmulator.copy(
                                    name="TrigEgammaEFElectronNoDphiResEmulator",
                                    LikelihoodInfo = "nodphires",
                                    OutputLevel            = OutputLevel,
                                    ElectronOnlLHSelector  = [ ToolSvc.AsgElectronLHTightNoDphiResSelector,
                                                               ToolSvc.AsgElectronLHMediumNoDphiResSelector,
                                                               ToolSvc.AsgElectronLHLooseNoDphiResSelector,
                                                               ToolSvc.AsgElectronLHVeryLooseNoDphiResSelector,
                                                              ])
EgammaEFElectronSmoothEmulator = EgammaEFElectronDefaultEmulator.copy(
                                 name="TrigEgammaEFElectronSmoothEmulator",
                                 LikelihoodInfo = "smooth",
                                 OutputLevel            = OutputLevel,
                                 ElectronOnlLHSelector  = [ ToolSvc.AsgElectronLHTightSmoothSelector,
                                                            ToolSvc.AsgElectronLHMediumSelector,
                                                            ToolSvc.AsgElectronLHLooseSelector,
                                                            #ToolSvc.AsgElectronLHVeryLooseSelector,
                                                           ])

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



