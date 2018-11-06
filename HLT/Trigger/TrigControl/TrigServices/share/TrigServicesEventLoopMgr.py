#**************************************************************
#
# TrigServicesEventLoopMgr configuration file
#
#==============================================================
from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import ToolSvc

# from AthenaCommon.Constants import *

# ==============================================================================
# Set up from StoreGateHiveExample.py and AthExHiveOpts.py and AtlasThreadedJob.py
# ==============================================================================
#nThreads = 1
#numStores = 1
#numAlgsInFlight = nThreads

#from AthenaCommon.ConcurrencyFlags import jobproperties as jps
#jps.ConcurrencyFlags.NumThreads = nThreads
#jps.ConcurrencyFlags.NumConcurrentEvents = numStores

# from GaudiCommonSvc.GaudiCommonSvcConf import AlgContextSvc
# svcMgr += AlgContextSvc("AlgContextSvc")
# svcMgr.AlgContextSvc.Check = True

from StoreGate.StoreGateConf import SG__HiveMgrSvc
svcMgr += SG__HiveMgrSvc("EventDataSvc")
#svcMgr.EventDataSvc.NSlots = numStores
#svcMgr.EventDataSvc.OutputLevel = VERBOSE

from GaudiHive.GaudiHiveConf import AlgResourcePool
arp=AlgResourcePool( OutputLevel = INFO )
arp.TopAlg=["AthMasterSeq"] #this should enable control flow
svcMgr += arp

from AthenaCommon.AlgScheduler import AlgScheduler
#AlgScheduler.setThreadPoolSize(nThreads)
AlgScheduler.ShowDataDependencies(True)
AlgScheduler.ShowControlFlow(True)
AlgScheduler.OutputLevel=VERBOSE

from StoreGate.StoreGateConf import StoreGateSvc
svcMgr += StoreGateSvc()
svcMgr.StoreGateSvc.OutputLevel = VERBOSE
svcMgr.StoreGateSvc.Dump = True

from StoreGate.StoreGateConf import SGImplSvc
svcMgr += SGImplSvc("SGImplSvc")
svcMgr.SGImplSvc.OutputLevel = VERBOSE

# ThreadPoolService thread local initialization
from GaudiHive.GaudiHiveConf import ThreadPoolSvc
svcMgr += ThreadPoolSvc("ThreadPoolSvc")
svcMgr.ThreadPoolSvc.ThreadInitTools = ["ThreadInitTool"]

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
#algCardinality = nThreads

# if (algCardinality != 1):
#     for alg in topSequence:
#        name = alg.name()
#        alg.Cardinality = algCardinality

from SGComps.SGCompsConf import SGInputLoader
topSequence += SGInputLoader( FailIfNoProxy=False )

theAuditorSvc = svcMgr.AuditorSvc
theApp.AuditAlgorithms=True
from SGComps.SGCompsConf import SGCommitAuditor
theAuditorSvc += SGCommitAuditor()

# for easier browsing of verbose logs
ClassIDSvc = Service("ClassIDSvc")
ClassIDSvc.OutputLevel = DEBUG

# ==============================================================================
# Event selector and input service
# ==============================================================================
from TrigByteStreamCnvSvc.TrigByteStreamCnvSvcConf import TrigByteStreamInputSvc
inputSvc = TrigByteStreamInputSvc("ByteStreamInputSvc")
svcMgr += inputSvc

from TrigByteStreamCnvSvc.TrigByteStreamCnvSvcConf import TrigEventSelectorByteStream
evtSel = TrigEventSelectorByteStream("EventSelector")
evtSel.ByteStreamInputSvc = inputSvc
svcMgr += evtSel
theApp.EvtSel = "EventSelector"

# ==============================================================================
# Output conversion service
# ==============================================================================
from TrigByteStreamCnvSvc.TrigByteStreamCnvSvcConf import TrigByteStreamCnvSvc
outputCnvSvc = TrigByteStreamCnvSvc("TrigByteStreamCnvSvc")
svcMgr += outputCnvSvc

# ==============================================================================
# Some extra services
# ==============================================================================
## basic Gaudi services from AtlasUnixStandardJob.py
import GaudiSvc.GaudiSvcConf as GaudiSvcConf
svcMgr += GaudiSvcConf.IncidentSvc()
svcMgr += GaudiSvcConf.EvtPersistencySvc( "EventPersistencySvc" )
svcMgr += GaudiSvcConf.HistogramSvc( "HistogramDataSvc" )
svcMgr += GaudiSvcConf.NTupleSvc()
svcMgr += GaudiSvcConf.ToolSvc()
svcMgr += GaudiSvcConf.RndmGenSvc()
svcMgr += GaudiSvcConf.ChronoStatSvc()

#####

from StoreGate.StoreGateConf import StoreGateSvc
svcMgr += StoreGateSvc("DetectorStore")

# ProxyProviderSvc services configuration
svcMgr += CfgMgr.ProxyProviderSvc()

# --- ByteStreamAddressProviderSvc configuration
svcMgr += CfgMgr.ByteStreamAddressProviderSvc()
svcMgr.ProxyProviderSvc.ProviderNames += [ "ByteStreamAddressProviderSvc" ]
#theApp.CreateSvc += [ svcMgr.ByteStreamAddressProviderSvc.getFullName() ]

# Initialization of DetDescrCnvSvc
svcMgr += CfgMgr.DetDescrCnvSvc(
    # specify primary Identifier dictionary to be used
    IdDictName = "IdDictParser/ATLAS_IDS.xml"
    )
theApp.CreateSvc += [ svcMgr.DetDescrCnvSvc.getFullName() ]
svcMgr.EventPersistencySvc.CnvServices += [ "DetDescrCnvSvc" ]

# --- ByteStreamCnvSvc configuration
svcMgr += CfgMgr.ByteStreamCnvSvc("ByteStreamCnvSvc")
svcMgr.EventPersistencySvc.CnvServices += [ "ByteStreamCnvSvc" ]

# dictionary services from AtlasUnixStandardJob.py
# the dict loader
import AthenaServices.AthenaServicesConf as AthenaServicesConf
if not hasattr(svcMgr, 'AthDictLoaderSvc'):
    svcMgr += AthenaServicesConf.AthDictLoaderSvc()
theApp.CreateSvc += [svcMgr.AthDictLoaderSvc.getFullJobOptName()]

# the dict checker
if not hasattr(svcMgr, 'AthenaSealSvc'):
    svcMgr += AthenaServicesConf.AthenaSealSvc()
theApp.CreateSvc += [svcMgr.AthenaSealSvc.getFullJobOptName()]

# ==============================================================================
#  Message format
# ==============================================================================
msgFmt = "% F%40W%S %4W%e%s %7W%R %T %0W%M"
svcMgr.MessageSvc.Format = msgFmt

# ==============================================================================
# General setup
# ==============================================================================

# make the HltEventLoopMgr service available
svcMgr.HltEventLoopMgr = theApp.service( "HltEventLoopMgr" )     # already instantiated
HltEventLoopMgr = svcMgr.HltEventLoopMgr
HltEventLoopMgr.WhiteboardSvc = "EventDataSvc"
HltEventLoopMgr.SchedulerSvc = AlgScheduler.getScheduler().getName()
HltEventLoopMgr.EvtSel = evtSel
HltEventLoopMgr.OutputCnvSvc = outputCnvSvc


# configure here Level-1 CTP ROB identifier which is used in HLT
# HltEventLoopMgr.Lvl1CTPROBid = 0x770001

# request that events with invalid or missing CTP ROBs are skipped by the HltEventLoopMgr
# (default: don't skip these events)
# HltEventLoopMgr.Lvl1CTPROBcheck = TRUE

#
# name of the HLT Result object in StoreGate
#
# HltEventLoopMgr.HltResultName = "HLTResult_HLT"

#
# handling of truncated HLT Results
#
# switch on saving of events with truncated HLT results to DEBUG stream (default FALSE)
#HltEventLoopMgr.WriteTruncatedHLTtoDebug = FALSE 
# name of DEBUG Stream (default "TruncatedHLTResult")
#HltEventLoopMgr.HltTruncationDebugStreamName = "TruncatedHLTResult"

# list of stream names which should not be send to the truncation DEBUG stream (default ["CostMonitoring"]) 
# HltEventLoopMgr.ExcludeFromHltTruncationDebugStream = ["CostMonitoring"]

#
# properties for the HLT result size histograms
# --> set upper edge of histogram to maximum allowed number of words in HLT
#
# HltEventLoopMgr.histHltResultSize=("HltResultSize",0,125000,100)

#
# Configuration of EDM size monitoring plots
#
# try:
    # from TrigEDMConfig.TriggerEDM import EDMDetails,getTypeAndKey,keyToLabel
    # from TrigEDMConfig.TriggerEDM import TriggerHLTList
    # 
    # # TODO update to use import TriggerHLTList directly
    # #from TrigEDMConfig.TriggerEDM import TriggerL2List,TriggerEFList
    # #TriggerHLTList = list(set(TriggerL2List).union(TriggerEFList))
    # 
    # l = []
    # for item in TriggerHLTList:
        # if ('BS' in item[1].split()) or ('DS' in item[1].split()):
            # t,k = getTypeAndKey(item[0])
            # ctype = t
            # if EDMDetails[t].has_key('collection'):
                # ctype = EDMDetails[t]['collection']
            # l += [ctype+'__'+keyToLabel(k)]
    # # add a bin for not accounted collections
    # l += ['not__configured']
# 
    # HltEventLoopMgr.HltEDMCollectionNames = l
    # number_of_collections=len( HltEventLoopMgr.HltEDMCollectionNames )
    # HltEventLoopMgr.histHltEdmSizes=("HltEDMSizes",0., 15000., number_of_collections)
# except ImportError,e:
    # print " +----------------------------------------------------------------------+ "
    # print " | No initial configuration for EDM monitoring plots will be available! | "
    # print " +----------------------------------------------------------------------+ "
    # print e
#==============================================================
#
# End of TrigServicesEventLoopMgr configuration file
#
#**************************************************************
