#Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# CaloClusterVecMon_jobOpt.py   R. Kehoe 07/03/06
# CaloClusterVecVecMon_jobOpt.py   H. Hadavand 07/30/07 --migrate to MMTB
# CaloClusterVecVecMon_jobOpt.py   F Spano' 08/15/07 --change monMan to  CaloMon
# CaloClustMonCosmicsCollisions_jobOpt.py: new names updates for collisions  F Spano' 07/16/09
# Implementation of trigger Awareness: Ram Dhullipudi 11/24/09  
#---------------------------------------------------------
# Author      : The CaloMonitoring group
# Description : JO steering CaloTopoClusters monitoring for single beam and collisions
#
#----------------------------------------------------------

# to change: 1) a jet trigger, a min bias trigger, an em trigger, a very much biased etmiss trigger
from CaloMonitoring.CaloMonitoringConf import CaloClusterVecMon

if 'rec' not in dir():
   from RecExConfig.RecFlags import rec
 
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
#  tmp_useReadyFilterTool=TRUE
  tmp_useReadyFilterTool=FALSE

if DQMonFlags.monManEnvironment() == 'online':
   tmp_useLArNoisyAlg = FALSE
else:
   tmp_useLArNoisyAlg = TRUE

if DQMonFlags.monManEnvironment() == 'online':
   tmp_useBeamBackgroundRemoval = FALSE
else:
   tmp_useBeamBackgroundRemoval = TRUE

if not (rec.triggerStream()=='CosmicCalo'):
  tmp_useBeamBackgroundRemoval = FALSE
  printfunc ("not CosmicCalo stream")

printfunc ("tmp_useBeamBackgroundRemoval=", tmp_useBeamBackgroundRemoval)

CaloClustMonCosmicsNoTA = CaloClusterVecMon(
   name           = "CaloClustMonCosmicsNoTA",
   CaloClusterContainer = "CaloCalTopoClusters",
   #CaloClusterContainer = "CaloCalTopoCluster",

   TimeGran = tmp_timeGran,

   useBadLBTool=tmp_useBadLBTool,
   BadLBTool = GetLArBadLBFilterTool(),

   useReadyFilterTool = tmp_useReadyFilterTool,
   ReadyFilterTool = GetAtlasReadyFilterTool(),

   useLArNoisyAlg = tmp_useLArNoisyAlg,
  
   useBeamBackgroundRemoval = tmp_useBeamBackgroundRemoval,

   # cluster energy threshold in GeV
   lowEthresh = 0.0,  
   lowmedEthresh = 5.0,
   medEthresh = 10,
   medhiEthresh = 15.0,
   hiEthresh = 20.0,
)

#ToolSvc += CaloClustMonCosmicsNoTA 
CaloMon.AthenaMonTools += [ CaloClustMonCosmicsNoTA ]

