print("Welcome in LArMonManager.py")

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "LArMon" )
LArMon = topSequence.LArMon
LArMon.FileKey = "AllMon"
LArMon.ManualDataTypeSetup = True
##LArMon.DataType            = "cosmics"
if online:
    LArMon.Environment         = "online"
    LArMon.CheckEveryNoEvents = 50
else:
    LArMon.Environment         = "user"
    LArMon.CheckEveryNoEvents = 9999999
LArMon.ManualRunLBSetup    = True
LArMon.Run                 = 1
LArMon.LumiBlock           = 1
