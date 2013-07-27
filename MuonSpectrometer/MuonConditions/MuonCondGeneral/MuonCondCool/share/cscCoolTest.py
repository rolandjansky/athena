###############################################################
# author Caleb Lampen lampen@physics.arizona.edu 
# for any info and password to database
#
# cscCoolTest.py - Goes with cscCoolTest, an example of how to use
# CscCondCoolStrSvc.
#==============================================================
#Run number to read from COOL
#Set to -1 to not set
if("forceRunNumber" not in dir()):
  forceRunNumber = 2147483647 #Set very high so that is at end of last IOV

#Setup as standard job
import AthenaCommon.AtlasUnixStandardJob
from AthenaCommon.AppMgr import theApp
theApp.EvtMax =1

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#load main algorithms
from MuonCondCool.MuonCondCoolConf import CscCoolTest
topSequence += CscCoolTest()
CscCoolTest = CscCoolTest()
CscCoolTest.StripHash = 1300
CscCoolTest.DoStatusTest = True #count number of problem channels from status flags

#----Set detector description tag
DetDescrVersion = "ATLAS-GEONF-08-00-00"

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_commis() 	 
GlobalFlags.DataSource.set_data()  #Using real data database, not simulation	

#----The next block will not have to be reproduced in some 
#----jobs. Top level joboptions like RecExCommon_topOptions.py
#----and RecExCommissionFlags_jobOptions.py usually set the appropriate flags.
include ("RecExCond/RecExCommon_flags.py")
DetFlags.ID_setOff()  #speeds up and prevents problems in commissioning database
DetFlags.Calo_setOff()
include ("RecExCond/AllDet_detDescr.py")

#Set up IOVDbService
from IOVDbSvc.CondDB import conddb
#Pick a database tag. 
#conddb.setGlobalTag('COMCOND-REPC-001-00')
conddb.setGlobalTag('COMCOND-BLKPS-004-05')
if(forceRunNumber > 0):
  Service("IOVDbSvc").forceRunNumber = forceRunNumber;
  Service("IOVDbSvc").forceLumiblockNumber = 1


        
#Prepare db access strings in CscCoolStrSvc_read_jobOptions.py
from MuonCondSvc.CscCondDB import cscCondDB
#add pedestal folders
cscCondDB.addGainFolder()
cscCondDB.addPedFolders()
cscCondDB.addF001Folder()
cscCondDB.addRmsFolder()
cscCondDB.addStatusFolder('CscStat-REF-001-00')
#cscCondDB.addStatusFolder()
cscCondDB.addPSlopeFolder()

#------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
#MessageSvc = Service( "MessageSvc" )
#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.OutputLevel    = 3
##EventSelector = Service ( "EventSelector" )
#EventSelector.RunNumber		= 9 
#EventSelector.EventsPerRun      = 2 
#EventSelector.FirstEvent	= 1

