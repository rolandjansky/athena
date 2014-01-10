#---------------------------------------------------------------
#
# Run reconstruction, the Minbias trigger slice and then the ntuple
# creation code over RDO.  This file is based on
# testMinBiasSliceAthenaModernRDO.py
#
# Modified April 11 to use AutoConfig
#
#---------------------------------------------------------------

# The LUCID components are commented out because they are not
# present in the example simulation files.


from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.FilesInput = ["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/phys-rig/pileupSamples/valid1.105200.T1_McAtNlo_Jimmy.digit.RDO.e603_s932_d479_tid274637_00/RDO.274637._000001.pool.root.1"]

athenaCommonFlags.EvtMax.set_Value_and_Lock(5)

doCBNT=False

doTrigger=True
TriggerModernConfig=True

doWriteAOD=False
doWriteESD=False
doWriteTAG=False
doAOD=False 
doESD=True
doTAG=False

doTruth=True

doLowPt=True

from AthenaCommon.GlobalFlags  import globalflags
globalflags.InputFormat = 'pool'

from RecExConfig.RecFlags  import rec
rec.AutoConfiguration=['everything']

ServiceMgr.MessageSvc.OutputLevel = 4
ServiceMgr.StatusCodeSvc.OutputLevel = 4
Service("AthenaEventLoopMgr").OutputLevel = 4
Service("PoolDbCacheSvc").OutputLevel = 4

#-----------------------------------------------------------
include("RecExCond/RecExCommon_flags.py")
#-----------------------------------------------------------

TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False

TriggerFlags.enableMonitoring = [ 'Validation', 'Time', 'Log' ]

#TriggerFlags.triggerMenuSetup = 'default'

from RecExConfig.RecConfFlags import recConfFlags
recConfFlags.AllowIgnoreConfigError=False



####  First switch all slices OFF
TriggerFlags.Slices_all_setOff()

###### This is a temporary fix ################
DetFlags.simulateLVL1.Calo_setOn()

# Enable Minimum bias slice
TriggerFlags.MinBiasSlice.setAll()

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

jobproperties.PerfMonFlags.doMonitoring = False
#jobproperties.PerfMonFlags.OutputFile = "ntuple.root"

MessageSvc.debugLimit = 10000000
MessageSvc.Format = "% F%48W%S%7W%R%T %0W%M"

#get rid of messages and increase speed
Service ("StoreGateSvc" ).ActivateHistory=False

#from AthenaCommon.AlgSequence import AlgSequence
print AlgSequence
print ServiceMgr

# disable printing of histograms in finalize
if hasattr(svcMgr,'THistSvc'):
    svcMgr.THistSvc.OutputLevel = WARNING

#-----------------------------------------------------------

from TrigMinBiasNtuple.TrigMinBiasNtupleConf import TrigMinBiasNtupleAlg
TrigMinBiasNtuple = TrigMinBiasNtupleAlg ()

TrigMinBiasNtuple.ntupleFile = "trigmb"

# A string vector enable and configure Ntuple components
#  "NtComponent[,Class:key][,opt:Option]"
# where ',' is used to separate the items and [] are optional.
TrigMinBiasNtuple.ntupleComponents = [#
  "BcmLvl1",
  "BcmRdo",
  "CtpRdo",
  "EventInfo,EventInfo:McEventInfo",
  "LucidLvl1",
  "Lvl1Config,EventInfo:McEventInfo",
  "TriggerDecision",
  "MbtsContainer",
  "MbtsLvl1",
  "MbtsLvl2",
  "McTruth,McEventCollection:TruthEvent",
  "PixelClus,PixelClusterContainer:PixelTrigClusters",
  "SctSp,SpacePointContainer:SCT_TrigSpacePoints",
  "SpLvl2",
  "Track,TrackCollection:Tracks",
  "TrkCntsEf",
  "TrtLvl2",
  "TrtRaw",
  "TruthLucidLvl1",
  "TruthMbtsLvl1",
  "ZdcLvl1" ]

topSequence += TrigMinBiasNtuple
print          TrigMinBiasNtuple

theApp.HistogramPersistency = "ROOT"

if not hasattr(ServiceMgr, 'THistSvc'):
  from GaudiSvc.GaudiSvcConf import THistSvc
  ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output += [ "trigmb DATAFILE='TrigMinBias.root' OPT='RECREATE'"]

# For debugging
#ServiceMgr.StoreGateSvc.ActivateHistory = True
#ServiceMgr.StoreGateSvc.Dump            = True
