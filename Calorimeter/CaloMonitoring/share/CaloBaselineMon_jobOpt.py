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

tmp_CaloBaselineMon = {"useBadLBTool":FALSE,
                       "useReadyFilterTool":FALSE,
                       "useLArNoisyAlg":FALSE,
                       "useBeamBackgroundRemoval":FALSE,
                       "useLArCollisionFilter":FALSE}

if not (DQMonFlags.monManEnvironment == 'online' or globalflags.DataSource.get_Value() == 'geant4' or globalflags.DataSource.get_Value() == 'geant3'):
  tmp_CaloBaselineMon["useBadLBTool"]=TRUE
  tmp_CaloBaselineMon["useReadyFilterTool"]=TRUE
  tmp_CaloBaselineMon["useLArNoisyAlg"] = TRUE

if rec.triggerStream()=='CosmicCalo':
  tmp_CaloBaselineMon["useLArCollisionFilter"] = TRUE

CaloBaseline = CaloBaselineMon(
   name           = "CaloBaseline",

   useBadLBTool=tmp_CaloBaselineMon["useBadLBTool"],
   BadLBTool = GetLArBadLBFilterTool(),
   useReadyFilterTool = tmp_CaloBaselineMon["useReadyFilterTool"],
   ReadyFilterTool = monAtlasReadyFilterTool,
   useLArCollisionFilterTool = tmp_CaloBaselineMon["useLArCollisionFilter"],
   useLArNoisyAlg = tmp_CaloBaselineMon["useLArNoisyAlg"],
   useBeamBackgroundRemoval = tmp_CaloBaselineMon["useBeamBackgroundRemoval"],

)

ToolSvc += CaloBaseline 
CaloMon.AthenaMonTools += [ CaloBaseline ]
