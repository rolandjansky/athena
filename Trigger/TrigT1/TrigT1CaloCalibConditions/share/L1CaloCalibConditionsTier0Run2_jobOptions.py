include.block ( "TrigT1CaloCalibConditions/L1CaloCalibConditionsTier0_jobOptions.py" )

# Setup the L1CaloCondSvc
#from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from TrigT1CaloCondSvc.TrigT1CaloCondSvcConf import L1CaloCondSvc
ServiceMgr += L1CaloCondSvc()

# Folders needed for Tier0 processing
from IOVDbSvc.CondDB import conddb
L1CaloFolderList = []
L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels"]
L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Conditions/DisabledTowers"]
L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Conditions/RunParameters"]
L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Configuration/PprChanDefaults"]
L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Configuration/PprChanDefaults"]

# In new version:
#L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Configuration/PprChanDefaults"]
#L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Configuration/PprMcmDefaults"]

for l1calofolder in L1CaloFolderList:
  if globalflags.DataSource() is "data":
    conddb.addFolderWithTag("TRIGGER", l1calofolder, "HEAD")
  if globalflags.DataSource() is not "data":
    conddb.addFolderWithTag("TRIGGER_ONL", l1calofolder, "HEAD")
