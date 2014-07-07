##########################################################
# Job options to dump the LAr FEB temperatures to ntuple.#
##########################################################

RunNumber = 162903

COOL_folder = "/LAR/DCS/FEBTEMP"
    
DBConnectionCOOL_OFL = "COOLOFL_DCS/COMP200"
DBConnectionCOOL_ONL = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_LAR;dbname=COMP200;user=ATLAS_COOL_READER"

include("LArCalibProcessing/LArCalib_Flags.py")
include("LArCalibProcessing/LArCalib_MinimalSetup.py")
include("RecExCommission/GetInputFiles.py")
include("AthenaCommon/Atlas_Gen.UnixStandardJob.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

import IOVDbSvc.IOVDb

svcMgr.IOVDbSvc.GlobalTag   = LArCalib_Flags.globalFlagDB

from IOVDbSvc.CondDB import conddb
PoolFileList     = []

conddb.addFolder("","/LAR/BadChannels/BadChannels<dbConnection>"+DBConnectionCOOL_ONL+"</dbConnection>")
conddb.addFolder("","/LAR/BadChannels/MissingFEBs<dbConnection>"+DBConnectionCOOL_ONL+"</dbConnection>")

svcMgr.PoolSvc.ReadCatalog += ["prfile:poolcond/PoolCat_oflcond.xml",
"xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_comcond.000005.lar_conditions.recon.pool.v0000_castor.xml",
"xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_comcond.000006.lar_conditions.recon.pool.v0000_castor.xml",
"xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_diskbuffer_afs.xml",
"xmlcatalog_file:/afs/cern.ch/user/l/larcalib/w0/stableConds/PoolCat_stable.xml",
"xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_cond08_data.000001.lar.COND_castor.xml"]

if ( len(PoolFileList)>0 ):   
    from AthenaCommon.ConfigurableDb import getConfigurable
    svcMgr += getConfigurable( "ProxyProviderSvc" )()
    svcMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]    
    svcMgr += getConfigurable( "CondProxyProvider" )()
    svcMgr.CondProxyProvider.InputCollections += PoolFileList

svcMgr.IOVDbSvc.OutputLevel        = INFO
svcMgr.IOVDbSvc.dbConnection  = DBConnectionCOOL_OFL

svcMgr.ProxyProviderSvc.OutputLevel = INFO
svcMgr.AthenaPoolCnvSvc.OutputLevel = INFO

svcMgr.IOVSvc.partialPreLoadData     = False
#svcMgr.IOVDbSvc.OnlineMode   = True

from McEventSelector.McEventSelectorConf import McEventSelector
svcMgr += McEventSelector("EventSelector")
svcMgr.EventSelector.RunNumber = RunNumber
#svcMgr.EventSelector.EventsPerRun      = 100
#svcMgr.EventSelector.FirstEvent = 2
#svcMgr.EventSelector.InitialTimeStamp = 1259163985
svcMgr.EventSelector.InitialTimeStamp = 1283145454
#svcMgr.EventSelector.TimeStampInterval = 1

theApp.Dlls += ["LArCalibTools"]
theApp.Dlls += [ "LArFEBTempTool" ]

from LArCondUtils.LArCondUtilsConf import LArFEBTempTool
ToolSvc += LArFEBTempTool()
ToolSvc.LArFEBTempTool.FolderName   = COOL_folder

from LArCalibTools.LArCalibToolsConf import LArFEBTemp2Ntuple
topSequence += LArFEBTemp2Ntuple()
LArFEBTemp2Ntuple = LArFEBTemp2Ntuple()

svcMgr.IOVDbSvc.Folders += [ COOL_folder ]

theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='FEBTemp.root' OPT='NEW'" ]

AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel=INFO

LArFEBTemp2Ntuple.OutputLevel=INFO
LArFEBTempTool.OutputLevel=INFO

theApp.EvtMax=1
MessageSvc.OutputLevel=INFO
svcMgr.IOVDbSvc.OutputLevel = DEBUG
