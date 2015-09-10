# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Slice='MUON'
RunType='Cosmics'
Recostep='ESD'
MonitoringTriggers = [ " ",]                        
Config = []
Config += [("HLTMuonMon", {"ZTPPtCone20RelCut" : 0.15,}),]
PkgName = "TrigMuonMonitoring.TrigMuonMonitoringConfig"
CreatorName = "TrigMuonMonitoringTool"
Script = "print 'Test script'"
