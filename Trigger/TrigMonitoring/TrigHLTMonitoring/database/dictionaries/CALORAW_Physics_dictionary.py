# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Slice='CALORAW'
RunType='Physics'
Recostep='RAW'
MonitoringTriggers= [" ",]                        
Config=[]
Config+= [("HLTCalo", {}), ("HLTCaloFEB", {"sigma" : 4,})]
PkgName = "TrigCaloMonitoring.TrigCaloMonitoringConfig"
CreatorName = "HLTCaloMonitoringTool"
Script = "print 'Test script'"
