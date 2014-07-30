theApp.Dlls += [ "TrigOnlineSpacePointTool" ]
ToolSvc      = Service( "ToolSvc" )
SpacePointTool = Algorithm( 'ToolSvc.OnlineSpacePointProviderTool' )
SpacePointTool.useStoreGate=TRUE
PixelSpacePointTool = Algorithm( 'ToolSvc.OnlineSpacePointProviderTool.PixelSpacePointTool' )
PixelSpacePointTool.PixelSP_ContainerName="TrigPixelSpacePoints"
SCT_SpacePointTool = Algorithm( 'ToolSvc.OnlineSpacePointProviderTool.SCT_SpacePointTool' )
SCT_SpacePointTool.SCT_SP_ContainerName="TrigSCT_SpacePoints"
SpacePointTool.useStoreGate=FALSE 


