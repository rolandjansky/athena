# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# configurables for TrigEgammaRec fexes
from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
import traceback
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon import CfgMgr
# import EventKernel.ParticleDataType

from TrigEgammaRec import TrigEgammaRecConf

# New configuration for use in rel 19.X with xAOD
# Adapted from egammaRec/egammaGetter.py
# Factory tools, handles configuration of tools and dependencies
from egammaRec.Factories import Factory, ToolFactory, FcnWrapper, getPropertyValue 

# Import tools required for trigger reconstruction
from egammaTools.egammaToolsFactories import \
        EMTrackMatchBuilder, EMVertexBuilder, EMConversionBuilder, EGammaAmbiguityTool, \
        EMFourMomBuilder, EMShowerBuilder
    # Not importing
    # egammaCheckEnergyDepositTool, EMBremCollectionBuilder, 
    # egammaOQFlagsBuilder
from CaloClusterCorrection import CaloClusterCorrectionConf as Cccc
TrigCaloFillRectangularCluster = ToolFactory( Cccc.CaloFillRectangularCluster,
        name = "trigegamma_CaloFillRectangularCluster",
        eta_size = 5,
        phi_size = 7,
        cells_name = "")

from AthenaCommon.GlobalFlags import globalflags
isMC = not globalflags.DataSource()=='data'
from IsolationCorrections.IsolationCorrectionsConf import CP__IsolationCorrectionTool as ICT
IsoCorrectionToolTrig = ToolFactory(ICT,
                                    name = "NewLeakageCorrToolTrig",
                                    IsMC = isMC)
from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool,xAOD__TrackIsolationTool
from CaloIdentifier import SUBCALO
CaloIsolationTool = ToolFactory(xAOD__CaloIsolationTool,name = "TrigEgammaCaloIsolationTool",
        doEnergyDensityCorrection = False,
        saveOnlyRequestedCorrections = True,
        IsoLeakCorrectionTool          = IsoCorrectionToolTrig,
        CaloFillRectangularClusterTool = TrigCaloFillRectangularCluster,
        EMCaloNums = [SUBCALO.LAREM],
        HadCaloNums = [SUBCALO.LARHEC,SUBCALO.TILE])

from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__CaloClustersInConeTool
CaloClustersInConeTool = ToolFactory(xAOD__CaloClustersInConeTool,CaloClusterLocation = "CaloCalTopoCluster")

CaloTopoIsolationTool = ToolFactory(xAOD__CaloIsolationTool,name = "TrigEgammaCaloTopoIsolationTool",
        doEnergyDensityCorrection = True,
        saveOnlyRequestedCorrections = True,
        IsoLeakCorrectionTool          = IsoCorrectionToolTrig,
        ClustersInConeTool              = CaloClustersInConeTool,
        CaloFillRectangularClusterTool = TrigCaloFillRectangularCluster,
        UseEMScale = True,
        TopoClusterEDCentralContainer = "HLTTopoClusterIsoCentralEventShape",
        TopoClusterEDForwardContainer = "HLTTopoClusterIsoForwardEventShape",
        OutputLevel = 2)

TrackIsolationTool = ToolFactory(xAOD__TrackIsolationTool, name = 'TrigEgammaTrackIsolationTool')

TrkIsoCfg = CfgMgr.xAOD__TrackIsolationTool('TrigEgammaTrackIsolationTool')
TrkIsoCfg.TrackSelectionTool.maxZ0SinTheta = 3.
TrkIsoCfg.TrackSelectionTool.minPt = 1000.
TrkIsoCfg.TrackSelectionTool.CutLevel = "Loose"

import ROOT, cppyy
# Need to be sure base dict is loaded first.
cppyy.loadDictionary('xAODCoreRflxDict')
cppyy.loadDictionary('xAODPrimitivesDict')
isoPar = ROOT.xAOD.Iso
#IsoTypes =  [[ int(isoPar.ptcone20), int(isoPar.ptcone30), int(isoPar.ptcone40) ],]

# PID -- selectors added via TrigEgammaPIDTools in TrigEgammaHypo
from TrigEgammaHypo.TrigEgammaPidTools import ElectronPidTools
from TrigEgammaHypo.TrigEgammaPidTools import PhotonPidTools
ElectronPidTools()
PhotonPidTools()


from egammaTools.egammaToolsConf import EMPIDBuilder

from LumiBlockComps.LuminosityToolDefault import LuminosityToolOnline
ToolSvc += LuminosityToolOnline()
def configureTrigEgammaMonitoring(tool):
  from RecExConfig.RecFlags import rec
  # Configure monitoring tools
  from TrigEgammaRec.TrigEgammaRecMonitoring import TrigEgammaRecValidationMonitoring, TrigEgammaRecOnlineMonitoring, TrigEgammaRecCosmicMonitoring
  validation = TrigEgammaRecValidationMonitoring()
  online     = TrigEgammaRecOnlineMonitoring()
  cosmic     = TrigEgammaRecCosmicMonitoring()

  from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
  time = TrigTimeHistToolConfig("Time")
  tool.AthenaMonTools = [ time, validation, online, cosmic ]

TrigEgammaRec = Factory(TrigEgammaRecConf.TrigEgammaRec, name="TrigEgammaRec",doAdd=False,doPrint=True,
    # Keys
    ElectronContainerName="egamma_Electrons", 
    PhotonContainerName="egamma_Photons",
    
    
    # Tools with non-default configuration
    TrackMatchBuilderTool = EMTrackMatchBuilder(
      name = "TrigEMTrackMatchBuilder",
      broadDeltaEta      = 0.2, #For offline 0.1
      broadDeltaPhi      = 0.2, #For offline 0.15
      useScoring         = False, 
      SecondPassRescale  = False  
    ),
    ShowerBuilderTool = EMShowerBuilder(
      name = "TrigEgammaShowerBuilder", 
      CellsName = "",
      Print = True,
    ),
    
    # Set the isolation tools
    TrackIsolationTool = TrackIsolationTool, 
    CaloCellIsolationTool = CaloIsolationTool, 
    CaloTopoIsolationTool = CaloTopoIsolationTool, 
    # Configure types -- define all needed
    # Decreasing order for pt/etcone (what about topocone?)
    IsoTypes = [ 
        [ int(isoPar.ptcone40), ## Be carefull : store them in decreasing dR
          int(isoPar.ptcone30), 
          int(isoPar.ptcone20)],
        [ int(isoPar.etcone40), ## Be carefull : store them in decreasing dR
          int(isoPar.etcone30), 
          int(isoPar.etcone20)],
        [ int(isoPar.topoetcone40), ## Be carefull : store them in decreasing dR
          int(isoPar.topoetcone30),
          int(isoPar.topoetcone20)]], 
    # Run isolation, easier to config than isotypes
    doTrackIsolation = True,
    doCaloCellIsolation = True,
    doCaloTopoIsolation = False,

    # Names of tools loaded must remain static and are set in TrigEgammaHypo/python/TrigEgammaPidTools.py
    ElectronPIDBuilder = ToolFactory( EMPIDBuilder, name = "TrigElectronPIDBuilder",
        electronIsEMselectors = [ToolSvc.AsgElectronIsEMVLooseSelector,
            ToolSvc.AsgElectronIsEMLooseSelector,
            ToolSvc.AsgElectronIsEMMediumSelector,
            ToolSvc.AsgElectronIsEMTightSelector],
            
        electronIsEMselectorResultNames = ["VLoose","Loose","Medium","Tight"],
        electronLHselectors = [ToolSvc.AsgElectronLHVLooseSelector,
            ToolSvc.AsgElectronLHLooseSelector,
            ToolSvc.AsgElectronLHMediumSelector,
            ToolSvc.AsgElectronLHTightSelector],
        electronLHselectorResultNames= ["LHVLoose","LHLoose","LHMedium","LHTight"],
        ),

    ElectronCaloPIDBuilder = ToolFactory( EMPIDBuilder, name = "TrigElectronCaloPIDBuilder",
        electronLHselectors = [ToolSvc.AsgElectronLHVLooseCaloSelector,
            ToolSvc.AsgElectronLHLooseCaloSelector,
            ToolSvc.AsgElectronLHMediumCaloSelector,
            ToolSvc.AsgElectronLHTightCaloSelector],
        electronLHselectorResultNames= ["LHCaloVLoose","LHCaloLoose","LHCaloMedium","LHCaloTight"],
        LHValueName = "LHCaloValue",
        ),
    PhotonPIDBuilder = ToolFactory( EMPIDBuilder, name = "TrigPhotonPIDBuilder",
        photonIsEMselectors= [ToolSvc.AsgPhotonIsEMLooseSelector,
            ToolSvc.AsgPhotonIsEMMediumSelector,
            ToolSvc.AsgPhotonIsEMTightSelector],
        photonIsEMselectorResultNames = ["Loose","Medium","Tight"],
        ),

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

