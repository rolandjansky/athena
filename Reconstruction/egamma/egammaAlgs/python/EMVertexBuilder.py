# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactory to instantiate EMVertexBuilder with default configuration"
__author__ = "Bruno Lenzi"

from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import FcnWrapper, AlgFactory, PublicToolFactory
from egammaRec import egammaKeys
from egammaTrackTools.egammaTrackToolsFactories import EMExtrapolationTools
from egammaTools.egammaExtrapolators import egammaExtrapolator


class VertexFinderToolInstance(FcnWrapper):
    def __call__(self):

        #################################################################
        # egamma InDet summary tool to be used conversion finding
        #
        # load association tool from Inner Detector to handle pixel ganged
        # ambiguities
        from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
        egammaInDetPrdAssociationTool = PublicToolFactory(InDet__InDetPRD_AssociationToolGangedPixels,
                                                          name="egammaInDetPrdAssociationTool",
                                                          PixelClusterAmbiguitiesMapName='PixelClusterAmbiguitiesMap')

        # Loading Configurable HoleSearchTool
        #
        from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
        egammaInDetHoleSearchTool = PublicToolFactory(InDet__InDetTrackHoleSearchTool,
                                                      name="egammaInDetHoleSearchTool",
                                                      Extrapolator=egammaExtrapolator)
        #
        # Load the InDetTrackSummaryHelperTool
        #
        from AthenaCommon.DetFlags import DetFlags
        from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
        egammaInDetTrackSummaryHelperTool = PublicToolFactory(InDet__InDetTrackSummaryHelperTool,
                                                              name="egammaInDetSummaryHelper",
                                                              AssoTool=egammaInDetPrdAssociationTool,
                                                              DoSharedHits=False,
                                                              HoleSearch=egammaInDetHoleSearchTool,
                                                              usePixel=DetFlags.haveRIO.pixel_on(),
                                                              useSCT=DetFlags.haveRIO.SCT_on(),
                                                              useTRT=DetFlags.haveRIO.TRT_on())

        #
        from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
        egammaInDetTrackSummaryTool = PublicToolFactory(Trk__TrackSummaryTool,
                                                        name="egammaInDetTrackSummaryTool",
                                                        InDetSummaryHelperTool=egammaInDetTrackSummaryHelperTool,
                                                        doSharedHits=False,
                                                        doHolesInDet=True)

        #
        # Configured conversion vertex reconstruction cuts
        #
        from InDetRecExample.ConfiguredSecondaryVertexCuts import ConfiguredSecondaryVertexCuts
        egammaConversionVertexCuts = ConfiguredSecondaryVertexCuts(
            mode="EGammaPileUp")

        from InDetRecExample.ConfiguredSecVertexFinding import ConfiguredSecVertexFinding
        theemvertexfindertool = ConfiguredSecVertexFinding(prefix="egammaConversion",
                                                           VertexCuts=egammaConversionVertexCuts,
                                                           TrackParticles=egammaKeys.outputTrackParticleKey(),
                                                           SecVertices=egammaKeys.outputConversionKey(),
                                                           Extrapolator=egammaExtrapolator(),
                                                           TrackSummaryTool=egammaInDetTrackSummaryTool(),
                                                           printConfig=False)

        return theemvertexfindertool.toolInstance()


EMVertexBuilder = AlgFactory(egammaAlgsConf.EMVertexBuilder,
                             InputTrackParticleContainerName=egammaKeys.outputTrackParticleKey(),
                             OutputConversionContainerName=egammaKeys.outputConversionKey(),
                             VertexFinderTool=VertexFinderToolInstance(),
                             ExtrapolationTool=EMExtrapolationTools)
