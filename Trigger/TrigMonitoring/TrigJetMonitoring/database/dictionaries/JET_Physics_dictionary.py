# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Slice='JET'
RunType='Physics'
Recostep='ESD'
MonitoringTriggers= ["HLT_j60","HLT_j100","HLT_j260",]
Config=[]
Config+=[("HLTJetMon",{"EMFractionCut" : 0.8,} )]
PkgName = "TrigJetMonitoring.TrigJetMonitoringConfig"
CreatorName = "TrigJetMonitoringTool"
Script = "print 'Test script'"
