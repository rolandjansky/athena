include.block ( "TrigT1CaloCalibConditions/L1CaloCalibConditions_jobOptions.py" )

include("TrigT1CaloCalibConditions/L1CaloCalibConditionsTier0Run1_jobOptions.py")

from IOVDbSvc.CondDB import conddb
L1CaloFolderList = []
L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Calibration/Calib1/PprChanCalib"]
L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Calibration/Calib2/PprChanCalib"]
L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Calibration/Cosmics/PprChanCalib"]
L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Configuration/ReadoutConfig"]
L1CaloFolderList += ["/TRIGGER/Receivers/Conditions/VgaDac"]
L1CaloFolderList += ["/TRIGGER/Receivers/Conditions/Strategy"]
L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Conditions/RunParameters"]
L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Conditions/DerivedRunPars"]

for l1calofolder in L1CaloFolderList:
	conddb.addFolder("TRIGGER", l1calofolder+"<tag>HEAD</tag>")
