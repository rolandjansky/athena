#example of personal topOptions
#
# to use it  
# athena >! athena.log
#  ( myTopOptions.py is defaulted through jobOptions.py soft link)
# 
# see RecExCommon/share/RecExCommon_flags.py for more available flags
# and https://twiki.cern.ch/twiki/bin/view/Atlas/RecExCommonFlags
# for more complete documentation.
#
# doESD, DetDescrVersion and other flags 
# needs be set before the include, since several
# secondary flags are configured according to that one
#

# Number of events to be processed (default is 10)
if "EventMax" in dir():
  EvtMax = EventMax  
else:
  EvtMax = 25

PoolRDOInput  = ["root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/det-muon/valid/mc11_Zmumu/RDO.533482._000001.pool.root.1" ]
#PoolRDOInput  = [ "rfio:/castor/cern.ch/user/g/golling/Align_Zmumu_RDO_2/misal1_mc12.005151.McAtNloZmumu.digit.RDO.v12003108_tid004784._00025.pool.root.1"]
#PoolRDOInput = [ "rfio:/castor/cern.ch/grid/atlas/dq2/misal1_valid1/RDO/misal1_valid1.005144.PythiaZee.digit.RDO.v12000605_tid009162/misal1_valid1.005144.PythiaZee.digit.RDO.v12000605_tid009162._00101.pool.root.3 " ]

# By default write ESD, AOD and TAG simultenaously, but note that in production
#      these should be done in separate steps (results might not be bit-by-bit identical).
# doESD=False # uncomment if do not run ESD making algorithms
doWriteESD=False # uncomment if do not write ESD
# doAOD=False # uncomment if do not run AOD making algorithms
doWriteAOD=False # uncomment if do not write AOD
doWriteTAG=False # uncomment if do not write TAG
#doTriggerConfigOnly=True
doCBNT = False
doHIST = False

#--------------------------------------------------------------
# detector description version
#--------------------------------------------------------------
DetDescrVersion = "ATLAS-GEO-18-01-01"
#--------------------------------------------------------------

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

jobproperties.PerfMonFlags.doMonitoring = False

MessageSvc.debugLimit = 10000000
MessageSvc.Format = "% F%48W%S%7W%R%T %0W%M"

#get rid of messages and increase speed
Service ("StoreGateSvc" ).ActivateHistory=False


include("InDetPerformanceMonitoring/IDPerfMon_jobOptions.py")
THistSvc = Service( "THistSvc" )
histOutput = "IDPerfMon DATAFILE='./IDPerfMon.root' OPT='RECREATE'"
THistSvc.Output += [histOutput]
IDPerfMonManager.FileKey = "IDPerfMon"

#--------------------------------------------------------------

# include this if you want to use the geometry from the database
#this is used to explicitely specify what geometry database to use for reconstruction
# Don NOT include these 2, don't know why...
#GeoModelSvc=Service("GeoModelSvc")
#GeoModelSvc.IgnoreTagDifference = True

#
IOVDbSvc = Service("IOVDbSvc")
#so for actually trying to recover the CSC misalignments,
#I DON'T want to use the truth info, so I go with 00
#on the other hand if I want to attempt to recover a handmade
#misalignment, then I don't want to be screwed by the CSC
#misalignment, so I DO want to use the truth database, i.e 01
#
#IOVDbSvc.GlobalTag = "OFLCOND-CSC-00-00-00"
#IOVDbSvc.GlobalTag = "OFLCOND-CSC-00-01-00"
#IOVDbSvc.GlobalTag = "OFLCOND-CSC-00-01-04" # DetectorPaper (Pawel)
if "AlignmentGlobalTag" in dir():
  IOVDbSvc.GlobalTag = AlignmentGlobalTag
else:  
  IOVDbSvc.GlobalTag = "OFLCOND-SDR-BS7T-05-14"

#--------------------------------------------------------------
