# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Slice='TAU'
RunType='Standby'
Recostep='ESD'
MonitoringTriggers= ["TauNoCut", "EF_tau45",]                        
Config=[]
Config+= [("HLTTauMon", {"EffOffTauPtCut" : 15.0,})]
PkgName = "TrigTauMonitoring.TrigTauMonitoringConfig"
CreatorName = "TrigTauMonitoringTool"
Script = "print 'Test script'"
