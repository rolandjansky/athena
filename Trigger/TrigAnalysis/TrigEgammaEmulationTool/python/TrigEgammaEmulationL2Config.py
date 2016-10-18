# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration




from egammaRec.Factories  import ToolFactory,FcnWrapper,AlgFactory, getPropertyValue
from TrigEgammaEmulationTool.TrigEgammaEmulationToolConf import Trig__TrigEgammaL2CaloSelectorTool
from TrigEgammaEmulationTool.TrigEgammaEmulationToolConf import Trig__TrigEgammaL2ElectronSelectorTool
from AthenaCommon.SystemOfUnits                          import GeV
from TrigEgammaEmulationTool.TrigEgammaEmulationToolConfig import OutputLevel
# L2 Calo step configurations
# Only Up to 22 GeV
from TrigEgammaHypo.TrigL2CaloHypoCutDefs                import L2CaloCutMaps
# define eta bins
EtaBins    = [0.00, 0.60, 0.80, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
theCutMaps = L2CaloCutMaps(22)
# create L2Calo tool    
EgammaL2CaloLooseEmulator = ToolFactory(Trig__TrigEgammaL2CaloSelectorTool, 
                                        OutputLevel    = OutputLevel,
                                        name           = "TrigEgammaL2CaloLooseEmulator", 
                                        HADETthr       = theCutMaps.MapsHADETthr["loose"],
                                        CARCOREthr     = theCutMaps.MapsCARCOREthr["loose"],
                                        CAERATIOthr    = theCutMaps.MapsCAERATIOthr["loose"],
                                        EtaBins        = EtaBins ,
                                        dETACLUSTERthr = 0.1,
                                        dPHICLUSTERthr = 0.1,
                                        ET2thr         = 90.0*GeV,
                                        F1thr          = 0.005)

#********************************************************************************
# VeryLoose
EgammaL2CaloVeryLooseEmulator = EgammaL2CaloLooseEmulator.copy(
                                name        = "TrigEgammaL2CaloVeryLooseEmulator",
                                OutputLevel       = OutputLevel,
                                HADETthr    = theCutMaps.MapsHADETthr['vloose'],
                                CARCOREthr  = theCutMaps.MapsCARCOREthr['vloose'],
                                CAERATIOthr = theCutMaps.MapsCAERATIOthr['vloose'])

#********************************************************************************
# Medium
EgammaL2CaloMediumEmulator = EgammaL2CaloLooseEmulator.copy(
                             name        = "TrigEgammaL2CaloMediumEmulator",
                             OutputLevel = OutputLevel,
                             HADETthr    = theCutMaps.MapsHADETthr['medium'],
                             CARCOREthr  = theCutMaps.MapsCARCOREthr['medium'],
                             CAERATIOthr = theCutMaps.MapsCAERATIOthr['medium'])

#********************************************************************************
# Tight
EgammaL2CaloTightEmulator = EgammaL2CaloLooseEmulator.copy(
                            name        = "TrigEgammaL2CaloTightEmulator",
                            OutputLevel = OutputLevel,
                            HADETthr    = theCutMaps.MapsHADETthr['tight'],
                            CARCOREthr  = theCutMaps.MapsCARCOREthr['tight'],
                            CAERATIOthr = theCutMaps.MapsCAERATIOthr['tight'])


#********************************************************************************
# L2 Electron configurations
EgammaL2ElectronEmulator  = ToolFactory(Trig__TrigEgammaL2ElectronSelectorTool,
                                        name                     = "TrigEgammaL2ElectronEmulator",
                                        EtaBins                  = EtaBins,
                                        EtaBinsTRT               = EtaBins,
                                        OutputLevel              = OutputLevel,
                                        CaloTrackdEoverPLow      = 9*[0.0],
                                        CaloTrackdEoverPHigh     = 9*[999.0],
                                        TRTRatio                 = 9*[-999.],
                                        CaloTrackdEoverPLowTRT   = 9*[0.0],
                                        CaloTrackdEoverPHighTRT  = 9*[999.0],
                                        TRTRatioTRT              = 9*[-999.],
                                        CaloTrackdETATRT         = 9*[999.],
                                        CaloTrackdETA            = 9*[0.2],
                                        CaloTrackdPHI            = 9*[999.])
#********************************************************************************
# L2 Calo Ringer configurations: high energy, up to 20GeV

from TrigMultiVarHypo.TrigL2CaloRingerConstants import SignaturesMap
from TrigMultiVarHypo.TrigL2CaloRingerCutDefs            import TrigL2CaloRingerCutDefs
from TrigEgammaEmulationTool.TrigEgammaEmulationToolConf import Trig__TrigEgammaL2CaloRingerSelectorTool

# Medium
theCutDefs  = TrigL2CaloRingerCutDefs( 20, 'lhmedium' ,'e')
EgammaL2RingerMediumEmulator = ToolFactory(Trig__TrigEgammaL2CaloRingerSelectorTool,
                                           name              = "TrigEgammaL2CaloRingerMediumEmulator",
                                           Nodes             = theCutDefs.Nodes,
                                           Weights           = theCutDefs.Weights,
                                           Bias              = theCutDefs.Bias,
                                           Thresholds        = theCutDefs.Thresholds,
                                           EtBins            = theCutDefs.EtBins,
                                           EtaBins           = theCutDefs.EtaBins,
                                           OutputLevel       = OutputLevel,
                                           NormalisationRings= theCutDefs.NormalisationRings,
                                           SectionRings      = theCutDefs.SectionRings,
                                           NRings            = theCutDefs.NRings) 
 
#********************************************************************************
# Tight
theCutDefs  = TrigL2CaloRingerCutDefs( 20, 'lhtight' ,'e')
EgammaL2RingerTightEmulator = EgammaL2RingerMediumEmulator.copy(
                              name              = "TrigEgammaL2CaloRingerTightEmulator",
                              Nodes             = theCutDefs.Nodes,
                              Weights           = theCutDefs.Weights,
                              Bias              = theCutDefs.Bias,
                              Thresholds        = theCutDefs.Thresholds,
                              EtBins            = theCutDefs.EtBins,
                              EtaBins           = theCutDefs.EtaBins,
                              OutputLevel       = OutputLevel,
                              NormalisationRings= theCutDefs.NormalisationRings,
                              SectionRings      = theCutDefs.SectionRings,
                              NRings            = theCutDefs.NRings) 
 
#********************************************************************************
# Loose
theCutDefs  = TrigL2CaloRingerCutDefs( 20, 'lhloose' ,'e')
EgammaL2RingerLooseEmulator = EgammaL2RingerMediumEmulator.copy(
                              name              = "TrigEgammaL2CaloRingerLooseEmulator",
                              Nodes             = theCutDefs.Nodes,
                              Weights           = theCutDefs.Weights,
                              Bias              = theCutDefs.Bias,
                              Thresholds        = theCutDefs.Thresholds,
                              EtBins            = theCutDefs.EtBins,
                              EtaBins           = theCutDefs.EtaBins,
                              OutputLevel       = OutputLevel,
                              NormalisationRings= theCutDefs.NormalisationRings,
                              SectionRings      = theCutDefs.SectionRings,
                              NRings            = theCutDefs.NRings)
 
#********************************************************************************
# VeryLoose
theCutDefs  = TrigL2CaloRingerCutDefs( 20, 'lhvloose' ,'e')
EgammaL2RingerVeryLooseEmulator = EgammaL2RingerMediumEmulator.copy(
                                  name              = "TrigEgammaL2CaloRingerVeryLooseEmulator",
                                  Nodes             = theCutDefs.Nodes,
                                  Weights           = theCutDefs.Weights,
                                  Bias              = theCutDefs.Bias,
                                  Thresholds        = theCutDefs.Thresholds,
                                  EtBins            = theCutDefs.EtBins,
                                  EtaBins           = theCutDefs.EtaBins,
                                  OutputLevel       = OutputLevel,
                                  NormalisationRings= theCutDefs.NormalisationRings,
                                  SectionRings      = theCutDefs.SectionRings,
                                  NRings            = theCutDefs.NRings)
 
#********************************************************************************









