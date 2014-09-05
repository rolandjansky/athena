##############################################################
# TopOptions_WriteBS_LVL1sim.py
#   athena.py -s TopOptions_WriteBS_LVL1sim_Modern.py &>! writeBS_LVL1sim.log  &
##############################################################
from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

from AthenaCommon.Logging import logging
logRecExCommon_flags = logging.getLogger( 'TopOptions_WriteBS_LVL1sim' )

if not ('EvtMax' in dir()):
    acf.EvtMax=-1
if not ('OutputLevel' in dir()):
    rec.OutputLevel=DEBUG
if not ('SkipEvents' in dir()):
    acf.SkipEvents=0
if (not "doFiltering" in dir()):
    doFiltering=False

from AthenaCommon.JobProperties import jobproperties
from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon.DetFlags import DetFlags

GlobalFlags.DetGeo.set_atlas()
GlobalFlags.DataSource.set_geant4()
GlobalFlags.InputFormat.set_pool()
GlobalFlags.Luminosity.set_zero()
GlobalFlags.DetDescrVersion = "ATLAS-CSC-01-02-00"

DetFlags.detdescr.all_setOn()
DetFlags.makeRIO.ID_setOn()

DetFlags.Print()
GlobalFlags.Print()

IOVDbSvc=theApp.service('IOVDbSvc')
IOVDbSvc.GlobalTag="OFLCOND-CSC-00-01-00"

# ----------------------------------------------------------------
# Detector Descriptions
# ----------------------------------------------------------------
DetDescrVersion = "ATLAS-CSC-01-02-00"

#-------------------------------------------------------------
# some people start using globalflags
#-------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion=DetDescrVersion

# ----------------------------------------------------------------
# Pool RDO input
# ----------------------------------------------------------------
#PoolRDOInput=["/afs/cern.ch/atlas/offline/data/testfile/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._00016_extract_10evt.pool.root"]
acf.PoolRDOInput=["LFN:top_CSC-01-02-00_RDO_extract.pool"]
import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.AthenaPoolCnvSvc.PoolAttributes = [ "DEFAULT_BUFFERSIZE = '2048'" ]

svcMgr.EventSelector.InputCollections = PoolRDOInput

# ----------------------------------------------------------------
# Setting detector geometry 
# ----------------------------------------------------------------
include ("RecExCommon/AllDet_detDescr.py")


# -------------------------------------------------------------
# Muon Cablings
# -------------------------------------------------------------
import MDTcabling.MDTcablingConfig
import RPCgeometry.RPCgeometryConfig
import RPCcabling.RPCcablingConfig
import TGCcabling.TGCcablingConfig
import CscClusterization.CscThresholdClusterBuilder

# -------------------------------------------------------------
# Muon RDO to Digit, needed for LVL1 simulation
# -------------------------------------------------------------
include( "MuonByteStreamCnvTest/jobOptions_MuonRDOToDigit.py" )

# -------------------------------------------------------------
# TriggerFlags
# -------------------------------------------------------------
from TriggerJobOpts.TriggerFlags  import TriggerFlags
TriggerFlags.triggerMenuSetup = 'lumi1E31'
TriggerFlags.readLVL1configFromXML=True
TriggerFlags.readHLTconfigFromXML=True  # this we need to make L1 working correctly
TriggerFlags.inputLVL1configFile = "../AthenaP1RDO/outputLVL1config.xml"

# -------------------------------------------------------------
# Menu generation
# -------------------------------------------------------------
# this is to limit messags when running with -s
from AthenaCommon.Include import excludeTracePattern
excludeTracePattern.append("*/TrigConfigSvc/TriggerPythonConfig.py")
excludeTracePattern.append("*/TrigConfigSvc/TrigConfigSvcConfig.py")
from TriggerMenuPython.GenerateMenu import GenerateMenu
g = GenerateMenu()
g.generate()

# -------------------------------------------------------------
# Setup LVL1 simulation
# -------------------------------------------------------------
from TriggerJobOpts.Lvl1TriggerOnlineGetter import Lvl1SimulationGetter
lvl1 = Lvl1SimulationGetter()

# -------------------------------------------------------------
# Add AthenaOutputStream
# -------------------------------------------------------------
include( "ByteStreamCnvSvc/WriteByteStream_EventStorage_jobOptions.py" )
StreamBS = AthenaOutputStream("StreamBS",
                              EvtConversionSvc = "ByteStreamCnvSvc")
StreamBS.ForceRead=True
StreamBS.ItemList  += [ "ROIB::RoIBResult#*" ]
StreamBS.ItemList += [ "TRT_RDO_Container#*" ]
StreamBS.ItemList += [ "SCT_RDO_Container#*" ]
StreamBS.ItemList += [ "PixelRDO_Container#*" ]
StreamBS.ItemList +=["2721#*"]
StreamBS.ItemList +=["2927#*"]
StreamBS.ItemList +=["MdtCsmContainer#*"]
StreamBS.ItemList +=["RpcPadContainer#*"]
StreamBS.ItemList +=["TgcRdoContainer#*"]
StreamBS.ItemList +=["CscRawDataContainer#*"]
StreamBS.ItemList +=["LVL1::TriggerTower#TriggerTowers"] 
if doFiltering:
    StreamBS.AcceptAlgs = ["RoIBuilder"]

from AthenaCommon.AppMgr import ToolSvc

from LArByteStream.LArByteStreamConf import LArRawDataContByteStreamTool
ToolSvc+=LArRawDataContByteStreamTool()               
ToolSvc.LArRawDataContByteStreamTool.InitializeForWriting=True

# -------------------------------------------------------------
# SCT Cablings
# -------------------------------------------------------------
if hasattr(ToolSvc,"SCT_CablingSelector"):
    ToolSvc.SCT_CablingSelector.Filename = "SCT_Jan08Cabling.dat"

theApp.CreateSvc  += ["StoreGateSvc/StoreGateSvc" ]
ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += [ 
	"PixelRDO_Container/PixelRDOs", 
	"SCT_RDO_Container/SCT_RDOs", 
	"TRT_RDO_Container/TRT_RDOs", 
	"LArRawChannelContainer/LArRawChannels", 
	"TileRawChannelContainer/TileRawChannelCnt", 
	"RpcPadContainer/RPCPAD",
	"MdtCsmContainer/MDTCSM",
	"TgcRdoContainer/TGCRDO",
	"CscRawDataContainer/CSCRDO",
	"ROIB::RoIBResult/RoIBResult",
	"MuCTPI_RDO/MUCTPI_RDO",
        "CTP_RDO/CTP_RDO"
	] 
ProxyProviderSvc = Service( "ProxyProviderSvc" )
ProxyProviderSvc.ProviderNames += [ "ByteStreamAddressProviderSvc" ] 

# Set output level threshold
#    (  ALL, VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel               = OutputLevel
MessageSvc.showStats=True
MessageSvc.enableSuppression = True
MessageSvc.debugLimit = 10000000
MessageSvc.Format = "% F%48W%S%7W%R%T %0W%M"

theApp.EvtMax = EvtMax

print "List all ExtSvc"
print theApp.ExtSvc
print "List of all top algorithms"
print theApp.TopAlg

#get rid of messages and increase speed
Service ("StoreGateSvc" ).ActivateHistory=False

print AlgSequence()
print ServiceMgr

