# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = """ToolFactory to instantiate EMVertexBuilder
with default configuration"""
__author__ = "Bruno Lenzi"

import InDetRecExample.TrackingCommon as TrackingCommon
from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import FcnWrapper, AlgFactory
from egammaRec import egammaKeys
from egammaTrackTools.egammaTrackToolsFactories import EMExtrapolationTools
from egammaTools.egammaExtrapolators import AtlasPublicExtrapolator


class VertexFinderToolInstance(FcnWrapper):
    def __call__(self):

        # In reality we do NOT need a summary tool
        # but the confgured Secondary vertex
        # still asks for one (TODO)
        egammaVtxInDetTrackSummaryHelperTool = (
            TrackingCommon.getInDetSummaryHelper(
                name="egammaVtxInDetSummaryHelper",
                AssoTool=None,
                HoleSearch=None,
                DoSharedHits=False,
                private=True))

        egammaVtxInDetTrackSummaryTool = (
            TrackingCommon.getInDetTrackSummaryTool(
                name="egammaVtxInDetTrackSummaryTool",
                InDetSummaryHelperTool=egammaVtxInDetTrackSummaryHelperTool,
                doSharedHits=False,
                doHolesInDet=False))

        #
        # Configured conversion vertex reconstruction cuts
        #
        from InDetRecExample.ConfiguredSecondaryVertexCuts import (
            ConfiguredSecondaryVertexCuts)

        egammaConversionVertexCuts = ConfiguredSecondaryVertexCuts(
            mode="EGammaPileUp")

        from InDetRecExample.ConfiguredSecVertexFinding import (
            ConfiguredSecVertexFinding)

        theemvertexfindertool = ConfiguredSecVertexFinding(
            prefix="egammaConversion",
            VertexCuts=egammaConversionVertexCuts,
            TrackParticles=egammaKeys.outputTrackParticleKey(),
            SecVertices=egammaKeys.outputConversionKey(),
            Extrapolator=AtlasPublicExtrapolator(),
            TrackSummaryTool=egammaVtxInDetTrackSummaryTool,
            printConfig=False)

        return theemvertexfindertool.toolInstance()


EMVertexBuilder = AlgFactory(
    egammaAlgsConf.EMVertexBuilder,
    InputTrackParticleContainerName=egammaKeys.outputTrackParticleKey(),
    OutputConversionContainerName=egammaKeys.outputConversionKey(),
    VertexFinderTool=VertexFinderToolInstance(),
    ExtrapolationTool=EMExtrapolationTools)
