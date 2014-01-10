#---------------------------------------------------------------
#
# Load the convertes and then run the ntuple
# creation code over data ESD.  This file is based on
# esdtoaod.py from RecExCommon
#
#---------------------------------------------------------------
# Set this to the Tag used for the data file you are running on (from AMI)
#DetDescrVersion="ATLAS-GEONF-04-00-00"
DetDescrVersion="ATLAS-GEO-02-01-00"
GeoModelSvc = Service("GeoModelSvc")
GeoModelSvc.AtlasVersion = "ATLAS-GEO-02-01-00"

#PoolESDInput = [ "/tmp/nurse/data08_cos.00091060.physics_RPCwBeam.recon.ESD.o4_f70._lb0012._sfo05._0008.1" ]
#PoolESDInput = [ "/tmp/nurse/mc08.105003.pythia_sdiff.recon.ESD.e344_s479_r604_tid032304/ESD.032304._00001.pool.root.1"]
PoolESDInput = [ "/tmp/rkwee/ESD.070454._000001.pool.root.1"]
#PoolESDInput = [ "/atlas/data1/userdata/wbell/reprocessed-esd/87863/physics_BPTX/cosmicsESD_87863_LB0000_0001_sfo1.pool.root" ]

from AthenaCommon.GlobalFlags import globalflags
#globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-ES1C-000-00')
# For some reason this must be set to OFLCOND-CSC-00-00-00, despite the file being made with COMCOND-ES1C-000-00
globalflags.ConditionsTag.set_Value_and_Lock('OFLCOND-CSC-00-00-00')

EvtMax=10

###############################
doCBNT=False

doTrigger=False

readESD=True
doWriteAOD=False
doWriteESD=False
doWriteTAG=False
doAOD=False 
doESD=True
doTAG=False
doTruth=False

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_flags.py")
#-----------------------------------------------------------

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
## Make SpacePoints from Clusters
from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiTrackerSpacePointFinder
InDetSiTrackerSpacePointFinder = InDet__SiTrackerSpacePointFinder()       
topSequence += InDetSiTrackerSpacePointFinder
#-----------------------------------------------------------

from TrigMinBiasNtuple.TrigMinBiasNtupleConf import TrigMinBiasNtupleAlg
TrigMinBiasNtuple = TrigMinBiasNtupleAlg ()


TrigMinBiasNtuple.ntupleFile = "trigmb"

# A string vector enable and configure Ntuple components
#  "NtComponent[,Class:key][,opt:Option]"
# where ',' is used to separate the items and [] are optional.
TrigMinBiasNtuple.ntupleComponents = [#

  "BcmRdo",
  "CtpRdo",
  "CtpDecision",
  "EventInfo,EventInfo:McEventInfo",
  "MbtsContainer",
  "MbtsLvl2",
  "McTruth,McEventCollection:TruthEvent",
  "PixelClus",
  "SctSp",
  "SpLvl2",
  "TrtLvl2",
  "TrkCntsEf",
  "ZdcLvl1",
  "Track,TrackCollection:Tracks"
  ]

## Switch on classes to be dumped,
## by default they are switched off (except doEventInfo)



#TrigMinBiasNtuple.doGenTruth = True


# WHY DOES THIS WORK FOR ESD DATA AND NOT MC?
#TrigMinBiasNtuple.doTrtRaw = False

topSequence += TrigMinBiasNtuple
print          TrigMinBiasNtuple

theApp.HistogramPersistency = "ROOT"

if not hasattr(ServiceMgr, 'THistSvc'):
  from GaudiSvc.GaudiSvcConf import THistSvc
  ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output += [ "trigmb DATAFILE='TrigMinBias.root' OPT='RECREATE'"]

# For debugging
# ServiceMgr.StoreGateSvc.ActivateHistory = True
# ServiceMgr.StoreGateSvc.Dump            = True

