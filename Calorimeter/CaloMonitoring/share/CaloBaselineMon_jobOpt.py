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

if (DQMonFlags.monManEnvironment == 'online' or globalflags.DataSource.get_Value() == 'geant4' or globalflags.DataSource.get_Value() == 'geant3'):
  tmp_useBadLBTool=FALSE
  tmp_useReadyFilterTool=FALSE
  tmp_useLArNoisyAlg = FALSE
  tmp_useBeamBackgroundRemoval = FALSE
else:
  tmp_useBadLBTool=TRUE
  tmp_useReadyFilterTool=TRUE
  tmp_useLArNoisyAlg = TRUE
  tmp_useBeamBackgroundRemoval = FALSE

if rec.triggerStream()=='CosmicCalo':
  tmp_useLArCollisionFilterTool = TRUE
else:
  tmp_useLArCollisionFilterTool = FALSE

CaloBaseline = CaloBaselineMon(
   name           = "CaloBaseline",

   useBadLBTool=tmp_useBadLBTool,
   BadLBTool = GetLArBadLBFilterTool(),
   useReadyFilterTool = tmp_useReadyFilterTool,
   ReadyFilterTool = monAtlasReadyFilterTool,
   useLArCollisionFilterTool = tmp_useLArCollisionFilterTool,
   useLArNoisyAlg = tmp_useLArNoisyAlg,
   useBeamBackgroundRemoval = tmp_useBeamBackgroundRemoval,

)

ToolSvc += CaloBaseline 
CaloMon.AthenaMonTools += [ CaloBaseline ]



