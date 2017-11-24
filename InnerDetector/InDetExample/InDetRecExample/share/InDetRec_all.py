
#--------------------------------------------------------------
#
#
# InDetExample driving joboptions
#
#
#--------------------------------------------------------------

#--------------------------------------------------------------
# load ServiceMgr, ToolSvc, topSequence
#--------------------------------------------------------------

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
#
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
topSequence = AlgSequence()
#
from AthenaCommon.AppMgr import ToolSvc

import MagFieldServices.SetupField
#--------------------------------------------------------------
# Abort on ERROR if set by RecFlags
#--------------------------------------------------------------

from RecExConfig.RecFlags import rec
if rec.abortOnErrorMessage(): CfgMgr.AlgErrorAuditor(Throw=True)

#--------------------------------------------------------------
# activate (memory/cpu) performance monitoring 
#--------------------------------------------------------------

if InDetFlags.doPerfMon():
  from PerfMonComps.PerfMonFlags import jobproperties
  jobproperties.PerfMonFlags.doMonitoring = True
  jobproperties.PerfMonFlags.doSemiDetailedMonitoring = True

#--------------------------------------------------------------
# change MessageSvc 
#--------------------------------------------------------------

# output level
ServiceMgr.MessageSvc.OutputLevel  = OutputLevel
# increase the number of letter reserved to the alg/tool name from 18 to 30
ServiceMgr.MessageSvc.Format       = "% F%40W%S%7W%R%T %0W%M"
# to change the default limit on number of message
ServiceMgr.MessageSvc.defaultLimit = 9999999  # all messages

#--------------------------------------------------------------
# Load POOL support, setup for reconstruction
#--------------------------------------------------------------

# --- GeoModel
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
# --- setup POOL access in ATHENA
from AthenaCommon.GlobalFlags import globalflags
if globalflags.InputFormat() == 'bytestream':
  # set up service
  include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
  # configure converters, including cabling
  include("InDetRecExample/InDetReadBS_jobOptions.py")
  if 'athenaCommonFlags' in dir() and athenaCommonFlags.FilesInput():
    ServiceMgr.ByteStreamInputSvc.FullFileName  = athenaCommonFlags.FilesInput()
else:
  # set up pool reading
  import AthenaPoolCnvSvc.ReadAthenaPool
  ServiceMgr.PoolSvc.AttemptCatalogPatch=True
  ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DEFAULT_BUFFERSIZE = '2048'" ]
  ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "TREE_BRANCH_OFFSETTAB_LEN ='100'" ]
  if 'athenaCommonFlags' in dir() and athenaCommonFlags.FilesInput():
    ServiceMgr.EventSelector.InputCollections   = athenaCommonFlags.FilesInput()
    ServiceMgr.EventSelector.SkipEvents         = athenaCommonFlags.SkipEvents()


# ---- Beam Spot service
include("InDetBeamSpotService/BeamCondSvc.py")
# --- particle property service
include("PartPropSvc/PartPropSvc.py")
# --- if truth, get corresponding POOL converters
if InDetFlags.doTruth():
  include("GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py")

from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
alg = xAODMaker__EventInfoCnvAlg()
print alg
topSequence += alg

#------------------------------------------------------------
# Event Data Model Monitor
#------------------------------------------------------------

if doEdmMonitor:
  from TrkValAlgs.TrkValAlgsConf import Trk__EventDataModelMonitor
  InDetEdmMonitor = Trk__EventDataModelMonitor (name = 'InDetEdmMonitor')
  topSequence += InDetEdmMonitor
  if (InDetFlags.doPrintConfigurables()):
    print          InDetEdmMonitor
## DBM TruthLinks
if (InDetFlags.doDBMstandalone() or InDetFlags.doDBM()) and InDetFlags.doTruth():
  from AthenaCommon.Resilience import protectedInclude
  protectedInclude( "McParticleAlgs/TruthParticleBuilder_jobOptions_DBM.py" )

#--------------------------------------------------------------
# Load Inner Detector reconstruction
#--------------------------------------------------------------

if not (('doWriteBS' in dir() and doWriteBS)):
  include( "InDetRecExample/InDetRec_jobOptions.py" )

#--------------------------------------------------------------
# Load Inner Detector Monitoring, use the data quality flags for steering
#--------------------------------------------------------------
if InDetFlags.doMonitoring():
  if not hasattr(ServiceMgr, 'THistSvc'):
      from GaudiSvc.GaudiSvcConf import THistSvc
      ServiceMgr += THistSvc()
  from AthenaMonitoring.DQMonFlags import DQMonFlags
  DQMonFlags.monManFileKey             = "GLOBAL"
  DQMonFlags.monManManualDataTypeSetup = True
  DQMonFlags.monManManualRunLBSetup    = True
  DQMonFlags.monManDataType            = "monteCarlo"
  DQMonFlags.monManEnvironment         = "user"
  DQMonFlags.monManRun                 = 1
  DQMonFlags.monManLumiBlock           = 1
  ServiceMgr.THistSvc.Output += [ DQMonFlags.monManFileKey()+" DATAFILE='monitoring.root' OPT='RECREATE'" ]
  include( "InDetRecExample/InDetMonitoring.py" )

# ------------------------------------------------------------
# switch off history service and no dump
# ------------------------------------------------------------

ServiceMgr.StoreGateSvc.ActivateHistory = False
ServiceMgr.StoreGateSvc.Dump            = False

# ------------------------------------------------------------
# get stack dump
# ------------------------------------------------------------

gbl.AthenaServices.SetFatalHandler(438)

#--------------------------------------------------------------
# run JiveXML for Atlantis event display
#--------------------------------------------------------------

if doJiveXML:
  include ("JiveXML/JiveXML_jobOptionBase.py")
  include ("JiveXML/DataTypes_InDet.py")
  if InDetFlags.doTruth():
    readG3 = False
    include ("JiveXML/DataTypes_Truth.py")
  
#--------------------------------------------------------------
# run VP1 (virtual point 1) event display
#--------------------------------------------------------------

if doVP1:
  from VP1Algs.VP1AlgsConf import VP1Alg
  topSequence += VP1Alg()

# ------------------------------------------------------------
# Use auditors
# ------------------------------------------------------------

if doAuditors:
  # --- load AuditorSvc
  from AthenaCommon.ConfigurableDb import getConfigurable
  # --- write out summary of the memory usage
  #   | number of events to be skip to detect memory leak
  #   | 20 is default. May need to be made larger for complete jobs.
  ServiceMgr.AuditorSvc += getConfigurable("ChronoAuditor")()
  # --- write out a short message upon entering or leaving each algorithm
  if doNameAuditor:
    ServiceMgr.AuditorSvc += getConfigurable("NameAuditor")()
  #
  theApp.AuditAlgorithms = True
  theApp.AuditServices   = True
  #  
  # --- Display detailed size and timing statistics for writing and reading
  if doWriteESD or doWriteAOD or ('doCopyRDO' in dir() and doCopyRDO):
    ServiceMgr.AthenaPoolCnvSvc.UseDetailChronoStat = True

# ------------------------------------------------------------
# Create additional collections
# ------------------------------------------------------------
if InDetFlags.doSplitReco() :
  from McParticleAlgs.JobOptCfg import createTruthParticlesBuilder,PileUpClassification
  builder =  createTruthParticlesBuilder(name = "McAodBuilderPU",
                                         inMcEvtCollection   = "GEN_EVENT_PU",
                                         outTruthParticles   = "SpclMCPU"
  )
  builder.DoFiltering = False
  topSequence += builder
  print builder

# ------------------------------------------------------------
# write BS 
# ------------------------------------------------------------

if 'doWriteBS' in dir() and doWriteBS:
  # --- load writing BS file
  include ("InDetRecExample/InDetWriteBS_jobOptions.py")
  
# ------------------------------------------------------------
# persistency
# ------------------------------------------------------------

if doWriteESD or doWriteAOD or ('doCopyRDO' in dir() and doCopyRDO):
  # --- load setup
  from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
  # --- check dictionary
  ServiceMgr.AthenaSealSvc.CheckDictionary = True
  # --- commit interval (test)
  ServiceMgr.AthenaPoolCnvSvc.CommitInterval = 10

# MC truth information
  if doWriteESD or doWriteAOD:
    if InDetFlags.doTruth():
      truthList = [ 'xAOD::TruthEventContainer#TruthEvents',
                    'xAOD::TruthEventAuxContainer#TruthEventsAux.',
                    'xAOD::TruthEventContainer#TruthEvents_PU',
                    'xAOD::TruthEventAuxContainer#TruthEvents_PUAux.',
                    'xAOD::TruthEventContainer#SpclMCPU',
                    'xAOD::TruthEventAuxContainer#SpclMCPUAux.',
                    'xAOD::TruthPileupEventContainer#TruthPileupEvents',
                    'xAOD::TruthPileupEventAuxContainer#TruthPileupEventsAux.',
                    'xAOD::TruthParticleContainer#TruthParticles',
                    'xAOD::TruthParticleAuxContainer#TruthParticlesAux.',
                    'xAOD::TruthVertexContainer#TruthVertices',
                    'xAOD::TruthVertexAuxContainer#TruthVerticesAux.',
                    'PileUpEventInfo#OverlayEvent' ]

  if doWriteESD:
    # --- create stream
    StreamESD            = AthenaPoolOutputStream ( "StreamESD", InDetKeys.OutputESDFileName(),asAlg=True) 
    # --- save MC collections if truth turned on
    if InDetFlags.doTruth():
      StreamESD.ItemList += truthList
    # ---- load list of objects
    include ( "InDetRecExample/WriteInDetESD.py" )
    StreamESD.ItemList  += InDetESDList
    StreamESD.ItemList  += [ 'xAOD::EventInfo#EventInfo' , 'xAOD::EventAuxInfo#EventInfoAux.' ]
    # --- add trigger to IDRE standalone ESD
    StreamESD.ItemList += [ "TrigDec::TrigDecision#TrigDecision" ]
    StreamESD.ItemList += [ "HLT::HLTResult#HLTResult_L2" ]
    StreamESD.ItemList += [ "HLT::HLTResult#HLTResult_EF" ]
    StreamESD.ForceRead = True # otherwise unread stuff is not copied
    if InDetFlags.doDBMstandalone(): 
      topSequence.StreamESD.ItemList+=["TrackCollection#SiSPSeededTracks"] 

  if doWriteAOD:
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    # --- create *augmented* stream; this makes ROOT happy
    StreamAOD_Augmented   = MSMgr.NewPoolRootStream ( "StreamAOD", InDetKeys.OutputAODFileName() )
    # --- here is the old, backward-compatible event stream
    StreamAOD = StreamAOD_Augmented.GetEventStream()
    # --- save MC collections if truth turned on
    if InDetFlags.doTruth():
      StreamAOD.ItemList += truthList

    # --- load list of objects
    include ( "InDetRecExample/WriteInDetAOD.py" )
    StreamAOD.ItemList  += [ 'xAOD::EventInfo#EventInfo' , 'xAOD::EventAuxInfo#EventInfoAux.' ]

    # --- add trigger to IDRE standalone AOD
    StreamAOD.ItemList += [ "TrigDec::TrigDecision#TrigDecision" ]
    StreamAOD.ItemList += [ "HLT::HLTResult#HLTResult_L2" ]
    StreamAOD.ItemList += [ "HLT::HLTResult#HLTResult_EF" ]
    if InDetFlags.doDBMstandalone():
      topSequence.StreamESD.ItemList+=["TrackCollection#SiSPSeededTracks"]
    StreamAOD.ForceRead = True # otherwise unread stuff is not copied
  
  if 'doCopyRDO' in dir() and doCopyRDO:
    # --- create stream
    StreamRDO            = AthenaPoolOutputStream ( "StreamRDO", InDetKeys.OutputRDOFileName())
    # --- add output to list
    StreamRDO.ItemList   +=  ['TRT_RDO_Container#*','SCT_RDO_Container#*','PixelRDO_Container#*']
    StreamRDO.ItemList   +=  ['InDetSimDataCollection#*','McEventCollection#*']
    # --- Force read
    StreamRDO.ForceRead  = TRUE;  #force read of output data objs
