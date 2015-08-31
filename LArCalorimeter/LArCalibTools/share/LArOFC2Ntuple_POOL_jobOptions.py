###########################################################################
#
# jobOptions to dump LAr OFC POOL file content to ROOT ntuple
#
# rewritten in Nov. 2010, P. Strizenec
###########################################################################

import commands

OFC2NtupleLog = logging.getLogger( "OFC2NtupleLog" )

if not 'PoolFileName' in dir():
   OFC2NtupleLog.fatal("Please setup the input POOL file ('PoolFileName')!")
   theApp.exit(-1)

if not 'RootFileName' in dir():
   RootFileName = "LArOFC2Ntuple_POOL.root"


if not 'DBConnectionCOOL' in dir():
        DBConnectionCOOL = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_LAR;dbname=COMP200;"
        #DBConnectionCOOL = "COOLONL_LAR/COMP200"

if not 'RunNumber' in dir():
        RunNumber = 165983


###########################################################################

include("LArCalibProcessing/LArCalib_Flags.py")

include("LArCalibProcessing/LArCalib_MinimalSetup.py")

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

###########################################################################
#
# POOL
#
include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )
include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py")
## define the DB Global Tag :
svcMgr.IOVDbSvc.GlobalTag   = LArCalib_Flags.globalFlagDB
svcMgr.IOVDbSvc.DBInstance=""
svcMgr.IOVDbSvc.forceRunNumber=RunNumber

from IOVDbSvc.CondDB import conddb
PoolFileList     = []

# Temperature folder
conddb.addFolder("DCS_OFL","/LAR/DCS/FEBTEMP")

if "ONL" in DBConnectionCOOL:
   conddb.addFolder("","/LAR/BadChannels/BadChannels<dbConnection>"+DBConnectionCOOL+"</dbConnection>")
   conddb.addFolder("","/LAR/BadChannels/MissingFEBs<dbConnection>"+DBConnectionCOOL+"</dbConnection>")
else:
   conddb.addFolder("","/LAR/BadChannelsOfl/BadChannels<dbConnection>"+DBConnectionCOOL+"</dbConnection>")
   conddb.addFolder("","/LAR/BadChannelsOfl/MissingFEBs<dbConnection>"+DBConnectionCOOL+"</dbConnection>")
   include("LArCalibProcessing/LArCalib_BadChanTool.py")

from AthenaCommon.ConfigurableDb import getConfigurable
svcMgr += getConfigurable( "ProxyProviderSvc" )()
svcMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
svcMgr += getConfigurable( "CondProxyProvider" )()
svcMgr.CondProxyProvider.InputCollections += [ PoolFileName ]

#
# Dump OFC folder to a ROOT ntuple
#
from LArCalibTools.LArCalibToolsConf import LArOFC2Ntuple
LArOFC2Ntuple = LArOFC2Ntuple( "LArOFC2Ntuple" )
LArOFC2Ntuple.OutputLevel = INFO
topSequence += LArOFC2Ntuple

theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+RootFileName+"' OPT='NEW'" ]
   
##########################################################################

theApp.EvtMax = 1

###########################################################################
#svcMgr.MessageSvc.OutputLevel  = INFO
#svcMgr.MessageSvc.defaultLimit = 10000
#svcMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

#svcMgr += CfgMgr.AthenaEventLoopMgr(OutputLevel = VERBOSE)

###########################################################################
