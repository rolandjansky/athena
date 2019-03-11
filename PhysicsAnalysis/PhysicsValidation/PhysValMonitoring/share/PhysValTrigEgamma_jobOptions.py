from TrigEgammaMonitoring.TrigEgammaMonitoringConfig import TrigEgammaMonitoringTool
tools = TrigEgammaMonitoringTool()
for tool1 in tools:
    tool1.EnableLumi = False
    tool1.OutputLevel = WARNING
    #tool1.DetailLevel = 10
    
    tool1.EnableLumi = False
    #monMan.AthenaMonTools += [tool1]
