#Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
# CaloTowerVecMon_jobOptions.py   L. Sawyer 12/02/2009
# Trigger awareness implementation Ram Dhullipudi 11/24/09

from CaloMonitoring.CaloMonitoringConf import CaloTowerVecMon

from AthenaMonitoring.DQMonFlags import DQMonFlags
from AthenaCommon.GlobalFlags  import globalflags

from AthenaMonitoring.BadLBFilterTool import GetLArBadLBFilterTool
from AthenaMonitoring.AtlasReadyFilterTool import GetAtlasReadyFilterTool

if DQMonFlags.monManEnvironment() == 'online':
   tmp_timeGran = "run"
else:
   tmp_timeGran = "lowStat"

if (DQMonFlags.monManEnvironment == 'online' or globalflags.DataSource.get_Value() == 'geant4' or globalflags.DataSource.get_Value() == 'geant3'):
  tmp_useBadLBTool=FALSE
else:
  tmp_useBadLBTool=TRUE

if DQMonFlags.monManEnvironment() == 'online':
  tmp_useReadyFilterTool=FALSE
else:
#  tmp_useReadyFilterTool=FALSE
  tmp_useReadyFilterTool=TRUE

if DQMonFlags.monManEnvironment() == 'online':
   tmp_useLArNoisyAlg = FALSE
else:
   tmp_useLArNoisyAlg = TRUE

tmp_useTrigger = TRUE
if 'DQMonFlags' in dir():
   if not DQMonFlags.useTrigger:
      tmp_useTrigger = FALSE

CaloTowerMonNoTA = CaloTowerVecMon(
   name           = "CaloTowerMonNoTA",
   CaloTowerContainer = "CombinedTower",

   TimeGran = tmp_timeGran,

   useBadLBTool=tmp_useBadLBTool,
   BadLBTool = GetLArBadLBFilterTool(),

   useReadyFilterTool = tmp_useReadyFilterTool,
   ReadyFilterTool = GetAtlasReadyFilterTool(),

   useLArNoisyAlg = tmp_useLArNoisyAlg,

   useTrigger = tmp_useTrigger,
   TriggerNames1 = "EF_mbZdc_a_c_L1VTE50_trk",
   TriggerNames2 = "EF_L1TE50_NoAlg",
   TriggerNames3 = "EF_mbZdc_a_c_eff",

   NoOfThreshTests=6,

   noEThresh = -99999.0,
   lowEThresh = 0.0,
   lowmedEThresh = 5.0,
   medEThresh = 10,
   medhiEThresh =15.0,
   hiEThresh = 20.0,

   fill_TransverseE=True,
   fill_TotalE=True,
)

#ToolSvc += CaloTowerMonNoTA 
CaloMon.AthenaMonTools += [ CaloTowerMonNoTA ]
