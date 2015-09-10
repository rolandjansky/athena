# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Slice='CALORAW'
RunType='Cosmics'
Recostep='RAW'
MonitoringTriggers= [" ",]                        
Config=[]
Config+= [("HLTCalo", {}), ("HLTCaloFEB", {"sigma" : 3,})]
PkgName = "TrigCaloMonitoring.TrigCaloMonitoringConfig"
CreatorName = "HLTCaloMonitoringTool"
Script = "print 'Test script'"
