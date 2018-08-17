from AmdcDb.AmdcDbConf import AmdcDb 
AmdcDb.NameOfTheSource = "ORACLEHARD"
AmdcDb.AMDBtag = database_layout
AmdcDb.UglyCodeOn = 0
AmdcDb.FromAmdcEpsLengthMM = 4
AmdcDb.FromAmdcEpsLengthCM = 4
AmdcDb.FromAmdcEpsAngle    = 6
AmdcDb.FromRDBEpsLengthMM = 4
AmdcDb.FromRDBEpsLengthCM = 4
AmdcDb.FromRDBEpsAngle    = 6
ServiceMgr += AmdcDb()
