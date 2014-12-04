# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Slice='CALOESD'
RunType='Physics'
Recostep='ESD'
MonitoringTriggers= [" ",]                        
Config=[]
Config+= [("HLTCaloL2", {"ListOfCellsKeys":['HLT_TrigT2CaloTauCells', ]}),("HLTCaloCluster", {})]
PkgName = "TrigCaloMonitoring.TrigCaloMonitoringConfig"
CreatorName = "HLTCaloMonitoringTool"
Script = "print 'Test script'"
