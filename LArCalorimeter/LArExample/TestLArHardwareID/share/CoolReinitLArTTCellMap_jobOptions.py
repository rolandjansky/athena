###############################################################


# This flag should remain True,if you just want to test the new object
# if False: enable COOL write
#TestOnly = True
TestOnly = False



#==============================================================

# Algorithm to (re-)initialize the LArTTCell map  
include("TestLArHardwareID/reinitTTMap_poolTestWrite_jobOptions.py")
reinitTTMap_Algo.testOnly = TestOnly
 
##############################################

# Writing POOL and COOL 
if not TestOnly :

 # This flag should remain False,if you just want to write new objects
 #RecreateFolder  = True
 RecreateFolder  = False

 # Following data now hardcoded in the .cxx
 # Objects  and its tag
 #ObjectList = []
 #TagList = []
 # output flags
 #ObjectList +=["LArTTCellMap#LArTTCellMapAtlas#/LAR/Identifier/LArTTCellMapAtlas"]
 #TTCellTag = "HadFcalFix"
 #TagList += ["LARIdentifierLArTTCellMapAtlas-"+TTCellTag]
 #OutputPOOLFileName = "LArTTCellMap-"+TTCellTag+".pool.root"


 include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")
 reinitTTMap_Algo.begRun = IOVRunNumberMin
 reinitTTMap_Algo.endRun = IOVRunNumberMax
 reinitTTMap_Algo.begLB = IOVLBNumberMin
 reinitTTMap_Algo.endLB = IOVLBNumberMax

 import RegistrationServices.IOVRegistrationSvc
 regSvc = svcMgr.IOVRegistrationSvc

 # Select the following to delete and recreate the folders. Default is
 # NOT to recreate - then content of a folder is delete before being recreated
 regSvc.RecreateFolders = RecreateFolder


 #--------------------------------------------------------------
 # Set the connection string
 #--------------------------------------------------------------
 
 from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
 
 # Local sqlite file
 IOVDbSvc.dbConnection="sqlite://;schema=ttcellmap.db;dbname=TMCP200"
 

##############################################
