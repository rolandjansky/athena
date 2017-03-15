#
# Note: this assumes the alignment monitoring has already been added to the topSequence
# 
InDetAlignMonResidualsTRTAlign = IDAlignMonResiduals (name = 'InDetAlignMonResidualsTRTAlign', 
                                                      Pixel_Manager = InDetKeys.PixelManager(), 
                                                      SCT_Manager = InDetKeys.SCT_Manager(), 
                                                      tracksName = 'TRTAlignTracks', 
                                                      OutputLevel = ERROR, 
                                                      trackSelection = InDetAlignMonTrackSelectionTool, 
                                                      TRT_Manager = InDetKeys.TRT_Manager())  
 
ToolSvc += InDetAlignMonResidualsTRTAlign 
if (InDetFlags.doPrintConfigurables()): 
    print InDetAlignMonResidualsTRTAlign 
     
InDetAlignMonEfficienciesTRTAlign = IDAlignMonEfficiencies (name = 'InDetAlignMonEfficienciesTRTAlign', 
                                                            HoleSearch = InDetExtendedHoleSearchTool, 
                                                            Pixel_Manager = InDetKeys.PixelManager(), 
                                                            SCT_Manager = InDetKeys.SCT_Manager(), 
                                                            tracksName = 'TRTAlignTracks', 
                                                            trackSelection = InDetAlignMonTrackSelectionTool, 
                                                            OutputLevel = ERROR,
                                                            TRT_Manager = InDetKeys.TRT_Manager())     
 
ToolSvc += InDetAlignMonEfficienciesTRTAlign 
if (InDetFlags.doPrintConfigurables()): 
    print InDetAlignMonEfficienciesTRTAlign 
 
InDetAlignMonGenericTracksTRTAlign = IDAlignMonGenericTracks (name = 'InDetAlignMonGenericTracksTRTAlign', 
                                                              tracksName= 'TRTAlignTracks', 
                                                              trackSelection = InDetAlignMonTrackSelectionTool, 
                                                              VxPrimContainerName = InDetKeys.PrimaryVertices())    
 
ToolSvc += InDetAlignMonGenericTracksTRTAlign 
if (InDetFlags.doPrintConfigurables()): 
    print InDetAlignMonGenericTracksTRTAlign 
 
InDetTrackSplitterToolTRTAlign = InDet__InDetTrackSplitterTool(name = 'TrackSplitterTool_TRTAlign', 
                                                               TrackFitter = InDetTrackFitter, 
                                                               OutputUpperTracksName = 'TRTAlign_Upper', 
                                                               OutputLowerTracksName = 'TRTAlign_Lower', 
                                                               OutputLevel = ERROR)  
 
ToolSvc += InDetTrackSplitterToolTRTAlign 
if (InDetFlags.doPrintConfigurables()): 
    print InDetTrackSplitterToolTRTAlign 
     
InDetAlignMonTrackSegmentsTRTAlign = IDAlignMonTrackSegments ( name = 'InDetAlignMonTrackSegmentsTRTAlign', 
                                                               InputTracksName = 'TRTAlignTracks', 
                                                               UpperTracksName = 'TRTAlign_Upper', 
                                                               LowerTracksName = 'TRTAlign_Lower', 
                                                               UseCTBSplitTracks = False, 
                                                               TrackSplitter =  InDetTrackSplitterToolTRTAlign, 
                                                               trackSelectionUp = InDetAlignMonTrackSelectionTool, 
                                                               trackSelectionLow = InDetAlignMonTrackSelectionTool, 
                                                               DeltaD0Range = 8, 
                                                               DeltaD0Range2D = 10, 
                                                               DeltaPhiRange = 0.01, 
                                                               DeltaPhiRange2D = 0.05, 
                                                               OutputLevel = ERROR, 
                                                               D0Range = 800 ) 
ToolSvc += InDetAlignMonTrackSegmentsTRTAlign 
if (InDetFlags.doPrintConfigurables()): 
    print InDetAlignMonTrackSegmentsTRTAlign 

InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonResidualsTRTAlign ] 
InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonEfficienciesTRTAlign ] 
InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonGenericTracksTRTAlign ] 
InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonTrackSegmentsTRTAlign ] 

topSequence += InDetAlignMonManager
if (InDetFlags.doPrintConfigurables()):
    print InDetAlignMonManager


