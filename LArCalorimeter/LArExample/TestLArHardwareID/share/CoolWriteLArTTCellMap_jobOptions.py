###############################################################
#
# Job options file 1
#
#==============================================================

#/-----------------------------------------
# Algorithm to initialize the LArTTCell map  
#/------------------------------- ---------
# "new FCAL" DC3-05-Comm-01
initFcalFromFile = True
# "old FCAL" DC3-02
#initFcalFromFile = False
include("TestLArHardwareID/initTTMap_poolTestWrite_jobOptions.py")
 

# This flag should remain True,if you just want to test the new object
# if False: enable POOL write
TestOnly = True
#TestOnly = False

# These flags should remain False,if you just want to write new objects
#RecreateFolder  = True
RecreateFolder  = False
#SchemaPrivilege = True
SchemaPrivilege = False 

# Objects  and its tag
ObjectList = []
TagList = []


#  Keep WriteIOV false, and OnOffIdTag and FebRodTag not defined, 
#   test the fix in the algorithm 
#  when everythingis tested OK, then enable WriteIOV and one of the tag. 

# output flags
if initFcalFromFile:
    TTCellTag = "DC3-05"
else:    
    TTCellTag = "DC3-02"

# Turn on flag for writing to COOL IOV
#WriteIOV = False
WriteIOV = True

if not TestOnly :
 ObjectList +=["LArTTCellMap#LArTTCellMapAtlas#/LAR/Identifier/LArTTCellMapAtlas"]
 TagList += ["LARIdentifierLArTTCellMapAtlas-"+TTCellTag]
 OutputPOOLFileName = "LArTTCellMap-"+TTCellTag+".pool.root"



##############################################

# Writing POOL and COOL 
if len(ObjectList)>0 : 

 # include regstration alg (default is WriteIOV = False)
 include("RegistrationServices/OutputConditionsAlg_jobOptions.py")

 # List of objects container type#key#foldername
 OutputConditionsAlg.ObjectList = ObjectList 
 OutputConditionsAlg.IOVTagList = TagList

 ToolSvc = Service("ToolSvc") 
 ToolSvc.ConditionsAlgStream.OutputFile = OutputPOOLFileName

 # Set flag to register and run interval Run1/Event1 to Run2/Event2
 #   Usually, only need to set Run1, others go to default
 #### 

 OutputConditionsAlg.WriteIOV = WriteIOV
 OutputConditionsAlg.Run1     = 0
 OutputConditionsAlg.LB1      = 0

 # Set the connection string
 include ( "IOVDbSvc/IOVDbSvc_jobOptions.py" )
 IOVDbSvc = Service( "IOVDbSvc" )

 if (SchemaPrivilege):
      # To recreate folder or add DB, need to use schema account:
      IOVDbSvc.dbConnection="impl=cool;techno=oracle;schema=ATLAS_COOL_LAR;ATLAS_COOLPROD:OFLP130:ATLAS_COOL_LAR:"
 else:
      # For db writing
      IOVDbSvc.dbConnection="impl=cool;techno=oracle;schema=ATLAS_COOL_LAR;ATLAS_COOLPROD:OFLP130:ATLAS_COOL_LAR_W:"


 # For schema creation - only should be used when creating the folder,
 # i.e. the first time
 IOVRegSvc = Service( "IOVRegistrationSvc" )
 IOVRegSvc.OutputLevel  = DEBUG
 if (RecreateFolder):
      IOVRegSvc.RecreateFolders = True	

include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )
#PoolSvc = Service( "PoolSvc" )
#include("/afs/cern.ch/atlas/maxidisk/d29/LArCalorimeter/data/oflp130/LArTTCellMap_"+TTCellTag+"_jobOptions.py")


##############################################
