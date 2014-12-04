# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Slice='MINBIAS'
RunType='Cosmics'
Recostep='ESD'
MonitoringTriggers= [ " ",]                        
Config=[]
#Config+=[("MbtsMinBiasMon", {}), ("BcmMinBiasMon", {}), ("HLTMinBiasMon", {}), ("LucidMinBiasMon",{}), ("ZdcMinBiasMon",{})] 
Config+=[("HLTMinBiasMon", {})] 
PkgName = "TrigMinBiasMonitoring.TrigMinBiasMonitoringConfig"
CreatorName = "TrigMinBiasMonitoringTool"
Script = "print 'Test script'"
