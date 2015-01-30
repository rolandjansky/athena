from TrigMinBiasMonitoring.TrigMinBiasMonitoringConfig import TrigMinBiasMonitoringTool
tool1 = getattr(ToolSvc, TrigMinBiasMonitoringTool()[0].split('/')[-1])

tool1.EnableLumi = False
tool1.OutputLevel = WARNING
#tool1.DetailLevel = 10

monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
monMan.AthenaMonTools += [ tool1 ]

# for name in TrigTauMonitoringTool():
#     tool = getattr(ToolSvc, name.split('/')[-1])
# 
#     tool.EnableLumi = False
#     tool.OutputLevel = WARNING
#     # tool.DetailLevel = 10
# 
#     monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
#     monMan.AthenaMonTools += [ tool ]
