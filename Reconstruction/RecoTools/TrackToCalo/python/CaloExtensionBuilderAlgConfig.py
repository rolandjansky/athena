# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def CaloExtensionBuilder(useLRT):
    import traceback
    from AthenaCommon.Logging import logging
    mlog = logging.getLogger('CaloExtensionBuilder')

    try: 
        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        from TrackToCalo.TrackToCaloConf import Trk__ParticleCaloExtensionTool
    except Exception:
        mlog.error("could not import" "TrackToCaloConf.Trk__ParticleCaloExtensionTool")
        mlog.error("could not import TrkExTools.AtlasExtrapolator")
        mlog.error (traceback.format_exc())
    try:
        from TrackToCalo.TrackToCaloConf import Trk__CaloExtensionBuilderAlg as CaloExtensionBuilderAlg
    except Exception:
        mlog.error("could not import TrackToCaloConf.Trk__CaloExtensionBuilderAlg")
        mlog.error (traceback.format_exc())
    try:
        from AthenaCommon.AppMgr import ToolSvc
    except Exception:
        mlog.error("could not import ToolSvc")
        mlog.error (traceback.format_exc())
    try:
        from AthenaCommon.AlgSequence import AlgSequence
    except Exception:
        mlog.error("could not import AlgSequence")
        mlog.error (traceback.format_exc())

    topSequence=AlgSequence()
    
    theAtlasExtrapolator=AtlasExtrapolator(name = "CaloExtensionBuilderAtlasExtrapolator")

    pcExtensionTool = Trk__ParticleCaloExtensionTool(Extrapolator = theAtlasExtrapolator)
    ToolSvc += pcExtensionTool

    if (True is useLRT):
      algName = "CaloExtensionBuilderAlg_LRT"      
    else:
      algName = "CaloExtensionBuilderAlg"
    CaloExtensionBuilderTool = CaloExtensionBuilderAlg(algName, LastCaloExtentionTool = pcExtensionTool)

    if True is useLRT:
      CaloExtensionBuilderTool.TrkPartContainerName = "InDetLargeD0TrackParticles"
      CaloExtensionBuilderTool.ParticleCache = "ParticleCaloExtension_LRT"

    ToolSvc += CaloExtensionBuilderTool.LastCaloExtentionTool
    topSequence += CaloExtensionBuilderTool

    return True
