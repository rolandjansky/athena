from TrigCaloMonitoring.TrigCaloMonitoringConfig import HLTCaloMonitoringTool
for tool1 in HLTCaloMonitoringTool():
    tool1.EnableLumi = False
    tool1.OutputLevel = WARNING
    #tool1.DetailLevel = 10
    
    tool1.EnableLumi = False
    monMan.AthenaMonTools += [tool1]
