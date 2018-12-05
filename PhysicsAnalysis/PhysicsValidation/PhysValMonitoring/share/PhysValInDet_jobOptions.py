import InDetPhysValMonitoring.InDetPhysValDecoration
decoration = InDetPhysValMonitoring.InDetPhysValDecoration.addDecoratorIfNeeded()

# add ID physics validation monitoring tool

from InDetPhysValMonitoring.InDetPhysValMonitoringTool import InDetPhysValMonitoringTool
from InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags

indet_mon_tool = InDetPhysValMonitoringTool.InDetPhysValMonitoringTool()
monMan.AthenaMonTools += [indet_mon_tool] 

if InDetPhysValFlags.doValidateDBMTracks():
    indet_mon_tool_DBM = InDetPhysValMonitoringTool.InDetPhysValMonitoringToolDBM()
    monMan.AthenaMonTools += [ indet_mon_tool_DBM ] 

if InDetPhysValFlags.doValidateGSFTracks():
    indet_mon_tool_GSF = InDetPhysValMonitoringTool.InDetPhysValMonitoringToolGSF()
    monMan.AthenaMonTools += [ indet_mon_tool_GSF ]

if InDetPhysValFlags.doValidateLooseTracks():
    indet_mon_tool_Loose = InDetPhysValMonitoringTool.InDetPhysValMonitoringToolLoose()
    monMan.AthenaMonTools += [ indet_mon_tool_Loose ] 

if InDetPhysValFlags.doValidateTightPrimaryTracks():
    indet_mon_tool_TightPrimary = InDetPhysValMonitoringTool.InDetPhysValMonitoringToolTightPrimary()
    monMan.AthenaMonTools += [ indet_mon_tool_TightPrimary ] 
