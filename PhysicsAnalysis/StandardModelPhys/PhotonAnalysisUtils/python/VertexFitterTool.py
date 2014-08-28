# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
import traceback
from AthenaCommon.AlgSequence import AlgSequence,AthSequencer

def VertexFitterTool(theAtlasExtrapolator):

    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr

    from TrkMagFieldTools.TrkMagFieldToolsConf import Trk__MagneticFieldTool
    theAtlasMagField = Trk__MagneticFieldTool('theAtlasMagField')
    ToolSvc += theAtlasMagField
    
    # ##### Vertex fitter
    from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
    InDetConversionVxFitterTool = Trk__TrkVKalVrtFitter(name            = "InDetConversionVxFitter",
                                                        Extrapolator        = theAtlasExtrapolator,
                                                        IterationNumber     = 30,
                                                        MagFieldSvc         = theAtlasMagField,
                                                        Robustness          = 6,
                                                        usePhiCnst          = True,
                                                        useThetaCnst        = True,
                                                        InputParticleMasses = [0.511,0.511],
                                                        MakeExtendedVertex=True,
                                                        FirstMeasuredPoint=True
                                                        )
    
    ToolSvc += InDetConversionVxFitterTool
    print      InDetConversionVxFitterTool

    return InDetConversionVxFitterTool

