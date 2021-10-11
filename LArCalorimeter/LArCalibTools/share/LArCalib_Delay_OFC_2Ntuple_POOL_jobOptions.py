###########################################################################
#
# LArCalib_Delay_OFC_2Ntuple_POOL_jobOptions.py dumps the conntet of a 
# CaliWave + OFC  POOL files (from standard AP processing)
# to ROOT TTree. 
#
# Author: Pavol Strizenec (pavol @ mail.cern.ch)
#
# Created: Nov. 2010 by merging existing Wave and OFC 2 Ntuple jO 
#
###########################################################################

CWOFC2NtupleLog = logging.getLogger( "CWOFC2NtupleLog" )

if not 'PoolFileName' in dir():
	CWOFC2NtupleLog.fatal("Please setup the input POOL file ('PoolFileName')!")
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
	SaveDerivedInfo = True

if not 'SaveJitter' in dir():
	SaveJitter = True

if not 'CaliWaveRootFileName' in dir():
	CaliWaveRootFileName = "LArWaves2Ntuple_POOL.root"

if not 'OFCRootFileName' in dir():
	OFCRootFileName = "LArWaves2Ntuple_POOL.root"

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
        DBConnectionCOOL = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_LAR;dbname=CONDBR2;"
        #DBConnectionCOOL = "COOLONL_LAR/COMP200"

## define the DB Global Tag :
svcMgr.IOVDbSvc.GlobalTag   = LArCalib_Flags.globalFlagDB
svcMgr.IOVDbSvc.DBInstance=""
svcMgr.IOVDbSvc.forceRunNumber=RunNumber

from IOVDbSvc.CondDB import conddb
PoolFileList     = []

# Temperature folder
#conddb.addFolder("DCS_OFL","/LAR/DCS/FEBTEMP")

if 'InputBadChannelSQLiteFile' in dir():
   from string import *
   InputDBConnectionBadChannel = "sqlite://;schema="+InputBadChannelSQLiteFile+";dbname=CONDBR2"
else:
   InputDBConnectionBadChannel = DBConnectionCOOL

if ( not 'InputBadChannelSQLiteFile' in dir()) and ("ONL" in DBConnectionCOOL):
   BadChannelsFolder="/LAR/BadChannels/BadChannels"
   conddb.addFolder("",BadChannelsFolder+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>",className="CondAttrListCollection")
   MissingFEBsFolder="/LAR/BadChannels/MissingFEBs"
   conddb.addFolder("",MissingFEBsFolder+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>",className='AthenaAttributeList')
else:   
   BadChannelsFolder="/LAR/BadChannelsOfl/BadChannels"
   conddb.addFolder("",BadChannelsFolder+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>",className="CondAttrListCollection")
   MissingFEBsFolder="/LAR/BadChannelsOfl/MissingFEBs"
   conddb.addFolder("",MissingFEBsFolder+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>",className='AthenaAttributeList')

from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelCondAlg, LArBadFebCondAlg
theLArBadChannelCondAlg=LArBadChannelCondAlg(ReadKey=BadChannelsFolder)
condSeq+=theLArBadChannelCondAlg
theLArBadFebCondAlg=LArBadFebCondAlg(ReadKey=MissingFEBsFolder)
condSeq+=theLArBadFebCondAlg

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
	LArCaliWaves2Ntuple.AddFEBTempInfo  = False
	LArCaliWaves2Ntuple.OutputLevel = INFO
	LArCaliWaves2Ntuple.NtupleFile = "FILE1"

	topSequence += LArCaliWaves2Ntuple


if ( WaveType == "Phys" ):
	from LArCalibTools.LArCalibToolsConf import LArPhysWaves2Ntuple
	LArPhysWaves2Ntuple = LArPhysWaves2Ntuple( "LArPhysWaves2Ntuple" )
	LArPhysWaves2Ntuple.NtupleName   = "PHYSWAVE" 
	LArPhysWaves2Ntuple.KeyList      = [ ContainerKey ]
	LArPhysWaves2Ntuple.SaveDerivedInfo = SaveDerivedInfo
	LArPhysWaves2Ntuple.AddFEBTempInfo  = False
	LArPhysWaves2Ntuple.OutputLevel = INFO
	LArPhysWaves2Ntuple.NtupleFile = "FILE1"

	topSequence += LArPhysWaves2Ntuple

from LArCalibTools.LArCalibToolsConf import LArOFC2Ntuple
LArOFC2Ntuple = LArOFC2Ntuple( "LArOFC2Ntuple" )
LArOFC2Ntuple.OutputLevel = INFO
LArOFC2Ntuple.NtupleFile = "FILE2"
topSequence += LArOFC2Ntuple

theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+CaliWaveRootFileName+"' OPT='NEW'" ]
svcMgr.NTupleSvc.Output += [ "FILE2 DATAFILE='"+OFCRootFileName+"' OPT='NEW'" ]
   
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
