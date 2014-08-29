include.block ( "MuonCondSvc/CscCoolStrSvc_read_jobOptions.py" )
###############################################################
# author Caleb Lampen lampen@physics.arizona.edu 
#
#CscCoolStrSvc_read_jobOptions.py
#Sets up the IOVDbSvc to read from the Cathode Strip Chambers'
#conditions folders. (In this file CSC = Cathode Strip Chambers)
#==============================================================

#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#This file is no longer needed since the move to configurables
#, and is retained for backwards compatibility only. Now to
#setup the Csc conditions database service, simply add the line:

msg = logging.getLogger('CscCoolStrSvc_read_jobOptions.py')
print "MuonCondSvc/CscCoolStrSvc_read_jobOptions.py is old. It only initializes the database"
print "for reading pedestals and noise. If you want other parameters, you should import the module directly"
print "and add the database folders you want as described in the CscCoolAccess twiki page" 
from MuonCondSvc.CscCondDB import cscCondDB
cscCondDB.addPedFolders()
#To your joboptions.  
#
#The instructions below are still essentially correct, although
#it would be best to use the configurable methods for IOVDbSvc
#where applicable. See the CoolIOVDbSvcConfigurable Twiki page
#for details.
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#Setup instructions
#Here is what must be specified in joboptions prior to including this file

#
#Much of the setup is automatically handled by most reconstruction topoptions. I've sorted
#It roughly most to least likely that its handled in some file you're already including. Also
#check MuonCondCool/cscCoolTest.py and MuonCondCool/cscCondCoolRead.py for examples.
#If you need more help, or you think something is missing form these instructions,
#please email me at lampen@physics.arizona.edu.
#
#Database selection
#	The CscCondDB.CSC_ONL string (used in conddb.addFolder), which specifies the database (CMCP200, COMP200, or OFLP200)
#	 is setup automatically in CondDBSetup.py. GlobalFlags.DetGeo and GlobalFlags.DataSource 
# 	must be set for this to work. They are usually set in reconstruction joboptions, however
#	in some situations you may want to set it yourself. To do this, add the follwing lines 
#	in your joboptions before including reconstruction files that set it for you:
#		from AthenaCommon.GlobalFlags import GlobalFlags
#		GlobalFlags.DetGeo.set_commis()		#For commssionging databases (CMCP200 or COMP200)
#		GlobalFlags.DetGeo.set_atlas() 		#For production databases (OFLP200)
#		GlobalFlags.DataSource.set_data()	#Set for COMP200, otherwise with set_commis you get COMP200
#Geometry
#	Make sure you have set up your Geometry correctly setup up, with the 
#	proper geometry tag. For example, DetDescrVersion = "ATLAS-Comm-01-00-00
# 	for commisioning data. Tags are recorded in the AtlasGeomDBTags
#IOVDbSvc.GlobalTag
#	You must set a database global tag with the line IOVDbSvc.GlobalTag = 'tagname'. Check the
#	twiki page AtlasGeomDBTags to find which database tags go with which DetDescrVersion tags.
#	I believe this is the tag least likely to be set up previously in the reconstruction joboptions
#	For example:
#		IOVDbSvc.globalTag = 'OFLCOND-CSC-00-00-00' #For production database
#
#Finally, if you want to access the cosmic data (4-chamber setup), you add the following line to your 
#joboptions. This one specifically is requesting the S01-S05-S02-S04-00 setup as specified in the
#CscPreCommissioning twiki page.
#	IOVDbSvc.OverrideTags += ["<prefix>/CSC</prefix> <tag>Csc-cosmic-S01-S05-S02-S04-00</tag>"]
#
#######################################################################################################

#Define cool foldernames. These need to be given to both
##CoolStrSvc and IovDbSvc
#pulseGainFolder = "/CSC/PGAIN"
#pedFolder 	= "/CSC/PED"
#peaktFolder 	= "/CSC/PEAKT"
#sat1Folder 	= "/CSC/SAT1"
#sat2Folder	= "/CSC/SAT2"
#noiseFolder	= "/CSC/NOISE"
#pulseStatFolder ="/CSC/PSTAT"
#runGainFolder   ="/CSC/RGAIN"
#runStatFolder   ="/CSC/RSTAT"


################
# specify the COOL datatbase connection string
#include("IOVDbSvc/IOVDbSvc_jobOptions.py")
#IOVDbSvc = Service ( "IOVDbSvc" ) 
#from IOVDbSvc.CondDB import conddb
################
# set dbConnection string 
#IOVDbSvc.dbConnection = "impl=cool;techno=oracle;devdb10:CSCNEW:ATLAS_MUON_CSC"
#IOVDbSvc.dbConnection = "impl=cool;techno=logical;schema=ATLAS_COOLONL_CSC;X:OFLP200"
#IOVDbSvc.dbConnection = "impl=cool;techno=logical;schema=COOLONL_CSC;X:OFLP200"
#conddb.addFolder("CSC",pulseGainFolder)
#conddb.addFolder("CSC",pedFolder)
#conddb.addFolder("CSC",peaktFolder)
#conddb.addFolder("CSC",sat1Folder)
#conddb.addFolder("CSC",sat2Folder)
#conddb.addFolder("CSC",noiseFolder)
#conddb.addFolder("CSC",pulseStatFolder)
#conddb.addFolder("CSC",runGainFolder)
#conddb.addFolder("CSC",runStatFolder)

