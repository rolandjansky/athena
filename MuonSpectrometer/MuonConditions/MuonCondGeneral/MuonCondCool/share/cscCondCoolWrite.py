###############################################################
# author Caleb Lampen lampen@physics.arizona.edu 
#
# cscWritePedCool.py
# Standard script to update CSC UPD1 pedestal tags
#==============================================================


##Standard User Configuration################################################

#Note: Merging below refers to taking values from COOL for any channels
#omitted from your text file


#Flags and settings - These can be all defined in an earlier script than this one 
# in the chain
#Turn merging on or off
if("doMerge" not in dir()):
  doMerge = True
#Merge from local sqlite file
if("MergeFromLocalFile" not in dir()):
  MergeFromLocalFile = False
#Input calibration text file
if("input" not in dir()):
  input = "pedestal.cal"
#Output db file name. Note that that repeated writes to this file 
#append, not overwrite!
#Note that for now name must be mycool.db for reading into user jobs!
if("output" not in dir()):
  output = "mycool.db"
#readDb is COOL database schema we're reading from for any merging
if('readDb' not in dir()):
  readDb = "COMP200"
#writeDb is database schema we're writing to
if("writeDb" not in dir()):
  writeDb = "COMP200"
if("CoolGlobalTag" not in dir()):
  CoolGlobalTag ='COMCOND-ES1C-002-00' 
#'/CSC' folder tag can be overwritten here for reading from COOL for merging
if("overrideCscTag" not in dir()):
  overrideCscTag = ""
#Run number to read from COOL when merging
#Set to -1 to not set
if("forceRunNumber" not in dir()):
  forceRunNumber = 2147483647 #Set very high so that is at end of last IOV

#Parameters to write
if ('parsToWrite' not in dir()):
  parsToWrite = ['ped','noise','rms','f001']


#Specify IOV to be written to sqlite file
if("IOVRunStart" not in dir()):
  IOVRunStart = 0
if("IOVRunEnd" not in dir()):
  IOVRunEnd = -1 #-1 will set to infinity

if("writeNewFolder" not in dir()):
  writeNewFolder = False

from MuonCondSvc import CscCoolFolders

#specify which folders to write out.
#needs to match tagsToWrite (below)
#ONLY include those that you actually include in the output pedestal
#file. Otherwise you'll get an error when it tries to write 
#that parameter to SQLite
if('FoldersToWrite' not in dir()):
  FoldersToWrite = CscCoolFolders.folderList(parsToWrite)

#leaf tags to write to.
#Needs to match order in FoldersToWrite
if('tagsToWrite' not in dir()):
  tagsToWrite = [ 
      "CscPed-REF-001-00", 
      "CscNoise-REF-001-00", 
      "CscRms-REF-001-00", 
      "CscFthold-REF-001-00", 
      #"CscStat-REF-001-00"
      ]

if(len(tagsToWrite) != len(FoldersToWrite)):
  print "FATAL - tagsToWrite and FoldersToWrite must have same number of entries!"

##IMPORTANT!!!! - Make sure all folders you need to write OR read from COOL 
# are added at bottom of this file!

####################################################
#End User Configuration - Begin advanced section####
####################################################
#use McEventSelector

#For the most part this doesn't matter so long as uses current id description
DetDescrVersion = "ATLAS-GEONTF-08-00-00"

import AthenaCommon.AtlasUnixStandardJob
from AthenaCommon.AppMgr import theApp
theApp.EvtMax =1 

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#switch Inner Detector and/or Calorimeter if needed
# DetFlags modifications are best set here (uncomment RecExCommon_flags first)
#from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
if(readDb == "OFLP200"):
  GlobalFlags.DataSource.set_geant4()
elif(readDb == "COMP200"): 
  GlobalFlags.DataSource.set_data();
else:
  print "ERROR! readDb = " + readDb + " is invalid! Must be either COMP200 or OFLP200!"

#inc ("RecExCommon/RecExCommon_flags.py")
include ("RecExCond/RecExCommon_flags.py")

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.Muon_setOn()
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOff()

#GlobalFlags.InputFormat.set_bytestream()

include ("RecExCond/AllDet_detDescr.py")

# Detector Initialization
from AtlasGeoModel import SetGeometryVersion
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
from AtlasGeoModel import GeoModelInit

#CscReadWRiteCoolStr is a wrapper for the CscCoolStrSvc. It will have the 
#flat file coppied to the database
from MuonCondCool.MuonCondCoolConf import MuonCalib__CscReadWriteCoolStr
topSequence += MuonCalib__CscReadWriteCoolStr()
CscReadWriteDatabase= MuonCalib__CscReadWriteCoolStr()
CscReadWriteDatabase.Write=True
CscReadWriteDatabase.iFiles=[input]

#Define cool foldernames. These need to be given to both
#CoolStrSvc and IovDbSvc

#suspendCaching()

# data is written to conditions database using OutputConditionsAlg
from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
OutCondAlg = OutputConditionsAlg("OutCondAlg","dummy.root")

# List of things to be written.
# Make sure that only the folders you actually want to write are
# in this list. If something is in this list, and isn't in 
# the calibration file, the entire job will fail.
prefix = "CondAttrListCollection#"
for folder in FoldersToWrite:
  OutCondAlg.ObjectList += [prefix + folder]

#Taglist must be in same order as folder list!
OutCondAlg.IOVTagList = tagsToWrite

#Write out IOV
OutCondAlg.WriteIOV=True
# set the interval of validity for the file here
# putting nothing for Run2 (uppser limit) falls back on default 
#which is to be valid for all run/event
OutCondAlg.Run1=IOVRunStart
#OutCondAlg.Event1=0
if(IOVRunEnd > 0):
  OutCondAlg.Run2=IOVRunEnd #If not specified, goes to Infinitity
#OutCondAlg.Event2=9999

##############
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag(CoolGlobalTag)
if(forceRunNumber > 0):
  Service("IOVDbSvc").forceRunNumber = forceRunNumber;
  Service("IOVDbSvc").forceLumiblockNumber = 1

#conddb.setGlobalTag("COMCOND-006-00")
include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")
#conddb.setGlobalTag('DEFAULTCOND')
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.IOVRegistrationSvc.OverrideNames+=["Data"]
ServiceMgr.IOVRegistrationSvc.OverrideTypes+=["String64k"]
#WARNING! This will APPEND to the end of the old database file! Not overwrite!
ServiceMgr.IOVDbSvc.dbConnection ="sqlite://;schema=" + output + ";dbname=" + writeDb
#Should never be changed to True unless absolutely sure!!!
#ServiceMgr.IOVRegistrationSvc.RecreateFolders = False


###############
#Set detector description
#DetDescrVersion = "ATLAS-Comm-01-00-00"
include ("RecExCond/AllDet_detDescr.py")

# CscCoolStrSvc preps data to be sent to cool database
from MuonCondSvc.CscCondDB import cscCondDB
#Stop caching since we aren't interested in reading it out right now
#cscCondDB.useLocalFile()
#cscCondDB.SetupForNewFolder()
if(doMerge == True):
  cscCondDB.CscCoolStrSvc.DoCaching = True
  cscCondDB.CscCoolStrSvc.DoMerge = True
else:
  print "WARNING no merging/caching"
  cscCondDB.CscCoolStrSvc.DoCaching = False
  cscCondDB.CscCoolStrSvc.DoMerge = False

if(MergeFromLocalFile):
  #Read local file for merging
  cscCondDB.useLocalFile()


if(writeNewFolder):
  print "Setting up for new folder"
  cscCondDB.SetupForNewFolder()


#Adding Folders 
if('ped' in parsToWrite):
  cscCondDB.addPedFolders()
if('rms' in parsToWrite):
  cscCondDB.addRmsFolder()
if('f001' in parsToWrite):
  cscCondDB.addF001Folder()
if('pslope' in parsToWrite):
  cscCondDB.addPSlopeFolder()
if('status' in parsToWrite):
  cscCondDB.addStatusFolder()
if('t0base' in parsToWrite):
  cscCondDB.addT0BaseFolder()
if('t0phase' in parsToWrite):
  cscCondDB.addT0PhaseFolder()

if(overrideCscTag != ""):
  conddb.addOverride("/CSC",overrideCscTag)

#You can override tags for readign like this:
#conddb.addOverride("/CSC/STAT","CscStat-comm-002-00")


#ServiceMgr.MessageSvc.OutputLevel = DEBUG 
ServiceMgr.MessageSvc.debugLimit = 0 
#ServiceMgr.CscCoolStrSvc.OutputLevel = VERBOSE
