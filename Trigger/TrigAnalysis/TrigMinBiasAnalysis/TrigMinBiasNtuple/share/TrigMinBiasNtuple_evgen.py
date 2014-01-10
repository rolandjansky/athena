#---------------------------------------------------------------
#
# Run the ntuple creation code over an evgen file to dump the
# generator record and event info to a file.
#
#---------------------------------------------------------------

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

ServiceMgr.MessageSvc.OutputLevel = DEBUG
#ServiceMgr.MessageSvc.OutputLevel = ERROR
theApp.EvtMax = 10

include("AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py")
ServiceMgr.EventSelector.InputCollections = ['rfio:/dpm/unige.ch/home/atlas/atlaslocalgroupdisk/mc09_7TeV/EVNT/e510/mc09_7TeV.105200.T1_McAtNlo_Jimmy.evgen.EVNT.e510_tid105822_00/EVNT.105822._000024.pool.root.1']
#ServiceMgr.EventSelector.InputCollections = ['EVNT.105822._000024.pool.root.1']

from TrigMinBiasNtuple.TrigMinBiasNtupleConf import TrigMinBiasNtupleAlg
TrigMinBiasNtuple = TrigMinBiasNtupleAlg ()


# A string vector enable and configure Ntuple components
#  "NtComponent[,Class:key][,opt:Option]"
# where ',' is used to separate the items and [] are optional.
TrigMinBiasNtuple.ntupleComponents = [#
    "EventInfo,EventInfo:McEventInfo",
    "McTruth,McEventCollection:GEN_EVENT"
]

topSequence += TrigMinBiasNtuple
print          TrigMinBiasNtuple

theApp.HistogramPersistency = "ROOT"

if not hasattr(ServiceMgr, 'THistSvc'):
  from GaudiSvc.GaudiSvcConf import THistSvc
  ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output += [ "trigmb DATAFILE='TrigMinBias.root' OPT='RECREATE'"]

# Force athena to load the STL dictionaries.
import PyCintex
PyCintex.loadDict('STLRflx')
PyCintex.loadDict('STLAddRflx')
PyCintex.loadDict('AtlasSTLAddReflexDict')
