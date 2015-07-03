import InDetPhysValMonitoring.InDetPhysValDecoration
decoration = InDetPhysValMonitoring.InDetPhysValDecoration.addDecoratorIfNeeded()

# add ID physics validation monitoring tool

from InDetPhysValMonitoring.InDetPhysValMonitoringTool import InDetPhysValMonitoringTool
indet_mon_tool = InDetPhysValMonitoringTool.InDetPhysValMonitoringTool()

ToolSvc += indet_mon_tool
monMan.AthenaMonTools += [indet_mon_tool]


