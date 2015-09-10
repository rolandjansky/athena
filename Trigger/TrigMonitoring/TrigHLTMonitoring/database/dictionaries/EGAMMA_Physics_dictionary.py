# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Slice='EGAMMA'
RunType='Physics'
Recostep='ESD'
MonitoringTriggers= [" ",]
Config=[]
#Config+=[("HLTEgammaMon", {"L2Et" : 3500.0,})]
Config+=[("HLTEgammaNavMon", {"L1EmClus" : 3500.0,})]
#Config+=[("HLTEgammaNavSigTEMon", {"L1EmIsol" : 98000.0,})] 
PkgName = "TrigEgammaMonitoring.TrigEgammaMonitoringConfig"
CreatorName = "HLTEgammaMonitoringTool"
Script = "print 'Test script'"
