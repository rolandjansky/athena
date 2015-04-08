include.block(
    "TrigT1CaloCalibConditions/L1CaloCalibConditionsSqlite_jobOptions.py")

dbpath = "/afs/cern.ch/atlas/project/tdaq/level1/calo/data/calib/calib.sqlite"

import os.path
if not os.path.isfile(dbpath):
    print "ERROR from L1CaloCalibConditionsSqlite_jobOptions.py: could not find database %s" % dbpath


from IOVDbSvc.CondDB import conddb
L1CaloFolderList = []
L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Calibration/Calib1/PprChanCalib"]
L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Calibration/Calib2/PprChanCalib"]
L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib"]

L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Configuration/PprChanDefaults"]
L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Configuration/PprMcmDefaults"]
L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Configuration/ReadoutConfig"]

L1CaloOracleFolderList = []
L1CaloOracleFolderList += ["/TRIGGER/Receivers/Conditions/VgaDac"]
L1CaloOracleFolderList += ["/TRIGGER/Receivers/Conditions/Strategy"]
L1CaloOracleFolderList += ["/TRIGGER/L1Calo/V1/Conditions/RunParameters"]
L1CaloOracleFolderList += ["/TRIGGER/L1Calo/V1/Conditions/DerivedRunPars"]

for l1calofolder in L1CaloFolderList:
	conddb.addFolder("TRIGGER", "<dbConnection>sqlite://;schema=%s;dbname=L1CALO</dbConnection>%s<tag>HEAD</tag>" % (dbpath, l1calofolder))

for l1calofolder in L1CaloOracleFolderList:
    conddb.addFolderWithTag("TRIGGER", l1calofolder, "HEAD")
