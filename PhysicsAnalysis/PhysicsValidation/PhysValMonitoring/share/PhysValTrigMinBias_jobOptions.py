from TrigMinBiasMonitoring.TrigMinBiasMonitoringConfig import TrigMinBiasMonitoringTool
for tool1 in TrigMinBiasMonitoringTool():
    tool1.EnableLumi = False
    tool1.OutputLevel = WARNING
    #tool1.DetailLevel = 10
    
    tool1.EnableLumi = False
    monMan.AthenaMonTools += [tool1]
