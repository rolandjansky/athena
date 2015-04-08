include.block ( "TrigT1CaloCalibConditions/L1CaloCalibConditionsTier0_jobOptions.py" )

# Setup the L1CaloCondSvc
#from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from TrigT1CaloCondSvc.TrigT1CaloCondSvcConf import L1CaloCondSvc
ServiceMgr += L1CaloCondSvc()

# Folders needed for Tier0 processing
from IOVDbSvc.CondDB import conddb
L1CaloFolderList = []
L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Configuration/PprChanDefaults"]
L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib"]
L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels"]
L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Conditions/DisabledTowers"]

for l1calofolder in L1CaloFolderList:
	conddb.addFolder("TRIGGER", l1calofolder+"<tag>HEAD</tag>")
