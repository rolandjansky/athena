from TrigBphysMonitoring.TrigBphysMonitoringConfig import TrigBphysMonitoringTool
for tool1 in TrigBphysMonitoringTool():
    tool1.EnableLumi = False
    tool1.JpsiCandidates = '' # The creation and monitoring of the JPsi collection is disabled in PhysVal operation
    tool1.OutputLevel = WARNING
    #tool1.DetailLevel = 10
    
    tool1.EnableLumi = False
    monMan.AthenaMonTools += [tool1]
