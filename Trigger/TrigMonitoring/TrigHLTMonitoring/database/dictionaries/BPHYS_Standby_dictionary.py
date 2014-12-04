# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Slice='BPHYS'
RunType='Standby'
Recostep='ESD'
MonitoringTriggers= [ " ",]                        
Config=[]
Config+= [("HLTBphysMon", {})]
PkgName = "TrigBphysMonitoring.TrigBphysMonitoringConfig"
CreatorName = "TrigBphysMonitoringTool"
Script = "print 'Test script'"
