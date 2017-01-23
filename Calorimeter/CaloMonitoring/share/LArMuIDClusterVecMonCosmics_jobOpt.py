# LArMuIDVecMon_jobOpt.py   R. Kehoe 07/03/06
# LArMuIDVecMon_jobOpt.py   H. Hadavand 07/30/07 --migrate to MMTB
# LArMuIDVecMon_jobOpt.py   F. Spano' 08/15/07 --modify name of MonMan to CaloMon
# plots for CaloCluster 'LArMuID clusters' momentum vector monitoring
from CaloMonitoring.CaloMonitoringConf import CaloClusterVecMon

if not 'rec' in dir():
   from RecExConfig.RecFlags import rec

from AthenaCommon.GlobalFlags  import globalflags
from AthenaMonitoring.DQMonFlags import DQMonFlags

from AthenaMonitoring.BadLBFilterTool import GetLArBadLBFilterTool
include ("AthenaMonitoring/AtlasReadyFilterTool_jobOptions.py")

if (DQMonFlags.monManEnvironment == 'online' or globalflags.DataSource.get_Value() == 'geant4' or globalflags.DataSource.get_Value() == 'geant3'):
  tmp_useBadLBTool=FALSE
else:
  tmp_useBadLBTool=TRUE

if DQMonFlags.monManEnvironment() == 'online':
  tmp_useReadyFilterTool=FALSE
else:
  tmp_useReadyFilterTool=FALSE
#  tmp_useReadyFilterTool=TRUE

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
  print "not CosmicCalo stream"

print "tmp_useBeamBackgroundRemoval=", tmp_useBeamBackgroundRemoval

LArCosmicsClusterMon = CaloClusterVecMon(
  name           = "LArCosmicsClusterMon",
  CaloClusterContainer = "LArMuClusterCandidates",

  TimeGran = "lowStat",

  useBadLBTool=tmp_useBadLBTool,
  BadLBTool = GetLArBadLBFilterTool(),

  useReadyFilterTool = tmp_useReadyFilterTool,
  ReadyFilterTool = monAtlasReadyFilterTool,

  useLArNoisyAlg = tmp_useLArNoisyAlg,
  
  useBeamBackgroundRemoval = tmp_useBeamBackgroundRemoval,

  # cluster energy threshold in GeV
  lowEthresh = 0.0,  
  lowmedEthresh = 1.0,
  medEthresh = 1.5,
#  medhiEthresh = 4.0,
  hiEthresh = 5.0,
)

ToolSvc += LArCosmicsClusterMon
CaloMon.AthenaMonTools += [ LArCosmicsClusterMon]
