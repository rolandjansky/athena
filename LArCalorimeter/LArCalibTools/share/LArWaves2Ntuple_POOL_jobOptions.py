###########################################################################
#
# LArWave2Ntuple_POOL_jobOptions.py dumps the contet of a Wave POOL files
# to ROOT TTree. Works with LArCaliWaveContainer and LAPhysWaveContainer.
#
# Author: Marco Delmastro <Marco.Delmastro@cern.ch>
#
# Last update: 2008-04-19
#
###########################################################################

import commands

Waves2NtupleLog = logging.getLogger( "Waves2NtupleLog" )

if not 'PoolFileName' in dir():
	Waves2NtupleLog.fatal("Please setup the input POOL file ('PoolFileName')!")
	theApp.exit(-1)

if not 'WaveType' in dir():
	WaveType = "Cali" # (Cali, Phys)

if not 'ContainerKey' in dir():
	if (WaveType == "Cali"):
		ContainerKey = "LArCaliWave"
	if (WaveType == "Phys"):
		ContainerKey = "LArPhysWave"

if not 'DACSaturSkip' in dir():
	DACSaturSkip = False

if not 'SaveDerivedInfo' in dir():
	SaveDerivedInfo = False

if not 'SaveJitter' in dir():
	SaveJitter = False

if not 'RootFileName' in dir():
	RootFileName = "LArWaves2Ntuple_POOL.root"

if not 'RunNumber' in dir():
        RunNumber = 165983

###########################################################################

#include("LArConditionsCommon/LArMinimalSetup.py")

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

if ( WaveType == "Cali" ):
	
	from LArCalibTools.LArCalibToolsConf import LArCaliWaves2Ntuple
	LArCaliWaves2Ntuple = LArCaliWaves2Ntuple( "LArCaliWaves2Ntuple" )
	LArCaliWaves2Ntuple.NtupleName   = "CALIWAVE"
	LArCaliWaves2Ntuple.KeyList      = [ ContainerKey ]
	LArCaliWaves2Ntuple.DACSaturSkip = DACSaturSkip
	LArCaliWaves2Ntuple.SaveDerivedInfo = SaveDerivedInfo
	LArCaliWaves2Ntuple.SaveJitter      = SaveJitter
	LArCaliWaves2Ntuple.OutputLevel = INFO

	topSequence += LArCaliWaves2Ntuple


if ( WaveType == "Phys" ):
	from LArCalibTools.LArCalibToolsConf import LArPhysWaves2Ntuple
	LArPhysWaves2Ntuple = LArPhysWaves2Ntuple( "LArPhysWaves2Ntuple" )
	LArPhysWaves2Ntuple.NtupleName   = "PHYSWAVE" 
	LArPhysWaves2Ntuple.KeyList      = [ ContainerKey ]
	LArPhysWaves2Ntuple.SaveDerivedInfo = SaveDerivedInfo
	LArPhysWaves2Ntuple.OutputLevel = INFO

	topSequence += LArPhysWaves2Ntuple

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

#from AthenaCommon.AppMgr import theAuditorSvc
#from AthenaCommon.ConfigurableDb import getConfigurable
#theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = DEBUG)
#theAuditorSvc += getConfigurable("ChronoAuditor")()
#theAuditorSvc += getConfigurable("NameAuditor")()

###########################################################################
