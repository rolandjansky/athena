# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Slice='CALOESD'
RunType='Cosmics'
Recostep='ESD'
MonitoringTriggers= [" ",]                        
Config=[]
Config+= [("HLTCaloL2", {}),("HLTCaloCluster", {})]
PkgName = "TrigCaloMonitoring.TrigCaloMonitoringConfig"
CreatorName = "HLTCaloMonitoringTool"
Script = "print 'Test script'"
