###############################################################
# author Caleb Lampen lampen@physics.arizona.edu 
#
#==============================================================
# cscCondCoolRead.py
# generates a calibration file from the CSC data in database
#Usage:
#athena.py -c "pslopeTag='pulserTag';runNumber=SomeRunNumber'
#
#Not specifying pslopeTag causes the tier0 tag to be loaded
#Not specifying runNumber causes a Very High run number to be set (may be invalid for reprocessing tags)

if('runNumber' not in dir()):
  runNumber = 3000000 #Bad for reprocessing tags!

import AthenaCommon.AtlasUnixStandardJob 
from AthenaCommon.AppMgr import theApp
theApp.EvtMax =1 

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
#CscReadWriteCoolStr to read a file from database

from MuonCondCool.MuonCondCoolConf import MuonCalib__CscReadWriteCoolStr
topSequence += MuonCalib__CscReadWriteCoolStr()
CscReadWriteCoolStr = MuonCalib__CscReadWriteCoolStr()
CscReadWriteCoolStr.Write=False
#Output stuff 
CscReadWriteCoolStr.Read = True
CscReadWriteCoolStr.oFile ="pulser.cal"
#03-00 is the old type. 04 will give you a more human readable type
CscReadWriteCoolStr.OutFileType ="04-00"
CscReadWriteCoolStr.OutParameters = ["pslope"]

print 'Preparing commissioning database COMP200 '
#---Set detector description tag
DetDescrVersion = "ATLAS-GEONF-08-00-00"

#----The next block will not have to be reproduced in some 
#----jobs. Top level joboptions like RecExCommon_topOptions.py
#----and RecExCommissionFlags_jobOptions.py usually set the appropriate flags.
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_commis() 	
#GlobalFlags.DataSource.set_geant4()#using simulation data
GlobalFlags.DataSource.set_data() #Using real data	
from IOVDbSvc.CondDB import conddb
include ("RecExCond/RecExCommon_flags.py")
DetFlags.ID_setOff()  #speeds up and prevents problems in commissioning database
DetFlags.Calo_setOff()
include ("RecExCond/AllDet_detDescr.py")

#Set up IOVDbService
#Pick a database tag. Should correspond with DetDescrVersion. See AtlasGeomDBTags page on twiki
conddb.setGlobalTag('COMCOND-ES1C-003-00')
Service("IOVDbSvc").forceRunNumber = runNumber;
Service("IOVDbSvc").forceLumiblockNumber = 1


from MuonCondSvc.CscCondDB import cscCondDB
#Read from local file. Must use before "addPedFolders"
cscCondDB.addPSlopeFolder()

if('pslopeTag' in dir()):
  print "Will override pusler slope to " + pslopeTag
  conddb.addOverride('/CSC/PSLOPE',pslopeTag)

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
#import AthenaPoolCnvSvc.ReadAthenaPool
#ServiceMgr.MessageSvc.OutputLevel      = VERBOSE
#ServiceMgr.MessageSvc.verboseLimit = 20000
#ServiceMgr.MessageSvc.debugLimit = 0
#IOVDbSvc = Service( "IOVDbSvc" )
#IOVDbSvc.OutputLevel        = 2
#ServiceMgr.EventSelector.RunNumber		= 0
#ServiceMgr.EventSelector.FirstEvent	= 0
