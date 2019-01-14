# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


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
                                        OutputLevel              = OutputLevel,
                                        CaloTrackdEoverPLow      = 0.0,
                                        CaloTrackdEoverPHigh     = 999.,
                                        TRTRatio                 = -999.,
                                        CaloTrackdETA            = 0.2,
                                        CaloTrackdPHI            = 0.3,)

#********************************************************************************
# L2 Calo Ringer configurations: high energy, up to 20GeV

from TrigEgammaEmulationTool.TrigEgammaEmulationToolConf import Trig__TrigEgammaL2CaloRingerSelectorTool
from TrigMultiVarHypo.TrigL2CaloRingerHypoConfig         import TrigL2CaloRingerPidConfs
from TrigMultiVarHypo.TrigRingerPreprocessorDefs         import Norm1

theRingerConfig = TrigL2CaloRingerPidConfs()
preproc = Norm1()

EgammaL2RingerVeryLooseEmulator = ToolFactory(Trig__TrigEgammaL2CaloRingerSelectorTool,
                                           name                = "TrigEgammaL2CaloRingerVeryLooseEmulator",
                                           NormalisationRings  = preproc.NormalisationRings,
                                           SectionRings        = preproc.SectionRings,
                                           NRings              = preproc.NRings,
                                           OutputLevel         = OutputLevel,
                                           CalibPathConstants  = theRingerConfig.get_constants_path('e', 'lhvloose'), 
                                           CalibPathThresholds = theRingerConfig.get_cutDefs_path('e', 'lhvloose'), 
                                           ) 

EgammaL2RingerTightEmulator = EgammaL2RingerVeryLooseEmulator.copy(
                                           name                = "TrigEgammaL2CaloRingerTightEmulator",
                                           NormalisationRings  = preproc.NormalisationRings,
                                           SectionRings        = preproc.SectionRings,
                                           NRings              = preproc.NRings,
                                           OutputLevel         = OutputLevel,
                                           CalibPathConstants  = theRingerConfig.get_constants_path('e', 'lhtight') ,
                                           CalibPathThresholds = theRingerConfig.get_cutDefs_path('e', 'lhtight') ,
                                           ) 

EgammaL2RingerMediumEmulator = EgammaL2RingerVeryLooseEmulator.copy(
                                           name                = "TrigEgammaL2CaloRingerMediumEmulator",
                                           NormalisationRings  = preproc.NormalisationRings,
                                           SectionRings        = preproc.SectionRings,
                                           NRings              = preproc.NRings,
                                           OutputLevel         = OutputLevel,
                                           CalibPathConstants  = theRingerConfig.get_constants_path('e', 'lhmedium') ,
                                           CalibPathThresholds = theRingerConfig.get_cutDefs_path('e', 'lhmedium') ,
                                           ) 

EgammaL2RingerLooseEmulator = EgammaL2RingerVeryLooseEmulator.copy(
                                           name                = "TrigEgammaL2CaloRingerLooseEmulator",
                                           NormalisationRings  = preproc.NormalisationRings,
                                           SectionRings        = preproc.SectionRings,
                                           NRings              = preproc.NRings,
                                           OutputLevel         = OutputLevel,
                                           CalibPathConstants  = theRingerConfig.get_constants_path('e', 'lhloose') ,
                                           CalibPathThresholds = theRingerConfig.get_cutDefs_path('e', 'lhloose') ,
                                           ) 




