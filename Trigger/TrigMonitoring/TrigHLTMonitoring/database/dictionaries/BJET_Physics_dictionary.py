# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Slice='BJET'
RunType='Physics'
Recostep='ESD'
MonitoringTriggers= [ " ",]                        
Config=[]
Config+= [("HLTBjetMon", {"Taggers" : ["IP1D", "IP2D", "IP3D"],})]
PkgName = "TrigBjetMonitoring.TrigBjetMonitoringConfig"
CreatorName = "TrigBjetMonitoringConfig"
Script = "print 'Test script'"
