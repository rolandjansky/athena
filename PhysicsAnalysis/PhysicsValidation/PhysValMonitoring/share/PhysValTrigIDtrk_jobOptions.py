from TrigIDtrkMonitoring.TrigIDPhysValMonitoringConfig import TrigIDPhysValMonitoringTool
for toolname in TrigIDPhysValMonitoringTool():
    tool1 = getattr(ToolSvc, toolname.split('/')[-1])
    tool1.EnableLumi = False
    tool1.OutputLevel = WARNING
    #tool1.DetailLevel = 10
    
    tool1.EnableLumi = False
    ToolSvc += tool1
    monMan.AthenaMonTools += [tool1]
