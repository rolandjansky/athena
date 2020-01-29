## CaloExtensionBuilderAlg Stuff
# Defined as function such that the user can change the cut level and minPt


def CaloExtensionBuilder( cutLevel = "TightPrimary", minPT = 100.0 ):
    try: 
        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        from TrackToCalo.TrackToCaloConf import Trk__ParticleCaloExtensionTool
    except:
        mlog.error("could not import TrackToCaloConf.Trk__ParticleCaloExtensionTool")
        mlog.error("could not import TrkExTools.AtlasExtrapolator")
        print traceback.format_exc()
    try:
        from TrackToCalo.TrackToCaloConf import Trk__CaloExtensionBuilderAlg as CaloExtensionBuilderAlg
    except:
        mlog.error("could not import TrackToCaloConf.Trk__CaloExtensionBuilderAlg")
        print traceback.format_exc()
    try:
        from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
        from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
    except:
        mlog.error("could not import InDetTrackSelectionTool.InDet__InDetTrackSelectionTool")
        print traceback.format_exc()
    try:
        from AthenaCommon.AppMgr import ToolSvc
    except:
        mlog.error("could not import ToolSvc")
        print traceback.format_exc()
    try:
        from AthenaCommon.AlgSequence import AlgSequence
    except:
        mlog.error("could not import AlgSequence")
        print traceback.format_exc()

    topSequence=AlgSequence()
    
    theAtlasExtrapolator=AtlasExtrapolator(name = "CaloExtensionBuilderAtlasExtrapolator")
    theAtlasExtrapolator.DoCaloDynamic = False # this turns off dynamic

    pcExtensionTool = Trk__ParticleCaloExtensionTool(Extrapolator = theAtlasExtrapolator)
    ToolSvc += pcExtensionTool

    CaloExtensionBuilderTool = CaloExtensionBuilderAlg(LastCaloExtentionTool = pcExtensionTool)

    from AthenaCommon.BeamFlags import jobproperties

    TrackSelectionToolHC = InDet__InDetTrackSelectionTool(name            = "CaloExtensionBuilderTrackSelectionTool",
                                                           minPt           = minPT,
                                                           CutLevel        = cutLevel,
                                                           minNSiHits      = 7) # PixelHits + SCTHits + PixelDeadSensors + SCTDeadSensors
    
        
    CaloExtensionBuilderTool.TrkSelection         = TrackSelectionToolHC    

    ToolSvc += CaloExtensionBuilderTool.LastCaloExtentionTool

    topSequence += CaloExtensionBuilderTool

    return True
