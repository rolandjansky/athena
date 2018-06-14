# CaloClusterVecMon_jobOpt.py   R. Kehoe 07/03/06
# CaloClusterVecVecMon_jobOpt.py   H. Hadavand 07/30/07 --migrate to MMTB
# CaloClusterVecVecMon_jobOpt.py   F Spano' 08/15/07 --change monMan to  CaloMon
# CaloClusterMonCollisions_jobOpt.py: new names updates for collisions  F Spano' 07/16/09
# Implementation of trigger Awareness: Ram Dhullipudi 11/24/09  
#---------------------------------------------------------
# Author      : The CaloMonitoring group
# Description : JO steering CaloTopoClusters monitoring for single beam and collisions
#
#----------------------------------------------------------

from CaloMonitoring.CaloMonitoringConf import CaloClusterVecMon

if not 'rec' in dir():
   from RecExConfig.RecFlags import rec

from AthenaMonitoring.DQMonFlags import DQMonFlags
from AthenaCommon.GlobalFlags  import globalflags

from AthenaMonitoring.BadLBFilterTool import GetLArBadLBFilterTool
include ("AthenaMonitoring/AtlasReadyFilterTool_jobOptions.py")

tmp_CaloClusterVecMonCollisions = {"timeGran":"lowStat",
                                   "useBadLBTool":FALSE,
                                   "useReadyFilterTool":FALSE,
                                   "useLArNoisyAlg":FALSE,
                                   "useBeamBackgroundRemoval":FALSE,
                                   "useLArCollisionFilter":FALSE}


if DQMonFlags.monManEnvironment() == 'online':
   tmp_CaloClusterVecMonCollisions["timeGran"] = "run"

if not (DQMonFlags.monManEnvironment == 'online' or globalflags.DataSource.get_Value() == 'geant4' or globalflags.DataSource.get_Value() == 'geant3'):
  tmp_CaloClusterVecMonCollisions["useBadLBTool"]=TRUE
  tmp_CaloClusterVecMonCollisions["useReadyFilterTool"]=TRUE
  tmp_CaloClusterVecMonCollisions["useLArNoisyAlg"] = TRUE

if (rec.triggerStream()=='CosmicCalo'):
  tmp_CaloClusterVecMonCollisions["useLArCollisionFilter"] = TRUE
  tmp_CaloClusterVecMonCollisions["useBeamBackgroundRemoval"] = TRUE

CaloClusterMonNoTA = CaloClusterVecMon(
   name           = "CaloClusterMonNoTA",
   CaloClusterContainer = "CaloCalTopoClusters",
    
   TimeGran = tmp_CaloClusterVecMonCollisions["timeGran"],

   useBadLBTool=tmp_CaloClusterVecMonCollisions["useBadLBTool"],
   BadLBTool = GetLArBadLBFilterTool(),

   useReadyFilterTool = tmp_CaloClusterVecMonCollisions["useReadyFilterTool"],
   ReadyFilterTool = monAtlasReadyFilterTool,

   useLArCollisionFilterTool = tmp_CaloClusterVecMonCollisions["useLArCollisionFilter"],

   useLArNoisyAlg = tmp_CaloClusterVecMonCollisions["useLArNoisyAlg"],

   useBeamBackgroundRemoval = tmp_CaloClusterVecMonCollisions["useBeamBackgroundRemoval"],

   # cluster energy threshold in GeV
   lowEthresh = 0.0,  
   lowmedEthresh = 10.0,
   medEthresh = 25.0,
   hiEthresh = 50.0,
)

ToolSvc += CaloClusterMonNoTA
CaloMon.AthenaMonTools += [ CaloClusterMonNoTA ]

