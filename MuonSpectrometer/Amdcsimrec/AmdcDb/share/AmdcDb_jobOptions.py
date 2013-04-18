include.block ( "AmdcDb/AmdcDb_jobOptions.py" )
#--------------------------------------------------------------
# AmdcDb
#--------------------------------------------------------------
from AmdcDb.AmdcDbConf import AmdcDb
ServiceMgr += AmdcDb()

#Oracle node
#ServiceMgr.AmdcDb.AMDBtag = "AMDB-R.01.01.Initial.Light_Egg_Rndm" ; 

#Set Ugly Code
ServiceMgr.AmdcDb.UglyCodeOn = 0 ; 

#Set Precisions
ServiceMgr.AmdcDb.FromAmdcEpsLengthMM = 4 ; 
ServiceMgr.AmdcDb.FromAmdcEpsLengthCM = 5 ; 
ServiceMgr.AmdcDb.FromAmdcEpsAngle    = 6 ; 
ServiceMgr.AmdcDb.FromRDBEpsLengthMM = 4 ; 
ServiceMgr.AmdcDb.FromRDBEpsLengthCM = 5 ; 
ServiceMgr.AmdcDb.FromRDBEpsAngle    = 6 ; 

