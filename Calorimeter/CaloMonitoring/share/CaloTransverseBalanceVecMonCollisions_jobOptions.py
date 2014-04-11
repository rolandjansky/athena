# CaloTransverseBalanceVecMon_jobOpt.py 
# Author      : FENG TIAN 
#----------------------------------------------------------

from CaloMonitoring.CaloMonitoringConf import CaloTransverseBalanceVecMon
 
from AthenaMonitoring.DQMonFlags import DQMonFlags


if DQMonFlags.monManEnvironment() == 'online':
   TG_NoTA = "run"
else:
   TG_NoTA = "lowStat"

GeV=1000

# no trigger awareness
CaloTransverseBalanceMonNoTA = CaloTransverseBalanceVecMon(
name           = "CaloTransverseBalanceMonNoTA",
# Time granularity: by default it is run. It can be: run,eventsblock,lumiblock,10minutes,20minutes,90minutes,fill,all.Uncomment to modify.
TimeGran = TG_NoTA,
Calo_LArNoisyAlg  = TRUE,
# variables cut used in gamma-jet balance
photonPTCut = 20*GeV,
)

ToolSvc += CaloTransverseBalanceMonNoTA 

CaloMon.AthenaMonTools += [ CaloTransverseBalanceMonNoTA ]

