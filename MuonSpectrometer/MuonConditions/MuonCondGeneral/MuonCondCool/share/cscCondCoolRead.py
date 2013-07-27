###############################################################
# author Caleb Lampen lampen@physics.arizona.edu 
#
#==============================================================
# cscCondCoolRead.py # generates a calibration file from the data in database
# also meant to be a bit of an example joboptions file of the settings that must be set


#Set which parameters to read from SQLite file
if('readPars' not in dir()):
  readPars = ["ped","rms","noise","f001","status"]
  readPars += ["pslope"]

if('readDb' not in dir()):
  readDb = "COMP200"

if('output' not in dir()):
  output = 'mycool.cal'

#Run number to read from COOL
#Set to -1 to not set
if("forceRunNumber" not in dir()):
  forceRunNumber = 2147483647 #Set very high so that is at end of last IOV


if('readFromLocalFile' not in dir()):
  readFromLocalFile = False

#Need to override to tags used in SQLite file, as we can't count on Global tag to 
#Resolve correctly
if('pedOverride' not in dir()):
  pedOverride = "CscPed-REF-001-00"
if('noiseOverride' not in dir()):
  noiseOverride = "CscNoise-REF-001-00"
if('rmsOverride' not in dir()):
  rmsOverride = "CscRms-REF-001-00"
if('f001Override' not in dir()):
  f001Override = "CscFthold-REF-001-00"
if('statOverride' not in dir()):
  statOverride = "CscStat-REF-001-00"
if('t0phaseOverride' not in dir()):
  t0phaseOverride = "CscT0phase-NULL-001-00"
if('t0baseOverride' not in dir()):
  t0baseOverride = "CscT0base-NULL-001-00"
if('pslopeOverride' not in dir()):
  pslopeOverride = "CscPslope-val-001-00"

if('forceReadAsChannelCategory' not in dir()):
  forceReadAsChannelCategory = False


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
CscReadWriteCoolStr.oFile = output
#01-00 is the "online" filetype using online ids. 00-00 is the offline datatype that comes from the
#CscDoCalib algorithm
CscReadWriteCoolStr.OutFileType ="04-00"
CscReadWriteCoolStr.OutParameters = readPars
if(forceReadAsChannelCategory):
  CscReadWriteCoolStr.ForceReadAsChannelCategory = True

print 'Preparing commissioning database COMP200 '
#---Set detector description tag
DetDescrVersion = "ATLAS-GEONF-08-00-00"

#----The next block will not have to be reproduced in some 
#----jobs. Top level joboptions like RecExCommon_topOptions.py
#----and RecExCommissionFlags_jobOptions.py usually set the appropriate flags.
#>>from AthenaCommon.GlobalFlags import GlobalFlags #GlobalFlags
#>>GlobalFlags.DetGeo.set_atlas() 	
#>>if(readDb == "OFLP200"):
#>>  GlobalFlags.DataSource.set_geant4()
#>>elif(readDb == "COMP200"): 
#>>  GlobalFlags.DataSource.set_data();
#>>else:
#>>  print "ERROR! readDb = " + readDb + " is invalid! Must be either COMP200 or OFLP200!"
from AthenaCommon.GlobalFlags import globalflags #GlobalFlags
globalflags.DetGeo = 'atlas' 
if(readDb == "OFLP200"):
  globalflags.DataSource = 'geant4'
elif(readDb == "COMP200"): 
  globalflags.DataSource = 'data'
else:
  print "ERROR! readDb = " + readDb + " is invalid! Must be either COMP200 or OFLP200!"

from IOVDbSvc.CondDB import conddb
include ("RecExCond/RecExCommon_flags.py")
DetFlags.ID_setOff()  #speeds up and prevents problems in commissioning database
DetFlags.Calo_setOff()
include ("RecExCond/AllDet_detDescr.py")

#Set up IOVDbService
#Pick a database tag. Should correspond with DetDescrVersion. See AtlasGeomDBTags page on twiki
#conddb.setGlobalTag('COMCOND-006-01')
conddb.setGlobalTag('COMCOND-BLKPS-004-05')
if(forceRunNumber > 0):
  Service("IOVDbSvc").forceRunNumber = forceRunNumber;
  Service("IOVDbSvc").forceLumiblockNumber = 1

from MuonCondSvc.CscCondDB import cscCondDB
#Read from local file. Must use before "addPedFolders"
#cscCondDB.setFolderSuffix("<dbConnection>sqlite://;schema=mod.db;dbname=COMP200</dbConnection>")

if(readFromLocalFile):
  cscCondDB.useLocalFile()

from MuonCondSvc.CscCoolFolders import dict as coolFolders

#Set pedestals
#Note if override strings are "", no overide will occur
if("ped" in readPars):
  cscCondDB.addPedFolder(pedOverride)
if("noise" in readPars):
  cscCondDB.addNoiseFolder(noiseOverride)
if("rms" in readPars):
  cscCondDB.addRmsFolder(rmsOverride)
if("f001" in readPars):
  cscCondDB.addF001Folder(f001Override)
if("status" in readPars):
  cscCondDB.addStatusFolder(statOverride)
if("pslope" in readPars):
  cscCondDB.addPSlopeFolder(pslopeOverride)
if("t0phase" in readPars):
  cscCondDB.addT0PhaseFolder(t0phaseOverride)
if("t0base" in readPars):
  cscCondDB.addT0BaseFolder(t0baseOverride)

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
#import AthenaPoolCnvSvc.ReadAthenaPool
#ServiceMgr.MessageSvc.OutputLevel      = VERBOSE
#IOVDbSvc = Service( "IOVDbSvc" )
#IOVDbSvc.OutputLevel        = 2
#ServiceMgr.EventSelector.RunNumber		= 0
#ServiceMgr.EventSelector.FirstEvent	= 0
