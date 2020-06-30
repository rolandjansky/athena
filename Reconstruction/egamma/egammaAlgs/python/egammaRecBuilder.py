# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = """ToolFactory to instantiate 
egammaRecBuilder with default configuration"""
__author__ = "Jovan Mitrevski"

from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import AlgFactory
from egammaRec.egammaRecFlags import jobproperties
from egammaRec import egammaKeys

from egammaTools.egammaToolsFactories import \
    EMTrackMatchBuilder, EMConversionBuilder

egammaRecBuilder = AlgFactory(
    egammaAlgsConf.egammaRecBuilder,
    name='egammaRecBuilder',
    InputTopoClusterContainerName=jobproperties.egammaRecFlags.egammaTopoClusterCollection(),
    egammaRecContainer=egammaKeys.EgammaRecKey(),
    # Builder tools
    TrackMatchBuilderTool=EMTrackMatchBuilder,
    ConversionBuilderTool=EMConversionBuilder
)
