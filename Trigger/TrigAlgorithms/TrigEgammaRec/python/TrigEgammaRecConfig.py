# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# configurables for TrigEgammaRec fexes
from AthenaCommon.Logging import logging
import traceback
from AthenaCommon.AppMgr import ToolSvc

from TrigEgammaRec import TrigEgammaRecConf


from egammaRec.Factories import Factory, ToolFactory, FcnWrapper, getPropertyValue 
# The following tools use the offline configuration
from egammaTools.egammaToolsFactories import EMConversionBuilder, EGammaAmbiguityTool,EMFourMomBuilder


# Define the isolation types
import ROOT, cppyy
# Need to be sure base dict is loaded first.
cppyy.loadDictionary('xAODCoreRflxDict')
cppyy.loadDictionary('xAODPrimitivesDict')
isoPar = ROOT.xAOD.Iso
## Be carefull : store them in decreasing dR
TrigEgammaIsoTypes = [ [ int(isoPar.ptcone40), int(isoPar.ptcone30), int(isoPar.ptcone20)],
        [ int(isoPar.etcone40), int(isoPar.etcone30), int(isoPar.etcone20)],
        [ int(isoPar.topoetcone40), int(isoPar.topoetcone30),int(isoPar.topoetcone20)] ]

# Import the TrigEgammaFactories
from TrigEgammaRec.TrigEgammaToolFactories import (TrigCaloIsolationTool,
        TrigCaloTopoIsolationTool,
        TrigTrackIsolationTool,
        TrigElectronPIDBuilder,
        TrigElectronCaloPIDBuilder,
        TrigPhotonPIDBuilder,
        TrigEMTrackMatchBuilder,
        TrigEMShowerBuilder
        )

def configureTrigEgammaMonitoring(tool):
  # from RecExConfig.RecFlags import rec
  # Configure monitoring tools
  from TrigEgammaRec.TrigEgammaRecMonitoring import TrigEgammaRecValidationMonitoring, TrigEgammaRecOnlineMonitoring, TrigEgammaRecCosmicMonitoring
  validation = TrigEgammaRecValidationMonitoring()
  online     = TrigEgammaRecOnlineMonitoring()
  cosmic     = TrigEgammaRecCosmicMonitoring()

  from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
  time = TrigTimeHistToolConfig("Time")
  tool.AthenaMonTools = [ time, validation, online, cosmic ]

# Define the Factory for TrigEgammaRec
TrigEgammaRec = Factory(TrigEgammaRecConf.TrigEgammaRec, name="TrigEgammaRec",doAdd=False,doPrint=True,
    # Keys
    ElectronContainerName="egamma_Electrons", 
    PhotonContainerName="egamma_Photons",
    
    # Tools with non-default configuration
    TrackMatchBuilderTool = TrigEMTrackMatchBuilder,
    ShowerBuilderTool = TrigEMShowerBuilder,
    
    # Set the isolation tools
    TrackIsolationTool = TrigTrackIsolationTool, 
    CaloCellIsolationTool = TrigCaloIsolationTool, 
    CaloTopoIsolationTool = TrigCaloTopoIsolationTool, 
    # Configure types -- define all needed
    # Decreasing order for pt/etcone (what about topocone?)
    IsoTypes = TrigEgammaIsoTypes,
    # Run isolation, easier to config than isotypes
    doTrackIsolation = True,
    doCaloCellIsolation = True,
    doCaloTopoIsolation = False,

    # Names of tools loaded must remain static and are set in TrigEgammaHypo/python/TrigEgammaPidTools.py
    ElectronPIDBuilder = TrigElectronPIDBuilder,
    ElectronCaloPIDBuilder = TrigElectronCaloPIDBuilder,
    PhotonPIDBuilder = TrigPhotonPIDBuilder,
    
    # Tools with default configuration    
#     VertexBuilderTool = EMVertexBuilder,
    ConversionBuilderTool = EMConversionBuilder, 
    AmbiguityTool = EGammaAmbiguityTool,
    FourMomBuilderTool = EMFourMomBuilder,
    
    # Flags
    doTrackMatching = True,
    doConversions = False,
    
    # Called after instantiation
    postInit = [configureTrigEgammaMonitoring],
)

