## CaloExtensionBuilderAlg Stuff

def CaloExtensionBuilder():
    try: 
        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        from TrackToCalo.TrackToCaloConf import Trk__ParticleCaloExtensionTool
    except:
        mlog.error("could not import" "TrackToCaloConf.Trk__ParticleCaloExtensionTool")
        mlog.error("could not import TrkExTools.AtlasExtrapolator")
        mlog.error (traceback.format_exc())
    try:
        from TrackToCalo.TrackToCaloConf import Trk__CaloExtensionBuilderAlg as CaloExtensionBuilderAlg
    except:
        mlog.error("could not import TrackToCaloConf.Trk__CaloExtensionBuilderAlg")
        mlog.error (traceback.format_exc())
    try:
        from AthenaCommon.AppMgr import ToolSvc
    except:
        mlog.error("could not import ToolSvc")
        mlog.error (traceback.format_exc())
    try:
        from AthenaCommon.AlgSequence import AlgSequence
    except:
        mlog.error("could not import AlgSequence")
        mlog.error (traceback.format_exc())

    topSequence=AlgSequence()
    
    theAtlasExtrapolator=AtlasExtrapolator(name = "CaloExtensionBuilderAtlasExtrapolator")
    theAtlasExtrapolator.DoCaloDynamic = False # this turns off dynamic

    pcExtensionTool = Trk__ParticleCaloExtensionTool(Extrapolator = theAtlasExtrapolator)
    ToolSvc += pcExtensionTool

    CaloExtensionBuilderTool = CaloExtensionBuilderAlg(LastCaloExtentionTool = pcExtensionTool)

    from AthenaCommon.BeamFlags import jobproperties
    ToolSvc += CaloExtensionBuilderTool.LastCaloExtentionTool

    topSequence += CaloExtensionBuilderTool

    return True
