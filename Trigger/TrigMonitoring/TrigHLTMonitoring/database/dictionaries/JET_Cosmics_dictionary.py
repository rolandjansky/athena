# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Slice='JET'
RunType='Cosmics'
Recostep='ESD'
MonitoringTriggers= []
Config=[]
Config+=[("HLTJetMon",{"EMFractionCut" : 0.7,} )]
PkgName = "TrigJetMonitoring.TrigJetMonitoringConfig"
CreatorName = "TrigJetMonitoringTool"
Script = "print 'Test script'"
