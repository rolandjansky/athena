# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function


def add_ipfordg(ToolSvc, calibration_tool, BTaggingFlags, 
                track_to_vertex_tool, track_to_vertex_ip_estimator, 
                do_neg=False): 
    # each tagger needs own instance, can't be shared!
    from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
    likelihood_tool = Analysis__NewLikelihoodTool( 
        name = "IPFordGNegProbTool" if do_neg else "IPFordGProbTool", 
        OutputLevel = BTaggingFlags.OutputLevel,
        taggerName = "IP3D",
        calibrationTool = calibration_tool, 
        )
    ToolSvc += likelihood_tool
    if BTaggingFlags.OutputLevel < 3:
      print (likelihood_tool)
    
    # each tagger needs own instance, can't be shared! ( -> not sure here ... ask Laurent, JB)
    from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
    track_selector = Analysis__TrackSelector( 
        useBLayerHitPrediction = True,
        name = "IPFordGNegTrackSel" if do_neg else "IPFordGTrackSel", 
        trackToVertexTool = track_to_vertex_tool,
        OutputLevel = BTaggingFlags.OutputLevel
        )
    ToolSvc += track_selector
    if BTaggingFlags.OutputLevel < 3:
      print (track_selector)
    
    from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
    sv_for_ip_tool = Analysis__SVForIPTool(
        name = "SVForIPTool_IPFordGNeg" if do_neg else "SVForIPTool_IPFordG",
        OutputLevel = BTaggingFlags.OutputLevel)
    
    ToolSvc += sv_for_ip_tool
    if BTaggingFlags.OutputLevel < 3:
      print (sv_for_ip_tool)
    
    from JetTagTools.JetTagToolsConf import (
        Analysis__DetailedTrackGradeFactory)
    detailed_track_grade_factory = Analysis__DetailedTrackGradeFactory(
        name = "IPFordGNegDetailedTGF" if do_neg else "IPFordGNegDetailedTGF",
        OutputLevel = BTaggingFlags.OutputLevel,
        hitBLayerGrade=True,
        TrackSummaryTool = None,
        useDetailSharedHitInfo = True,
        useSharedHitInfo = True)
    
    ToolSvc += detailed_track_grade_factory
    
    if BTaggingFlags.OutputLevel < 3:
      print (detailed_track_grade_factory)

    if do_neg: 
        flip_args = dict(
            usePosIP = True,
            useNegIP = False,
            flipIPSign = True,
            )
        vx_tool_name = "InDetVKalVxInJetToolFordGNeg"
    else: 
        flip_args = dict()
        vx_tool_name = "InDetVKalVxInJetToolFordG"

    from InDetVKalVxInJetTool.InDetVKalVxInJetToolConf import (
        InDet__InDetVKalVxInJetTool)
    vx_in_jet_tool = InDet__InDetVKalVxInJetTool(
        name = vx_tool_name,
        TrackSummaryTool = None, #Do not need it in b-tagging
        OutputLevel      = BTaggingFlags.OutputLevel)
    ToolSvc += vx_in_jet_tool

    from JetTagTools.JetTagToolsConf import Analysis__IPTag
    ip3d_tool = Analysis__IPTag(
        # IPTag strips the Tag of the end of the name and uses the rest as 
        # the TagInfo object name
        name = "IPFordGNegTag" if do_neg else "IPFordGTag",
        Runmodus = BTaggingFlags.Runmodus,
        referenceType = BTaggingFlags.ReferenceType,
        impactParameterView = "3D",
        OutputLevel = BTaggingFlags.OutputLevel,
        LikelihoodTool = likelihood_tool,
        trackSelectorTool = track_selector,
        trackToVertexTool = track_to_vertex_tool,
        trackGradePartitions = [ 
            "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ],
        RejectBadTracks = False,
        writeInfoPlus = False,
        originalTPCollectionName = BTaggingFlags.TrackParticleCollectionName,
        jetCollectionList = BTaggingFlags.Jets,
        jetWithInfoPlus   = BTaggingFlags.JetsWithInfoPlus,
        SecVxFinderNameForV0Removal = vx_tool_name, 
        SecVxFinderNameForIPSign = vx_tool_name, 
        SVForIPTool = sv_for_ip_tool,
        trackGradeFactory = detailed_track_grade_factory,
        TrackToVertexIPEstimator = track_to_vertex_ip_estimator,
        UseCHypo=True,

        #switch to true (better!) when creating new PDFs
        unbiasIPEstimation = False, 
        **flip_args
        )

    # Ford's cuts
    track_selector.nHitBLayer=0
    track_selector.nHitPix=1
    # track_selector.d0Max=2.0
    # track_selector.z0Max=2.5
    # InDetVKalVxNegInJetTool.CutPixelHits=1
    vx_in_jet_tool.CutPixelHits=1 

    if BTaggingFlags.OutputLevel < 3:
      print (vx_in_jet_tool)

    ToolSvc += ip3d_tool
    if BTaggingFlags.OutputLevel < 3:
      print (ip3d_tool)
    return ip3d_tool
