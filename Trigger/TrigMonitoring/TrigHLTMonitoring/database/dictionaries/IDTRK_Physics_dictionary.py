# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Slice='IDTRK'
RunType='Physics'
Recostep='ESD'
MonitoringTriggers= [ "BENS_MADE-UP_TRIGGER_NAME",]
Config=[]
Config+= [("MyAtlasExtrapolator", {}),
]
PkgName = "TrigIDtrkMonitoring.TrigIDtrkMonitoringConfig"
CreatorName = "TrigIDtrkMonitoringTool"
Script = "print 'Test script'"
