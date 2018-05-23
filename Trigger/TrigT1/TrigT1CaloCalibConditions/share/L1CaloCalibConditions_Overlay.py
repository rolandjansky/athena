include.block ( "TrigT1CaloCalibConditions/L1CaloCalibConditions_Overlay.py" )

#print("JOHN IN OVERLAY DB L1CaloCalibConditions_Overlay.py")
## Overlay DB
from IOVDbSvc.CondDB import conddb
conddb.addFolderWithTag("TRIGGER_OFL","<key>/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib</key> /TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib","V2-PHYSICS-CHANCALIB-00-00",force=True,forceMC=True)
conddb.addFolderWithTag("TRIGGER_OFL","<key>/TRIGGER/L1Calo/V2/Configuration/PprChanDefaults</key> /TRIGGER/L1Calo/V2/Configuration/PprChanDefaults","V2-CHANDEFAULTS-00-00",force=True,forceMC=True)
conddb.addFolderWithTag("TRIGGER_OFL","<key>/TRIGGER/L1Calo/V2/Conditions/DisabledTowers</key> /TRIGGER/L1Calo/V2/Conditions/DisabledTowers","V2-DISABLEDTOWERS-00-00",force=True,forceMC=True)
conddb.addFolderWithTag("TRIGGER_OFL","<key>/TRIGGER/L1Calo/V2/Calibration/PpmDeadChannels</key> /TRIGGER/L1Calo/V2/Calibration/PpmDeadChannels","V2-DEADCHANNELS-00-00",force=True,forceMC=True)

conddb.addFolderWithTag("TRIGGER_ONL","<key>/TRIGGER/L1Calo/V2overlay/Calibration/Physics/PprChanCalib</key> /TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib","HEAD",force=True,forceData=True)
conddb.addFolderWithTag("TRIGGER_ONL","<key>/TRIGGER/L1Calo/V2overlay/Configuration/PprChanDefaults</key> /TRIGGER/L1Calo/V2/Configuration/PprChanDefaults","HEAD",force=True,forceData=True)
conddb.addFolderWithTag("TRIGGER_ONL","<key>/TRIGGER/L1Calo/V2overlay/Conditions/DisabledTowers</key> /TRIGGER/L1Calo/V1/Conditions/DisabledTowers","HEAD",force=True,forceData=True)
conddb.addFolderWithTag("TRIGGER_ONL","<key>/TRIGGER/L1Calo/V2overlay/Calibration/PpmDeadChannels</key> /TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels","HEAD",force=True,forceData=True)


