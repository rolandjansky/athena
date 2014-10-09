# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactories to instantiate all egammaCaloTools with default configuration"
__author__ = "Bruno Lenzi"

from egammaRec.Factories import FcnWrapper, ToolFactory
import egammaTrackToolsConf
from egammaRec import egammaKeys
from egammaTools.InDetTools import egammaExtrapolator

#---------------------------------------

from TrackToCalo import TrackToCaloConf
ExtrapolateToCaloTool = ToolFactory( TrackToCaloConf.ExtrapolateToCaloTool,
                                     name = 'exToCalo',
                                     Extrapolator = egammaExtrapolator)


EMExtrapolationTools = ToolFactory( egammaTrackToolsConf.EMExtrapolationTools,
                                    ExtrapolateToCaloTool = ExtrapolateToCaloTool,
                                    doPrint = True)

#---------------------------------------

# Import the factories that are not defined here
pass
