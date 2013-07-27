###############################################################
# author Caleb Lampen lampen@physics.arizona.edu 
#
# cscWritePedCool.py
# Standard script to update CSC UPD1 pedestal tags
#==============================================================
#Input calibration text file
if("input" not in dir()):
  input = "/raid02/lampen/datasets/csc/pedRuns/nov/pedestal.cal"
#Output db file name
if("output" not in dir()):
  output = "cscPed.db"
#Should be "COMP200" or "OFLP200"
if("dbname" not in dir()):
  dbname = "COMP200"
#Csc tag that will be used for any merging
if("mergeCscTag" not in dir()):
  mergeCscTag = "Csc-ES1-002-00"

IOVRunStart=0

#use McEventSelector
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
#GlobalFlags.DetGeo.set_atlas()
#GlobalFlags.DataSource.set_geant4()
GlobalFlags.DetGeo.set_commis();
GlobalFlags.DataSource.set_data();
#inc ("RecExCommon/RecExCommon_flags.py")
include ("RecExCond/RecExCommon_flags.py")

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.Muon_setOn()
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOff()
DetFlags.Print()

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
gainFolder = "/CSC/GAIN"
pslopeFolder = "/CSC/PSLOPE"
rslopeFolder = "/CSC/RSLOPE"
pedFolder 	= "/CSC/PED"
noiseFolder	= "/CSC/NOISE"
rmsFolder = "/CSC/RMS"
f001Folder = "/CSC/FTHOLD"
tholdFolder = "/CSC/THOLD"
peaktFolder = "/CSC/PEAKT"
widthFolder = "/CSC/WIDTH"
sat1Folder 	= "/CSC/SAT1"
sat2Folder	= "/CSC/SAT2"
peakcFolder	= "/CSC/PEAKC"
sampleTimeRatioFolder	= "/CSC/SAMPLERATIO"
occupancyFolder	= "/CSC/OCCUPANCY"
statFolder  = "/CSC/STAT"

#suspendCaching()

# data is written to conditions database using OutputConditionsAlg
from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
OutCondAlg = OutputConditionsAlg("OutCondAlg","dummy.root")

# List of things to be written.
# Make sure that only the folders you actually want to write are
# in this list. If something is in this list, and isn't in 
# the calibration file, the entire job will fail.
prefix = "CondAttrListCollection#"
OutCondAlg.ObjectList=[ 
#prefix + gainFolder, 
#prefix + pslopeFolder,
#prefix + rslopeFolder,
prefix + pedFolder,
prefix + noiseFolder, 
prefix + rmsFolder,
prefix +  f001Folder,
#prefix +  f001Folder,
#prefix + tholdFolder,
#prefix + peaktFolder,
#prefix + widthFolder,
#prefix + sat1Folder , 
#prefix +sat2Folder, 
#prefix + peakcFolder,
#prefix + sampleTimeRatioFolder,
#prefix + occupancyFolder,
#prefix + statFolder
]

#Taglist must be in same order as folder list!
OutCondAlg.IOVTagList = [ 
#"CscGain-sim-100-00",
#"CscPslope-sim-000-01",
#"CscRslope-temp-000-00",
"CscPed-ES1-UPD1-002-00",
"CscNoise-ES1-UPD1-002-00",
"CscRms-ES1-UPD1-002-00",
"CscFthold-ES1-UPD1-002-00",
#"CscFthold-comm-ES1-UPD1-002-03",
#"CscThold-test-000-00",
#"CscPeakt-test-000-00",
#"CscWidth-sim-100-00",
#"CscSat1-sim-100-00",
#"CscSat2-sim-100-00",
#"CscPeakc-sim-100-00",
#"CscSampleratio-sim-100-00",
#"CscOccupancy-sim-100-00",
#"CscStat-comm-002-01"
]

OutCondAlg.WriteIOV=True
# set the interval of validity for the file here
# putting nothing for Run2 (uppser limit) falls back on default 
#which is to be valid for all run/event
OutCondAlg.Run1=IOVRunStart
#OutputConditionsAlg.Event1=0
#OutputConditionsAlg.Run2=9999
#OutputConditionsAlg.Event2=9999

##############
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('COMCOND-HLTP-002-00')
#conddb.setGlobalTag("COMCOND-006-00")
include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")
#conddb.setGlobalTag('DEFAULTCOND')
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.IOVRegistrationSvc.OverrideNames+=["Data"]
ServiceMgr.IOVRegistrationSvc.OverrideTypes+=["String64k"]
#WARNING! This will APPEND to the end of the old database file! Not overwrite!
ServiceMgr.IOVDbSvc.dbConnection ="sqlite://;schema=" + output + ";dbname=" + dbname
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
cscCondDB.CscCoolStrSvc.DoCaching = True
cscCondDB.CscCoolStrSvc.DoMerge = True
cscCondDB.addPedFolders()
cscCondDB.addRmsFolder()
cscCondDB.addF001Folder()
conddb.addOverride("/CSC",mergeCscTag)
#cscCondDB.addStatusFolder()
#conddb.addOverride("/CSC/STAT","CscStat-comm-002-00")
#cscCondDB.addStatusFolder()
#ServiceMgr.MessageSvc.OutputLevel = DEBUG 
ServiceMgr.MessageSvc.debugLimit = 0 
#ServiceMgr.CscCoolStrSvc.OutputLevel = VERBOSE
