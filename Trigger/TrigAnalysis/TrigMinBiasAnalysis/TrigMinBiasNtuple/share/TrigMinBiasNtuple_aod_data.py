## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

from AthenaCommon.GlobalFlags import jobproperties
#jobproperties.Global.DetDescrVersion = 'ATLAS-CSC-01-00-00'
jobproperties.Global.DetDescrVersion = 'ATLAS-GEONF-04-00-00'
log.info( "DetDescrVersion = %s" % (jobproperties.Global.DetDescrVersion()) )

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
## These need to be commented out for data ??
#include( "TileIdCnv/TileIdCnv_jobOptions.py" )
#include( "TileConditions/TileConditions_jobOptions.py" )  

ServiceMgr.MessageSvc.OutputLevel = INFO
#ServiceMgr.MessageSvc.OutputLevel = ERROR
theApp.EvtMax = -1

include("AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py")

# The AOD input file

ServiceMgr.EventSelector.InputCollections = [ "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/trig-daq/minbias/testdata/data/data08_cos.00091060.physics_RNDM.merge.AOD.o4_r602_p16_tid033147/AOD.033147._00002.pool.root.1",
                                              "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/trig-daq/minbias/testdata/data/data08_cos.00091060.physics_RNDM.merge.AOD.o4_r602_p16_tid033147/AOD.033147._00003.pool.root.1",
                                              "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/trig-daq/minbias/testdata/data/data08_cos.00091060.physics_RNDM.merge.AOD.o4_r602_p16_tid033147/AOD.033147._00005.pool.root.1",
                                              "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/trig-daq/minbias/testdata/data/data08_cos.00091060.physics_RNDM.merge.AOD.o4_r602_p16_tid033147/AOD.033147._00006.pool.root.1",
                                              "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/trig-daq/minbias/testdata/data/data08_cos.00091060.physics_RNDM.merge.AOD.o4_r602_p16_tid033147/AOD.033147._00008.pool.root.1",
                                              "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/trig-daq/minbias/testdata/data/data08_cos.00091060.physics_RNDM.merge.AOD.o4_r602_p16_tid033147/AOD.033147._00010.pool.root.1",
                                              "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/trig-daq/minbias/testdata/data/data08_cos.00091060.physics_RNDM.merge.AOD.o4_r602_p16_tid033147/AOD.033147._00014.pool.root.1",
                                              "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/trig-daq/minbias/testdata/data/data08_cos.00091060.physics_RNDM.merge.AOD.o4_r602_p16_tid033147/AOD.033147._00015.pool.root.1",
                                              "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/trig-daq/minbias/testdata/data/data08_cos.00091060.physics_RNDM.merge.AOD.o4_r602_p16_tid033147/AOD.033147._00020.pool.root.1",
                                              "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/trig-daq/minbias/testdata/data/data08_cos.00091060.physics_RNDM.merge.AOD.o4_r602_p16_tid033147/AOD.033147._00021.pool.root.1" ]



#-----------------------------------------------------------

from TrigMinBiasNtuple.TrigMinBiasNtupleConf import TrigMinBiasNtupleAlg
TrigMinBiasNtuple = TrigMinBiasNtupleAlg ()

TrigMinBiasNtuple.ntupleFile = "trigmb"

# A string vector enable and configure Ntuple components
#  "NtComponent[,Class:key][,opt:Option]"
# where ',' is used to separate the items and [] are optional.
TrigMinBiasNtuple.ntupleComponents = [#

  "CtpDecision",
  "EventInfo,EventInfo:ByteStreamEventInfo",
  "MbtsContainer",
  "MbtsLvl2",
  "SpLvl2",
  "TrtLvl2",
  "TrkCntsEf"

  ]




topSequence += TrigMinBiasNtuple
print          TrigMinBiasNtuple

theApp.HistogramPersistency = "ROOT"

if not hasattr(ServiceMgr, 'THistSvc'):
  from GaudiSvc.GaudiSvcConf import THistSvc
  ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output += [ "trigmb DATAFILE='TrigMinBias.root' OPT='RECREATE'"]
