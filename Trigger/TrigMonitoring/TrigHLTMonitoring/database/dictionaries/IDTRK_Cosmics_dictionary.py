# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Slice='IDTRK'
RunType='Cosmics'
Recostep='ESD'
MonitoringTriggers= [ " ",]                        
Config=[]
Config+= [("HLTIDpvtx", {"OnlineEfficiancyRangeCutY" : 0.02,}), ("HLTIDtrkDump", {"MinSiHits" : 7,}),
("tidatool", {}), ("MyAtlasExtrapolator", {}), ("MyCaloExtrapolatorTool", {}), ("HLTIDZeeTag", {})
]
PkgName = "TrigIDtrkMonitoring.TrigIDtrkMonitoringConfig"
CreatorName = "TrigIDtrkMonitoringTool"
Script = "print 'Test script'"
