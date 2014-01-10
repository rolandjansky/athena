## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

from AthenaCommon.GlobalFlags import jobproperties
#jobproperties.Global.DetDescrVersion = 'ATLAS-CSC-01-00-00'
jobproperties.Global.DetDescrVersion = 'ATLAS-GEO-06-00-00'
log.info( "DetDescrVersion = %s" % (jobproperties.Global.DetDescrVersion()) )

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
#include( "TileIdCnv/TileIdCnv_jobOptions.py" )
#include( "TileConditions/TileConditions_jobOptions.py" )  

ServiceMgr.MessageSvc.OutputLevel = INFO
#ServiceMgr.MessageSvc.OutputLevel = ERROR
theApp.EvtMax = -1

include("AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py")

# The AOD input file
ServiceMgr.EventSelector.InputCollections = [ "../run/AOD.pool.root" ]

#-----------------------------------------------------------

from TrigMinBiasNtuple.TrigMinBiasNtupleConf import TrigMinBiasNtupleAlg
TrigMinBiasNtuple = TrigMinBiasNtupleAlg ()

TrigMinBiasNtuple.ntupleFile = "trigmb"

# A string vector enable and configure Ntuple components
#  "NtComponent[,Class:key][,opt:Option]"
# where ',' is used to separate the items and [] are optional.
TrigMinBiasNtuple.ntupleComponents = [#
  "EventInfo,EventInfo:McEventInfo",
#  "McTruth,McEventCollection:GEN_AOD",
  "CtpDecision",
  "MbtsLvl2",
  "SpLvl2",
  "TrtLvl2",
  "TrkCntsEf",
  "MbtsContainer"]

topSequence += TrigMinBiasNtuple
print          TrigMinBiasNtuple

theApp.HistogramPersistency = "ROOT"

if not hasattr(ServiceMgr, 'THistSvc'):
  from GaudiSvc.GaudiSvcConf import THistSvc
  ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output += [ "trigmb DATAFILE='TrigMinBiasAOD.root' OPT='RECREATE'"]

## For debugging
# ServiceMgr.StoreGateSvc.Dump            = True
