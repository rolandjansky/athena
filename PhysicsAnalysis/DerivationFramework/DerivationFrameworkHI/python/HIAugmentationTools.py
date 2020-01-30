# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from DerivationFrameworkCore.DerivationFrameworkMaster import *

def addHIGlobalAugmentationTool(deriv="HION7",vn=2, minPt=500) :
    from AthenaCommon.AppMgr import ToolSvc
    CutLevels = ["HILoose","HITight"]
    TrkSelTools = []
    for CutLevel in CutLevels:
        #Track selector
        from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
        ToolName = CutLevel + "TrackSelectionTool_%s" % deriv
        TrackSelectorTool=InDet__InDetTrackSelectionTool(ToolName) 
        TrackSelectorTool.CutLevel = CutLevel
        TrackSelectorTool.minPt=minPt
        ToolSvc += TrackSelectorTool
        TrkSelTools.append(TrackSelectorTool)
            
    from DerivationFrameworkHI.DerivationFrameworkHIConf import DerivationFramework__HIGlobalAugmentationTool
    HIGAugTool=DerivationFramework__HIGlobalAugmentationTool(name='%sHIGAugTool' % deriv,
                                                                InDetTrackParticlesKey="InDetTrackParticles",
                                                                cutLevels = CutLevels,
                                                                TrackSelectionTools=TrkSelTools,
                                                                nHarmonic=vn)
                                                                 

    ToolSvc+=HIGAugTool
    return HIGAugTool    
