# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Slice='JET'
RunType='Physics'
Recostep='ESD'
MonitoringTriggers= [" ",]
Config=[]
Config+=[("HLTJetMon",{"EMFractionCut" : 0.8,} )]
PkgName = "TrigJetMonitoring.TrigJetMonitoringConfig"
CreatorName = "TrigJetMonitoringTool"
Script = "print 'Test script'"
