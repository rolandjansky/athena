# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.Include     import include
import traceback
from AthenaCommon.AlgSequence import AlgSequence,AthSequencer
from AthenaCommon.GlobalFlags import GlobalFlags,globalflags

def HybridPrimaryVertex(theAtlasExtrapolator,
                        InDetSingleTrackConversion,
                        InDetConversionVxFitterTool,
                        exToCalo,
                        InDetConversionFinderUtils):

    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr
    
    from CaloDetDescr.CaloDepthToolBase import CaloDepthToolFactory
    hybridCaloDepthTool=CaloDepthToolFactory(depth='cscopt2')
    # showerdefault


    from PhotonD3PDMaker.ComputePointingQuantities import PAUComputePointingQuantities
    from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import DiphotonVertexID
    DiPhotonVertexIDTool = DiphotonVertexID(PAUComputePointingQuantities=PAUComputePointingQuantities,
                                            useFixedBeamSpread=(globalflags.DataSource=='geant4'),
                                            beamSpread=48,
                                            isData=not(globalflags.DataSource=='geant4'))
    ToolSvc += DiPhotonVertexIDTool
    
    from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUhybridPrimaryVertexTool
    ToolSvc += PAUhybridPrimaryVertexTool()
    ToolSvc.PAUhybridPrimaryVertexTool.ExtrapolationTool=theAtlasExtrapolator

    ToolSvc.PAUhybridPrimaryVertexTool.ConversionFinderUtils=  InDetConversionFinderUtils 
    ToolSvc.PAUhybridPrimaryVertexTool.VertexFitterTool = InDetConversionVxFitterTool
    ToolSvc.PAUhybridPrimaryVertexTool.SingleTrkConvTool = InDetSingleTrackConversion

    ToolSvc.PAUhybridPrimaryVertexTool.ExtrapolateToCaloTool=exToCalo

    ToolSvc.PAUhybridPrimaryVertexTool.CaloDepthTool=hybridCaloDepthTool
    ToolSvc.PAUhybridPrimaryVertexTool.isData = (globalflags.DataSource()=='data')
    ToolSvc.PAUhybridPrimaryVertexTool.beamSpread = 48;

    ToolSvc.PAUhybridPrimaryVertexTool.DiPhotonVertexID = DiPhotonVertexIDTool
    
    return ToolSvc.PAUhybridPrimaryVertexTool
    
