theApp.Dlls += [ "InDetVKalVxInJetTool" ]
theApp.Dlls += [ "TrkVKalVrtFitter" ]


from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
AtlasTrackSummaryTool = AtlasTrackSummaryTool()
ToolSvc += AtlasTrackSummaryTool
print AtlasTrackSummaryTool

