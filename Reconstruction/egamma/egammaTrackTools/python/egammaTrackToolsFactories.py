# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = """ToolFactories to instantiate all
egammaCaloTools with default configuration"""
__author__ = "Bruno Lenzi , Christos Anastopoulos"

from egammaRec.Factories import ToolFactory
from egammaTrackTools import egammaTrackToolsConf
from egammaTools.egammaExtrapolators import (
    egammaCaloExtrapolator, egammaExtrapolator)
import AthenaCommon.CfgMgr as CfgMgr


egCaloDepthTool = ToolFactory(
    CfgMgr.CaloDepthTool,
    name="egCaloDepthTool",
    DepthChoice="middle"
)

egCaloSurfaceBuilder = ToolFactory(
    CfgMgr.CaloSurfaceBuilder,
    name="egCaloSurfaceBuilder",
    CaloDepthTool=egCaloDepthTool
)

EMParticleCaloExtensionTool = ToolFactory(
    CfgMgr.Trk__ParticleCaloExtensionTool,
    name="EMParticleCaloExtensionTool",
    Extrapolator=egammaCaloExtrapolator,
    ParticleType="electron",
    CaloSurfaceBuilder=egCaloSurfaceBuilder,
    StartFromPerigee=True)


EMExtrapolationTools = ToolFactory(
    egammaTrackToolsConf.EMExtrapolationTools,
    name="EMExtrapolationTools",
    CaloExtensionTool=EMParticleCaloExtensionTool,
    Extrapolator=egammaExtrapolator
)
