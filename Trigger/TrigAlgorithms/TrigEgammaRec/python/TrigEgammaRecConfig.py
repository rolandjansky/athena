# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# configurables for TrigEgammaRec fexes
from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
import traceback
import EventKernel.ParticleDataType

import PyCintex
PyCintex.loadDictionary('egammaEnumsDict')
from ROOT import egammaParameters 

from TrigEgammaRec import TrigEgammaRecConf

# New configuration for use in rel 19.X with xAOD
# Adapted from egammaRec/egammaGetter.py
# Factory tools, handles configuration of tools and dependencies
from egammaRec.Factories import ToolFactory, FcnWrapper 

# Import tools required for trigger reconstruction
from egammaTools.egammaToolsFactories import \
        EMTrackMatchBuilder, EMVertexBuilder, EMConversionBuilder, EMAmbiguityTool, \
        EMFourMomBuilder, EMShowerBuilder
    # Not importing
    # EMVertexBuilder
    # egammaCheckEnergyDepositTool, EMBremCollectionBuilder, 
    # egammaOQFlagsBuilder, PIDBuilders


def configureTrigEgammaMonitoring(tool):
  print 'configuring'
  from RecExConfig.RecFlags import rec
#   if not rec.doMonitoring(): # TODO: Check if there is another trigger-specific flag
#     return
  # Configure monitoring tools
  from TrigEgammaRec.TrigEgammaRecMonitoring import TrigEgammaRecValidationMonitoring, TrigEgammaRecOnlineMonitoring, TrigEgammaRecCosmicMonitoring
  validation = TrigEgammaRecValidationMonitoring()
  online     = TrigEgammaRecOnlineMonitoring()
  cosmic     = TrigEgammaRecCosmicMonitoring()

  from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
  time = TrigTimeHistToolConfig("Time")
  tool.AthenaMonTools = [ time, validation, online, cosmic ]

TrigEgammaRec = ToolFactory(TrigEgammaRecConf.TrigEgammaRec, name="TrigEgammaRec",
    # Keys
    ElectronContainerAliasSuffix="egamma_Electrons", 
    PhotonContainerAliasSuffix="egamma_Photons",
    
    
    # Tools with non-default configuration
    TrackMatchBuilderTool = EMTrackMatchBuilder(
      name = "TrigEMTrackMatchBuilder",
      broadDeltaEta      = 0.2, #For offline 0.1
      broadDeltaPhi      = 0.2, #For offline 0.15
      useScoring         = False, #AT 26-March-2012: use old beheavior (differently from offline for GSF)
      SecondPassRescale  = False  #AT 26-March-2012: use old beheavior (differently from offline for GSF)
    ),
    ShowerBuilderTool = EMShowerBuilder(
      name = "TrigEgammaShowerBuilder", 
      doEnergyDensityCorrection=False,
      TopoCaloClusterInputName = "",
      CellsName = "",
      EMTopoCaloIsolationTool=None,
      TopoIsoTypes = [],
      Print = True,
    ),

    # Tools with default configuration    
#     VertexBuilderTool = EMVertexBuilder,
    ConversionBuilderTool = EMConversionBuilder, 
    AmbiguityTool = EMAmbiguityTool,
    FourMomBuilderTool = EMFourMomBuilder,
    
    # Flags
    doTrackMatching = True,
    doConversions = False,
    
    # Called after instantiation
    postInit = [configureTrigEgammaMonitoring],
)

# TODO: remove and add to the menu:
# TrigEgammaRec() # electron chains
# TrigEgammaRec(name = "TrigEgammaRec_NoIDEF", doTrackMatching = False) # photon chains

TrigEgammaRec_eGamma = TrigEgammaRec
def TrigEgammaRec_NoIDEF_eGamma():
  TrigEgammaRec(name = "TrigEgammaRec_NoIDEF_eGamma", doTrackMatching = False)
  

