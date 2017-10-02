import InDetPhysValMonitoring.InDetPhysValDecoration
decoration = InDetPhysValMonitoring.InDetPhysValDecoration.addDecoratorIfNeeded()

# add ID physics validation monitoring tool

from InDetPhysValMonitoring.InDetPhysValMonitoringTool import InDetPhysValMonitoringTool

indet_mon_tool = InDetPhysValMonitoringTool.InDetPhysValMonitoringTool()
ToolSvc += [indet_mon_tool] 
monMan.AthenaMonTools += [indet_mon_tool] 

if InDetPhysValFlags.doValidateLooseTracks():
    indet_mon_tool_Loose = InDetPhysValMonitoringTool.InDetPhysValMonitoringToolLoose()
    ToolSvc += [ indet_mon_tool_Loose ]
    monMan.AthenaMonTools += [ indet_mon_tool_Loose ] 

if InDetPhysValFlags.doValidateTightPrimaryTracks():
    indet_mon_tool_TightPrimary = InDetPhysValMonitoringTool.InDetPhysValMonitoringToolTightPrimary()
    ToolSvc += [ indet_mon_tool_TightPrimary ]
    monMan.AthenaMonTools += [ indet_mon_tool_TightPrimary ] 


