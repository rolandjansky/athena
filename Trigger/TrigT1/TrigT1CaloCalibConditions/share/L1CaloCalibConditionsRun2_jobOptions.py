include.block ( "TrigT1CaloCalibConditions/L1CaloCalibConditions_jobOptions.py" )

include("TrigT1CaloCalibConditions/L1CaloCalibConditionsTier0Run2_jobOptions.py")

from IOVDbSvc.CondDB import conddb
L1CaloFolderList = []
L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib"]
L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib"]
L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Conditions/RunParameters"]
L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Conditions/DerivedRunPars"]
L1CaloFolderList += ["/TRIGGER/Receivers/Conditions/VgaDac"]
L1CaloFolderList += ["/TRIGGER/Receivers/Conditions/Strategy"]

# In new version
#L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib"]

# Not exists now
#L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Calibration/Cosmics/PprChanCalib"]
#L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Configuration/ReadoutConfig"]

for l1calofolder in L1CaloFolderList:
	conddb.addFolderWithTag("TRIGGER", l1calofolder, "HEAD")
