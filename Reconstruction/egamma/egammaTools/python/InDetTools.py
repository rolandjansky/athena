# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactories to instantiate InDet tools for egamma with default configuration"
__author__ = "Bruno Lenzi"

from egammaRec.Factories import FcnWrapper, ToolFactory, PublicToolFactory

###############
#egammaExtrapolator: just use the AtlasExtrapolator, making a tool factory
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
egammaExtrapolator = ToolFactory(AtlasExtrapolator,
                                 name = 'AtlasExtrapolator')

#egammaInDetTrackSummaryTool: just use the AtlasTrackSummaryTool, making a tool factory
from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
egammaInDetTrackSummaryTool = PublicToolFactory( AtlasTrackSummaryTool,
                                                 name = "AtlasTrackSummaryTool")


