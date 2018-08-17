from AmdcDb.AmdcDbConf import AmdcDb
AmdcDb.UglyCodeOn = 0
AmdcDb.FromAmdcEpsLengthMM = 4
AmdcDb.FromAmdcEpsLengthCM = 4
AmdcDb.FromAmdcEpsAngle    = 6
AmdcDb.FromRDBEpsLengthMM = 4
AmdcDb.FromRDBEpsLengthCM = 4
AmdcDb.FromRDBEpsAngle    = 6

#AmdcDb Setting Table by Table (to be used starting from MuonSpectrometer-R.04.06)
AmdcDb.TabFromRDBEpsLengthMM  = [ "AMDC" ,"DBAM" ,"WCSC" ,"WMDT" , ]
AmdcDb.ValFromRDBEpsLengthMM  = [ 6      ,6      ,6      ,6      , ]
AmdcDb.TabFromRDBEpsLengthCM  = [ "AMDC" ,"DBAM" ,"WCSC" ,"WMDT" , ] 
AmdcDb.ValFromRDBEpsLengthCM  = [ 6      ,6      ,6      ,6      , ] 

AmdcDb.TabFromAmdcEpsLengthMM = [ "AMDC" ,"DBAM" ,"WCSC" ,"WMDT" , ]
AmdcDb.ValFromAmdcEpsLengthMM = [ 6      ,6      ,6      ,6      , ]
AmdcDb.TabFromAmdcEpsLengthCM = [ "AMDC" ,"DBAM" ,"WCSC" ,"WMDT" , ]
AmdcDb.ValFromAmdcEpsLengthCM = [ 6      ,6      ,6      ,6      , ]
ServiceMgr += AmdcDb()
