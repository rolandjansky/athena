##############################################################
# Job options read a LArRampComplete object from a POOL file #
# and dump it to an ROOT ntuple                              #
##############################################################

import commands
Ramp2NtupleLog = logging.getLogger( "Ramp2NtupleLog" )

if not 'PoolFileNameList' in dir():
   Ramp2NtupleLog.fatal("Please setup the input POOL file list ('PoolFileNameList')!") 
   theApp.exit(-1) 

if not 'RootFileName' in dir():
   RootFileName = "LArRamp2Ntuple_POOL_root"

if not 'SaveRawRamp' in dir():
   SaveRawRamp = True

if not 'SaveAllSamples' in dir():
   SaveAllSamples = False

if not 'RunNumber' in dir():
   RunNumber=165983

###########################################################################

include("LArCalibProcessing/LArCalib_Flags.py")

include("LArCalibProcessing/LArCalib_MinimalSetup.py")


## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

###########################################################################

include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py")

if not 'DBConnectionCOOL' in dir():
        DBConnectionCOOL = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_LAR;dbname=COMP200;"
        #DBConnectionCOOL = "COOLONL_LAR/COMP200"

## define the DB Global Tag :
svcMgr.IOVDbSvc.GlobalTag   = LArCalib_Flags.globalFlagDB
svcMgr.IOVDbSvc.DBInstance=""
svcMgr.IOVDbSvc.forceRunNumber=RunNumber

from IOVDbSvc.CondDB import conddb
PoolFileList     = []

# Temperature folder
conddb.addFolder("DCS_OFL","/LAR/DCS/FEBTEMP")

if 'InputBadChannelSQLiteFile' in dir():
   from string import *
   InputDBConnectionBadChannel = "sqlite://;schema="+InputBadChannelSQLiteFile+";dbname=COMP200"
else:   
   InputDBConnectionBadChannel = DBConnectionCOOL


if ('InputBadChannelSQLiteFile' in dir()) or ("OFL" in DBConnectionCOOL):
   conddb.addFolder("","/LAR/BadChannelsOfl/BadChannels<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>")
   conddb.addFolder("","/LAR/BadChannelsOfl/MissingFEBs<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>")
else:
   conddb.addFolder("","/LAR/BadChannels/BadChannels<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>")
   conddb.addFolder("","/LAR/BadChannels/MissingFEBs<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>")

include("LArCalibProcessing/LArCalib_BadChanTool.py")

from AthenaCommon.ConfigurableDb import getConfigurable
svcMgr += getConfigurable( "ProxyProviderSvc" )()
svcMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
svcMgr += getConfigurable( "CondProxyProvider" )()
svcMgr.CondProxyProvider.InputCollections += PoolFileNameList


from LArCalibTools.LArCalibToolsConf import LArRamps2Ntuple
LArRamps2Ntuple=LArRamps2Ntuple("LArRamps2Ntuple")
LArRamps2Ntuple.ContainerKey = ["HIGH","MEDIUM","LOW"]
LArRamps2Ntuple.RawRamp=SaveRawRamp
LArRamps2Ntuple.SaveAllSamples=SaveAllSamples

topSequence+= LArRamps2Ntuple


theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+RootFileName+"' OPT='NEW'" ]


AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel=ERROR

theApp.EvtMax=1
svcMgr.MessageSvc.OutputLevel=DEBUG

#StoreGateSvc=Service("StoreGateSvc");
#StoreGateSvc.dump=TRUE

#DetStore=Service("DetectorStore");
#DetStore.dump=TRUE
