import commands
###########################################################################
#
# <Marco.Delmastro@cern.ch>
#
# Example jobOptions to put params in a poolfile+SQLite file
#
# Last update: 07/03/2008 <Fabien.Tarrade@cern.ch>
#
###########################################################################

###########################################################################
#
#  * Read parameters from ascii file into the appropriate conditions object
#  
#  FILE FORMAT:
#  -----------
#  * Each record in the file refers to a channel, which can be identified using
#  * either offline coordinates or online coordinates.
#
#  *------------------------------------------------------------------------------------------
#  * File record format for OFFLINE coordinates (for readout channels only):
#  * det subdet barrel_ec layer region eta phi   parameters...  (using offline channel id)
#  *  > det       = 4 for LAr Calorimeter
#  *  > subdet    = 1(EMB+EMEC) , 2(HEC) , 3(FCAL)
#  *  > barrel_ec = +/-1(barrel) , +/-2(emec outer wheel) , +/-3(emec inner wheel)
#  *  > region,eta,phi: see explanation in CaloIdentifier/LArEM_ID LArHEC_ID and LArFCAL_ID.
#  *  > parameters: values to be set for each channel
#  *------------------------------------------------------------------------------------------
#  * File record format for ONLINE coordinates (for readout and calibration channels):
#  * barrel_ec posneg feedthrough slot channel      parameters...  (using online channel id)
#  *  > barrel_ec   = 0(barrel) , 1(em+had+fcal endcap)
#  *  > posneg      = 0(neg.eta,C) , 1(pos.eta,A)
#  *  > feedthrough: see explanation in LArIdentifier/LArOnlineID.h
#  *  > parameters: values to be set for each channel
#  *------------------------------------------------------------------------------------------
#
###########################################################################
#
# define here the name of data structure to be read and saved to POOL.
# you need to set DataStructure (the name of the transient data class to fill)
# and DetStoreKey (the key to record/retrieve to/from detector store)
#
# Possible data structures are:
#
# - LArCaliPulseParamsComplete
# - LArDetCellParamsComplete
# - LArPhysCaliTdiffComplete
# - LArTdriftComplete
# - LArMphysOverMcalComplete
# - LArRinjComplete
# - LArTshaperComplete
# - LArEMEC_CphiComplete
# - LArEMEC_HValphaComplete
# - LArEMEC_HVbetaComplete
# - LArCableLengthComplete
# - LArCableAttenuationComplete
# - LArCaliPulseParamsVsCalib
#
# DetStoreKey must the same as DataStructure, without 'Complete' or 'VsCalib'
#
###########################################################################



#DataStructure = "MyData"        # used by LArReadParamsFromFile and LArParams2Ntuple
#DetStoreKey   = "Key"           # used to extract collections from det.store and save to POOL
#AsciiFileName = "MyAsciiFile"   # input ascii file
#ChannelIdType = "OfflineID"     # file uses 'offline coordinates' (readout only)
#ChannelIdType = "OnlineID"      # file uses 'online coordinates' (readout channels)
#ChannelIdType = "OnlineCalibID" # file uses 'online coordinates' (calibration lines)

# An example for 'Complete' conditions object:
#--------------------------------------------

if not 'DataStructure' in dir():
    DataStructure = "LArDetCellParamsComplete"

if not 'DetStoreKey' in dir():
    DetStoreKey   = "LArDetCellParams"

if not 'AsciiFileName' in dir():
    AsciiFileName = "/users/carolina/rootFiles/pascal/lcmeas/new_detinner_Ccold.dat"

if not 'ChannelIdType' in dir():
    ChannelIdType = "OfflineID"
   
from string import *
def DBConnectionFile(sqlitefile):
   return "sqlite://;schema="+sqlitefile+";dbname=COMP200"    

# output POOL file:
if not 'WritePoolFile' in dir():
    WritePoolFile = True
    
if not 'PoolFileName' in dir():
    PoolFileName = "LArParams.pool.root"
    
if not 'GroupingType' in dir():
    GroupingType = "FeedThroughGrouping"

if not 'WriteIOV' in dir():
    WriteIOV = True

if not 'IOVBegin' in dir():
    IOVBegin = 0
   
if not 'IOVEnd' in dir():
    IOVEnd = 0
      
if not 'LArCalibFolderOutputTag' in dir():
    LArCalibFolderOutputTag = "-comm-08"

if not 'OutputDB' in dir():
    OutputDB = "sqlite://;schema=myDB200.db;dbname=COMP200"

if 'OutputSQLiteFile' in dir():
    print ('OutputSQLiteFile',OutputSQLiteFile )
    OutputDB = DBConnectionFile(OutputSQLiteFile)
    print ('OutputDB',OutputDB )

if not 'ParamsFolderName' in dir():
    ParamsFolderName = "/LAR/ElecCalib/DetCellParams"

if not 'WriteNT' in dir():
    WriteNT  = True

if not 'RootFileName' in dir():
    RootFileName = "LArParams.root"
    
###########################################################################
#
# Global settings
#
###########################################################################

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

#
# Provides ByteStreamInputSvc name of the data file to process in the offline context
#

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

include("LArCalibProcessing/LArCalib_MinimalSetup.py")

include("LArCalibProcessing/LArCalib_Flags.py")
# define the DB Gobal Tag :
svcMgr.IOVDbSvc.GlobalTag = LArCalib_Flags.globalFlagDB

#########################################################################################################
#
# Reading ASCII file...

#########################################################################################################

# Example import statement for templated algorithms
#from LArCalibTools.LArCalibToolsConf import LArReadParamsFromFile_LArTdriftComplete_ as LArReadFile

ImportStatement = "from LArCalibTools.LArCalibToolsConf import LArReadParamsFromFile_"+DataStructure+"_ as LArReadFile"
exec(ImportStatement)
    
LArReadFile = LArReadFile("LArReadFile")
LArReadFile.File = AsciiFileName
LArReadFile.ChannelIdType = ChannelIdType
LArReadFile.GroupingType  = GroupingType
if 'CustomKey' in dir():
    LArReadFile.CustomKey = CustomKey

topSequence += LArReadFile

###########################################################################
#
# Writing to Ntuple...
#
###########################################################################

if ( WriteNT ):

    from LArCalibTools.LArCalibToolsConf import LArParams2Ntuple
    LArParams2Ntuple = LArParams2Ntuple("LArParams2Ntuple")
    LArParams2Ntuple.KeyList += [ DataStructure ]
    # the following jOs act on ALL LAr readout channels! (~190000, also empty ones) and should be used for test only!
    #LArParams2Ntuple.AllChannels2Ntuple = True                  # all channels to ntuple (also those with no params set)
    #LArParams2Ntuple.DumpAllOnlineChannels = "dumpChannels.txt" # all channels identifiers to ascii file (for mapping tests)
    if 'DetStoreSuffix' in dir():
         LArParams2Ntuple.DetStoreSuffix = DetStoreSuffix

    topSequence += LArParams2Ntuple

    theApp.HistogramPersistency = "ROOT"
    from GaudiSvc.GaudiSvcConf import NTupleSvc
    svcMgr += NTupleSvc()
    svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+RootFileName+"' OPT='NEW'" ]

###########################################################################
#
# writing to POOL file...
#
###########################################################################

from string import *
def LArCalibFolderTag(folder,tag=LArCalibFolderOutputTag):
    return join(split(folder, '/'),'') + tag

if ( WritePoolFile ):
    
    OutputObjectSpec    = DataStructure + "#" + DetStoreKey + "#" + ParamsFolderName
    OutputObjectSpecTag = LArCalibFolderTag(ParamsFolderName, LArCalibFolderOutputTag)
    
    from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
    theOutputConditionsAlg = OutputConditionsAlg("OutputConditionsAlg",PoolFileName,
                                                 [OutputObjectSpec],[OutputObjectSpecTag],WriteIOV)
    theOutputConditionsAlg.Run1 = IOVBegin
    if IOVEnd>0:
      theOutputConditionsAlg.Run2 = IOVEnd

    svcMgr.IOVDbSvc.dbConnection  = OutputDB

    from AthenaCommon.AppMgr import ServiceMgr
    from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
    svcMgr += IOVRegistrationSvc()
    svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
    svcMgr.IOVRegistrationSvc.RecreateFolders = False

##########################################################################

theApp.EvtMax = 1

###########################################################################

svcMgr.MessageSvc.OutputLevel  = INFO
svcMgr.MessageSvc.defaultLimit = 10000
svcMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = INFO)

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = DEBUG)
theAuditorSvc += getConfigurable("ChronoAuditor")()
theAuditorSvc += getConfigurable("NameAuditor")()

###########################################################################

