# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Slice='IDTRK'
RunType='Standby'
Recostep='ESD'
MonitoringTriggers= [ " ",]                        
Config=[]
Config+= [("MyAtlasExtrapolator", {}),
]
PkgName = "TrigIDtrkMonitoring.TrigIDtrkMonitoringConfig"
CreatorName = "TrigIDtrkMonitoringTool"
Script = "print 'Test script'"
