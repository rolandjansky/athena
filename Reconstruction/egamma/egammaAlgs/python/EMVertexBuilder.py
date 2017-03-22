# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactory to instantiate EMVertexBuilder with default configuration"
__author__ = "Bruno Lenzi"

from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import FcnWrapper, AlgFactory
from egammaRec import egammaKeys
from egammaTrackTools.egammaTrackToolsFactories import EMExtrapolationTools
from egammaTools.InDetTools import egammaInDetTrackSummaryTool, egammaExtrapolator

class VertexFinderToolInstance(FcnWrapper):
  def __call__(self):
    #
    # Configured conversion vertex reconstruction cuts
    #
    from InDetRecExample.ConfiguredSecondaryVertexCuts import ConfiguredSecondaryVertexCuts
    egammaConversionVertexCuts = ConfiguredSecondaryVertexCuts(mode ="EGammaPileUp")
    
    from InDetRecExample.ConfiguredSecVertexFinding import ConfiguredSecVertexFinding
    theemvertexfindertool=ConfiguredSecVertexFinding (prefix         = "egammaConversion",
                                                      VertexCuts     = egammaConversionVertexCuts,
                                                      TrackParticles = egammaKeys.outputTrackParticleKey(),
                                                      SecVertices    = egammaKeys.outputConversionKey(),
                                                      Extrapolator   = egammaExtrapolator(),
                                                      TrackSummaryTool = egammaInDetTrackSummaryTool(),
                                                      printConfig      = False)

    return theemvertexfindertool.toolInstance()


EMVertexBuilder = AlgFactory( egammaAlgsConf.EMVertexBuilder,
    InputTrackParticleContainerName = egammaKeys.outputTrackParticleKey(),
    OutputConversionContainerName   = egammaKeys.outputConversionKey(),
    VertexFinderTool                = VertexFinderToolInstance(),
    ExtrapolationTool = EMExtrapolationTools)
