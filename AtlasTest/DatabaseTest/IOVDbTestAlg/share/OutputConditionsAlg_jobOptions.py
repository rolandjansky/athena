###############################################################
#
# Job options file for IOVDbTestAlg:  it uses codes in IOVDbTestAlg 
# for creates/prints the objects and the codes in OutputConditionsAlg
# for streaming  and registering the objects   
#
#==============================================================

#use McEventSelector
## basic job configuration (for generator)
import AthenaCommon.AtlasUnixGeneratorJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

#--------------------------------------------------------------
# Access to IOVSvc, IOVDbSvc and CondDBMySQLCnvSvc
#--------------------------------------------------------------
import IOVDbSvc.IOVDb

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# Load "user algorithm" top algorithms to be run, and the libraries that house them
from IOVDbTestAlg.IOVDbTestAlgConf import IOVDbTestAlg
topSequence += IOVDbTestAlg( "IOVDbTestAlg" )

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = INFO
svcMgr.MessageSvc.debugLimit       = 100000
svcMgr.MessageSvc.infoLimit        = 100000
svcMgr.MessageSvc.Format           = "% F%30W%S%7W%R%T %0W%M"
IOVDbTestAlg.OutputLevel           = DEBUG
svcMgr.IOVDbSvc.OutputLevel        = INFO

# Set time to register - used for IOVDbTestAmdbCorrection
IOVDbTestAlg.RegTime       = 0
IOVDbTestAlg.CreateExtraChanns=True
IOVDbTestAlg.NameChanns=True

#--------------------------------------------------------------
# Choose the following to write out cond objects . 
IOVDbTestAlg.WriteCondObjs = False
# Choose the following to register cond objects in the IOVDB. 
IOVDbTestAlg.RegisterIOV   = False
# Write out attribute list with cool version
IOVDbTestAlg.ReadWriteCool = False
# Set the tag value
IOVDbTestAlg.TagID   = "COOL-TEST-001"
# Print lumiblock as well
IOVDbTestAlg.PrintLB = False
# True for avoid stream out with IOVDdTestAlgs, but allowing
# stream out with OutputConditionAlgs of the object recorded in DetStore
IOVDbTestAlg.NoStream = True
#--------------------------------------------------------------
# For IOVDb: specify dbConnection with COOL and oracle specified
#
#   Default is to use oracle schema named TESTCOOL, to override this use
#     athena  'TESTCOOL="<your test db name>"' IOVDbTestAlgWriteCool.py
#
# uncomment this to use a local SQLite file instead
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=mytest.db;dbname=TESTCOOL"

#--------------------------------------------------------------
# Options for IOVRegistrationSvc
import RegistrationServices.IOVRegistrationSvc


# Dont set the tag for ALL folders
# in COOL folder tags must be globally unique
regSvc = svcMgr.IOVRegistrationSvc
# regSvc.IOVDbTag   = "DC1"
regSvc.OutputLevel=3

# Select the following to delete and recreate the folders. Default is
# NOT to recreate
regSvc.RecreateFolders = False
# The following set the interval for each of the IOVDbTest folders
# regSvc.BeginRun   = 4
# Max EndRun: 0x7FFFFFFF
# regSvc.EndRun     = 2147483647 
# regSvc.EndRun     = 3 
# regSvc.BeginLB = 0
# Max EndLB: 0xFFFFFFFF
# regSvc.EndLB   = 4294967295
# regSvc.IOVDbTag   = "DC1"

# extra write list to test all AttributeList types
IOVDbTestAlg.FancyList=True  
regSvc.OverrideNames+=['FanSmallString','FanBigString']
regSvc.OverrideTypes+=['String255','String64k']



#--------------------------------------------------------------
# The following turns off the ability to set EventSelector parameters
# below
#ApplicationMgr.EvtSel = "NONE"
# Number of events to be processed (default is 10)
#ApplicationMgr.EvtMax = 1
#--------------------------------------------------------------
# Set the following when reading back to adjust the run/event numbers
#   (don't forget ApplicationMgr.EvtMax)
svcMgr.EventSelector.RunNumber         = 1
svcMgr.EventSelector.EventsPerRun      = 25
svcMgr.EventSelector.FirstEvent        = 1
svcMgr.EventSelector.EventsPerLB       = 1
svcMgr.EventSelector.FirstLB           = 1
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 5
theApp.EvtMax                          = 250

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool

#--------------------------------------------------------------
# AthenaPool details
#--------------------------------------------------------------

#==================== Configure OutputConditionAlgs
# 
from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
myOCA=OutputConditionsAlg("TestOutconditionAlg",
            outputFile="mycondobjects.root",
            ObjectList=[ 
                         # "IOVDbTestMDTEleMap#/IOVDbTestDani/IOVDbTestMDTEleMapDani",
                         "IOVDbTestMDTEleMap#/IOVDbTest/IOVDbTestMDTEleMap",
                         "IOVDbTestAmdbCorrection#/IOVDbTest/IOVDbTestAMDBCorrection",
                         "IOVDbTestMDTEleMapColl#/IOVDbTest/IOVDbTestMDTEleMapColl", 
			             "AthenaAttributeList#/IOVDbTest/IOVDbTestAttrList",
			 		     "AthenaAttributeList#/IOVDbTest/IOVDbTestFancyList"
			             # "AthenaAttributeList#/IOVDbTestDani/IOVDbTestAttrListDani"
                        ],
    	                WriteIOV=True,
                        IOVTagList=["nominal"])




#===================== ADDITIONAL USEFUL INFORMATIONS ============================ 
#The OutputConditionsAlg has the following properties:

#- StreamName (default: "ConditionsAlgStream")
#- ObjectList (default: "")
# - WriteIOV (default: False)
# - Run1 (default IOVTime::MINRUN)
# - LB1 (default: IOVTime::MINEVENT)
# - Run2 (default: IOVTime::MAXRUN)
# - LB2 (default: IOVTime::MAXEVENT)
# - Time1 (default IOVTime::MINTIMESTAMP) (in seconds)
# - Time2 (default IOVTime::MAXEVENT) (in seconds)
# - UseTime (default False - use specifications based on run/event)
# - IOVTagList (default: "")

# If WriteIOV is not set, only the output stream is written, without registration
# in the IOV database.
# The ObjectList is used to identify the conditions data objects to be written
# from the Transient Detector Store (TDS).
# Each entry in the list is either a classname, in which case the default
# instance of the class in Storegate TDS will be written, using the corresponding
# Storegate key as conditions database folder name; or an entry of the
# form 
# 'classname#key' (e.g. AlignableTransform#/Indet/Align/SCT). 
# This latter form is appropriate when several instances of the same object exist, and
# need to be written out to several folders. For example, the following joboption
# tells the tool
# to write out two MyCondObjects with seperate keys /MyCond/Obj1 and
# /MyCond/Obj2 to folders /MyCond/Obj1 and /MyCond/Obj2.
# 
# myOCA.ObjectList=[ "MyCondObject#/MyCond/Obj1", "MyCondObject#/MyCond/Obj2" ]
# 
# a third form 'classname#key#folder' can be used to override the default
# where the conditions DB folder is set to the same name as the Storegate key;
# for example the specification 'MyCondObject#/MyCond/Obj1#/MyCond/AlternateFolder' 
# writes the object with key /MyCond/Obj1 to folder /MyCond/AlternateFolder.
# The AthenaOutputStream-like syntax "MyCondObject#*" to write ALL instances
# of MyCondObject does NOT work with this algorithm.
# 
# The IOVTagList should contain one tag for each entry in the ObjectList. If
# no (or an insufficient number) of tags are specified, the corresponding
# objects are not tagged (i.e. just visible in the HEAD of the conditions
# data.)

