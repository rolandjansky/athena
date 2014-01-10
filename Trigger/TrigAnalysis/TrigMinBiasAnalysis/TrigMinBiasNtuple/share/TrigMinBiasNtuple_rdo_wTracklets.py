#---------------------------------------------------------------
#
# Run reconstruction, the Minbias trigger slice and then the ntuple
# creation code over RDO.  This file is based on
# testMinBiasSliceAthenaModernRDO.py
#
#---------------------------------------------------------------

# The LUCID components are commented out because they are not
# present in the example simulation files.

DetDescrVersion = 'ATLAS-GEO-06-00-00'

PoolRDOInput=[
     "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/trig-daq/validation/test_data/valid1.105003.pythia_sdiff.digit.RDO.e380_s511_d159_tid044456/RDO.044456._00001.pool.root.1",
     "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/trig-daq/validation/test_data/valid1.105003.pythia_sdiff.digit.RDO.e380_s511_d159_tid044456/RDO.044456._00002.pool.root.1",
     "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/trig-daq/validation/test_data/valid1.105003.pythia_sdiff.digit.RDO.e380_s511_d159_tid044456/RDO.044456._00003.pool.root.1",
     "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/trig-daq/validation/test_data/valid1.105003.pythia_sdiff.digit.RDO.e380_s511_d159_tid044456/RDO.044456._00004.pool.root.1",
     "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/trig-daq/validation/test_data/valid1.105003.pythia_sdiff.digit.RDO.e380_s511_d159_tid044456/RDO.044456._00005.pool.root.1",
     "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/trig-daq/validation/test_data/valid1.105003.pythia_sdiff.digit.RDO.e380_s511_d159_tid044456/RDO.044456._00006.pool.root.1",
     "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/trig-daq/validation/test_data/valid1.105003.pythia_sdiff.digit.RDO.e380_s511_d159_tid044456/RDO.044456._00007.pool.root.1",
     "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/trig-daq/validation/test_data/valid1.105003.pythia_sdiff.digit.RDO.e380_s511_d159_tid044456/RDO.044456._00008.pool.root.1",
     "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/trig-daq/validation/test_data/valid1.105003.pythia_sdiff.digit.RDO.e380_s511_d159_tid044456/RDO.044456._00009.pool.root.1",
     "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/trig-daq/validation/test_data/valid1.105003.pythia_sdiff.digit.RDO.e380_s511_d159_tid044456/RDO.044456._00010.pool.root.1"]

EvtMax=-1

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
doHist = True

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_flags.py")
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

from HIGlobal.HIGlobalConf import MakePixelTracklets
MakePixelTracklets = MakePixelTracklets()
MakePixelTracklets.EtaMax = 2
MakePixelTracklets.NtupleLoc = "trigmb"

topSequence += MakePixelTracklets

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
  "Lvl1Config",
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
  "ZdcLvl1",
  "Tracklet"  ]


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
