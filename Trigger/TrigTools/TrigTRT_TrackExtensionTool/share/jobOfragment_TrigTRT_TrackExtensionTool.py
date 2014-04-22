theApp.Dlls += [ "TrigOfflineDriftCircleTool"]
theApp.Dlls += [ "TrigTRT_TrackExtensionTool" ]
ToolSvc      = Service( "ToolSvc" )
TrigTRTTool = Algorithm( 'ToolSvc.TrigTRT_TrackExtensionTool' )
TrigTRTTool.doMultScattering=TRUE
TrigTRTTool.OfflineClusters=TriggerFlags.useOfflineSpacePoints
Algorithm("ToolSvc.TrigTRT_TrackExtensionTool.TrigTRT_DetElementRoadTool").associationThreshold=0.2
TrigField=Algorithm( 'ToolSvc.TrigTRT_TrackExtensionTool.TrigMagneticFieldTool' )
TrigField.UseAthenaFieldService=TRUE



