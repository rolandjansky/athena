#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright @2016 Ryan Mackenzie White <ryan.white@cern.ch>
#

"""

"""
__doc__ = "Tool Factories to instantiate Egamma Tools for trigger"

__author__ = "Ryan Mackenzie White"
import logging
from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import ToolSvc
# New configuration for use in rel 19.X with xAOD
# Adapted from egammaRec/egammaGetter.py
# Factory tools, handles configuration of tools and dependencies
from egammaRec.Factories import Factory, ToolFactory, FcnWrapper, getPropertyValue 

# Import tools required for trigger reconstruction
# Following offline tools not used at HLT: 
# egammaCheckEnergyDepositTool 
# EMBremCollectionBuilder
# egammaOQFlagsBuilder
# EMVertexBuilder

# Following tools have TrigEgamma factories
from egammaTools.egammaToolsFactories import EMTrackMatchBuilder, EMFourMomBuilder, EMShowerBuilder
from egammaTrackTools.egammaTrackToolsFactories import EMExtrapolationTools

from egammaTools.egammaToolsConf import EMPIDBuilder
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
TrigCaloIsolationTool = ToolFactory(xAOD__CaloIsolationTool,name = "TrigEgammaCaloIsolationTool",
        doEnergyDensityCorrection = False,
        saveOnlyRequestedCorrections = True,
        IsoLeakCorrectionTool          = IsoCorrectionToolTrig,
        CaloFillRectangularClusterTool = TrigCaloFillRectangularCluster,
        EMCaloNums = [SUBCALO.LAREM],
        HadCaloNums = [SUBCALO.LARHEC,SUBCALO.TILE])

from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__CaloClustersInConeTool
TrigCaloClustersInConeTool = ToolFactory(xAOD__CaloClustersInConeTool,CaloClusterLocation = "CaloCalTopoCluster")

TrigCaloTopoIsolationTool = ToolFactory(xAOD__CaloIsolationTool,name = "TrigEgammaCaloTopoIsolationTool",
        doEnergyDensityCorrection = True,
        saveOnlyRequestedCorrections = True,
        IsoLeakCorrectionTool          = IsoCorrectionToolTrig,
        ClustersInConeTool              = TrigCaloClustersInConeTool,
        CaloFillRectangularClusterTool = TrigCaloFillRectangularCluster,
        UseEMScale = True,
        TopoClusterEDCentralContainer = "HLTTopoClusterIsoCentralEventShape",
        TopoClusterEDForwardContainer = "HLTTopoClusterIsoForwardEventShape")

TrigTrackIsolationTool = ToolFactory(xAOD__TrackIsolationTool, name = 'TrigEgammaTrackIsolationTool')

TrkIsoCfg = CfgMgr.xAOD__TrackIsolationTool('TrigEgammaTrackIsolationTool')
TrkIsoCfg.TrackSelectionTool.maxZ0SinTheta = 3.
TrkIsoCfg.TrackSelectionTool.minPt = 1000.
TrkIsoCfg.TrackSelectionTool.CutLevel = "Loose"

TrigElectronPIDBuilder = ToolFactory( EMPIDBuilder, name = "TrigElectronPIDBuilder",
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
)

TrigElectronCaloPIDBuilder = ToolFactory( EMPIDBuilder, name = "TrigElectronCaloPIDBuilder",
    electronLHselectors = [ToolSvc.AsgElectronLHVLooseCaloSelector,
        ToolSvc.AsgElectronLHLooseCaloSelector,
        ToolSvc.AsgElectronLHMediumCaloSelector,
        ToolSvc.AsgElectronLHTightCaloSelector],
    electronLHselectorResultNames= ["LHCaloVLoose","LHCaloLoose","LHCaloMedium","LHCaloTight"],
    LHValueName = "LHCaloValue",
)

TrigPhotonPIDBuilder = ToolFactory( EMPIDBuilder, name = "TrigPhotonPIDBuilder",
    photonIsEMselectors= [ToolSvc.AsgPhotonIsEMLooseSelector,
        ToolSvc.AsgPhotonIsEMMediumSelector,
        ToolSvc.AsgPhotonIsEMTightSelector],
    photonIsEMselectorResultNames = ["Loose","Medium","Tight"],
)

def appendtoTrigEMTrackMatchBuilder(tool):
    "add track to calo tool "
    if not hasattr(tool,"EMExtrapolationTools"):
        tool += EMExtrapolationTools()

TrigEMTrackMatchBuilder = EMTrackMatchBuilder.copy(
    name = "TrigEMTrackMatchBuilder",
    postInit=[appendtoTrigEMTrackMatchBuilder],
    broadDeltaEta      = 0.2, #For offline 0.1
    broadDeltaPhi      = 0.2, #For offline 0.15
    useScoring         = False, 
    SecondPassRescale  = False,

)

TrigEMShowerBuilder = EMShowerBuilder.copy(
  name = "TrigEgammaShowerBuilder", 
  CellsName = "",
  Print = True,
)

