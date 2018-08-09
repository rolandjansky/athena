#---------------------------------------------------------
# Author      : B.Trocme (LPSC Grenoble) 
# Description : JO steering CaloBaselineMon
#
#----------------------------------------------------------

from CaloMonitoring.CaloMonitoringConf import CaloBaselineMon
 
from AthenaMonitoring.DQMonFlags import DQMonFlags
from AthenaCommon.GlobalFlags  import globalflags
if not 'rec' in dir():
   from RecExConfig.RecFlags import rec

from AthenaMonitoring.BadLBFilterTool import GetLArBadLBFilterTool
include ("AthenaMonitoring/AtlasReadyFilterTool_jobOptions.py")

from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool

tmp_CaloBaselineMon = {"useBadLBTool":FALSE,
                       "useReadyFilterTool":FALSE,
                       "useLArNoisyAlg":FALSE,
                       "useBeamBackgroundRemoval":FALSE,
                       "useLArCollisionFilter":FALSE,
                       "pedestalMon_BCIDmin":0,
                       "bcidtoolMon_BCIDmax":0}

if not (DQMonFlags.monManEnvironment == 'online' or globalflags.DataSource.get_Value() == 'geant4' or globalflags.DataSource.get_Value() == 'geant3'):
  tmp_CaloBaselineMon["useBadLBTool"]=TRUE
  tmp_CaloBaselineMon["useReadyFilterTool"]=TRUE
  tmp_CaloBaselineMon["useLArNoisyAlg"] = TRUE

if rec.triggerStream()=='CosmicCalo':
  tmp_CaloBaselineMon["useLArCollisionFilter"] = TRUE
  tmp_CaloBaselineMon["pedestalMon_BCIDmin"] = 40
  tmp_CaloBaselineMon["TriggerChain"] = "HLT_noalg_cosmiccalo_L1RD1_EMPTY"

if rec.triggerStream()=='ZeroBias':
  tmp_CaloBaselineMon["bcidtoolMon_BCIDmax"] = 144
  tmp_CaloBaselineMon["TriggerChain"] = "HLT_noalg_zb_L1ZB"

CaloBaseline = CaloBaselineMon(
   name           = "CaloBaseline",
   
   useBadLBTool=tmp_CaloBaselineMon["useBadLBTool"],
   BadLBTool = GetLArBadLBFilterTool(),
   BunchCrossingTool=BunchCrossingTool(),
   useReadyFilterTool = tmp_CaloBaselineMon["useReadyFilterTool"],
   ReadyFilterTool = monAtlasReadyFilterTool,
   useLArCollisionFilterTool = tmp_CaloBaselineMon["useLArCollisionFilter"],
   useLArNoisyAlg = tmp_CaloBaselineMon["useLArNoisyAlg"],
   useBeamBackgroundRemoval = tmp_CaloBaselineMon["useBeamBackgroundRemoval"],
   pedestalMon_BCIDmin = tmp_CaloBaselineMon["pedestalMon_BCIDmin"],
   bcidtoolMon_BCIDmax = tmp_CaloBaselineMon["bcidtoolMon_BCIDmax"],
   TriggerChain = tmp_CaloBaselineMon["TriggerChain"]
)

ToolSvc += CaloBaseline 
CaloMon.AthenaMonTools += [ CaloBaseline ]
