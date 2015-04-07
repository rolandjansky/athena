# Andy Haas (ahaas@cern.ch), SLAC, 4/2010
# Use pileUp infrastructure to overlay HITS from one signal stream to HITS from one background stream
# One background event is read for each signal event
# Modified by Piyali (Piyali.Banerjee@cern.ch) to include Metadata containers in the output hits file

from AthenaCommon.Logging import logging
G4HitMergeLog = logging.getLogger('G4HitMerge')
G4HitMergeLog.info( '****************** STARTING G4HitMerge *****************' )

# Job definition parameters:
EvtMax           = runArgs.maxEvents
SkipEvents      = runArgs.skipEvents

G4HitMergeLog.info( '**** Transformation run arguments' )
G4HitMergeLog.info( str(runArgs) )

#------------------------------------------------

# setup DBReplicaSvc to choose closest Oracle replica, configurables style
from AthenaCommon.AppMgr import ServiceMgr
from PoolSvc.PoolSvcConf import PoolSvc
ServiceMgr+=PoolSvc(SortReplicas=True)
from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
ServiceMgr+=DBReplicaSvc(UseCOOLSQLite=False)

# Overall common flags
# DetDescrVersion='ATLAS-GEO-08-00-02'

DetDescrVersion=runArgs.geometryVersion
IOVTag=runArgs.conditionsTag

include( "AthenaCommon/AthenaCommonFlags.py" )

minBiasInputCols = [ runArgs.inputHITS1File ] #["Input1.g4.pool.root"]
PoolHitsInput = [ runArgs.inputHITS2File ] #["Input2.g4.pool.root"]

from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Truth_setOn()
DetFlags.LVL1_setOn()

#include ( "Digitization/Digitization_flags.py" )

# EventInfo Converters
import AthenaPoolCnvSvc.ReadAthenaPool
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

# GeoModel stuff
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion=DetDescrVersion
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

from AthenaCommon.AppMgr import ServiceMgr
from PileUpComps.PileUpCompsConf import PileUpEventLoopMgr

# ad-hoc configuration of pileup event loop manager
theApp.EventLoop = "PileUpEventLoopMgr"
pileUpEventLoopMgr = PileUpEventLoopMgr()
from Digitization.DigitizationFlags import jobproperties
pupStream = "PileUpCollXingStream"
jobproperties.Digitization.rndmSeedList.addSeed( pupStream, 123, 345 )
pileUpEventLoopMgr.OrigSelector="EventSelector"

from PileUpComps.PileUpCompsConf import BkgStreamsCache
from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import EventSelectorAthenaPool
minBiasCache = BkgStreamsCache("MinBiasCache")
minBiasCache.CollPerXing=1
minBiasCache.CollDistribution="Fixed"
minBiasCache.ReadDownscaleFactor=1  # read one new event every event
#minBiasCache.RndmGenSvc=digitizationRndmSvc
minBiasCache.RndmStreamName=pupStream
minBiasEvtSel = EventSelectorAthenaPool("minBiasEventSelector")
minBiasEvtSel.InputCollections = minBiasInputCols
minBiasEvtSel.KeepInputFilesOpen = True
ServiceMgr += minBiasEvtSel
minBiasCache.EventSelector="minBiasEventSelector"
pileUpEventLoopMgr.bkgCaches += [ minBiasCache ]

from AthenaCommon.AppMgr import ToolSvc
from LArRecUtils.LArRecUtilsConf import LArAutoCorrTotalTool
try:
  ToolSvc.LArAutoCorrTotalTool.NMinBias=1.;   # FIXME  <ncol> per crossing
except AttributeError:
  ToolSvc += LArAutoCorrTotalTool(NMinBias=1.)

pileUpEventLoopMgr.OutStreamType = "AthenaOutputStream"
pileUpEventLoopMgr.XingFrequency=25;
pileUpEventLoopMgr.firstXing=0
pileUpEventLoopMgr.lastXing=0
pileUpEventLoopMgr.OutputLevel=VERBOSE
ServiceMgr += pileUpEventLoopMgr

# in any case we need the PileUpMergeSvc for the digitize algos
if not hasattr(ServiceMgr, 'PileUpMergeSvc'):
    from PileUpTools.PileUpToolsConf import PileUpMergeSvc
    ServiceMgr += PileUpMergeSvc()

# Pool input (Change this to use a different file)
if not hasattr(ServiceMgr, 'EventSelector'):
   import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = PoolHitsInput

# Pool Output
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream("StreamHITS", runArgs.outputHITSFile )
#find full list with "checkSG.py <inputfile.pool.root>"
Stream1.ItemList+=["EventInfo#*"];
Stream1.ItemList+=["CaloCalibrationHitContainer#*"];
Stream1.ItemList+=["SiHitCollection#*"];
Stream1.ItemList+=["TrackRecordCollection#*"];
Stream1.ItemList+=["LArHitContainer#*"];
Stream1.ItemList+=["TRTUncompressedHitCollection#*"];
Stream1.ItemList+=["CSCSimHitCollection#*"];
Stream1.ItemList+=["TileHitVector#*"];
#Stream1.ItemList+=["DataHeader#*"];
Stream1.ItemList+=["McEventCollection#*"];
Stream1.ItemList+=["RPCSimHitCollection#*"];
Stream1.ItemList+=["MDTSimHitCollection#*"];
Stream1.ItemList+=["TGCSimHitCollection#*"];
Stream1.ItemList += [ "IOVMetaDataContainer#*" ]
print Stream1

# storegate dump
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.Dump = True  #true will dump data store contents

# the LAr and Calo detector description package
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_joboptions.py" )
include( "LArAthenaPool/LArAthenaPool_joboptions.py" )

from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

from OverlayCommonAlgs.OverlayCommonAlgsConf import G4HitMerger
G4HitMerger = G4HitMerger("G4HitMerger")
topSequence += G4HitMerger

theApp.EvtMax = EvtMax
theApp.SkipEvents = SkipEvents
MessageSvc.OutputLevel = INFO

#---------------------------
# Post-include
if hasattr(runArgs,"postInclude"):
    for fragment in runArgs.postInclude:
        include(fragment)

