# example jobOption to use pileUp infrastructure to overlay HITS from one signal stream
#  to RDO from one background stream
#  One background event is read for each signal event
#
# to run this job :  athena.py HitRdoOverlayExample_topOptions.py
# (note that since 13.X.0, beginning 2008, the -p pileUpBootStrap.py is not needed anymore)

# Overall common flags
DetDescrVersion='ATLAS-CSC-01-02-00'
include( "AthenaCommon/AthenaCommonFlags.py" )

EvtMax = 20
PoolHitsInput = ["rfio:/castor/cern.ch/user/g/gunal/HITS.014280._81495.pool.root.1"]
minBiasInputCols = ["rfio:/castor/cern.ch/user/g/gunal/misal1_mc12_V1.005001.pythia.DIGITS.pool.root"]

from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()

include ( "Digitization/Digitization_flags.py" )

# EventInfo Converters
import AthenaPoolCnvSvc.ReadAthenaPool
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

# GeoModel stuff: 
#--------------------------------------------------------------
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
#define inputs
pileUpEventLoopMgr.OrigSelector="EventSelector"

from PileUpComps.PileUpCompsConf import BkgStreamsCache
from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import EventSelectorAthenaPool
minBiasCache = BkgStreamsCache("MinBiasCache")
minBiasCache.CollPerXing=1
minBiasCache.CollDistribution="Fixed"
minBiasCache.ReadDownscaleFactor=1  # read one new event every event
minBiasCache.RndmGenSvc=digitizationRndmSvc
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


#define output stream
pileUpEventLoopMgr.OutStreamType = "AthenaOutputStream"

#define time range to be studied. t0 at t=0, xing=0
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

# the LAr and Calo detector description package
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_joboptions.py" )
# Pool Converters
include( "LArAthenaPool/LArAthenaPool_joboptions.py" )

#algorithm
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

from OverlayExamples.OverlayExamplesConf import HitRdoOverlayExample
HitRdoOverlayExample = HitRdoOverlayExample("HIT_RDO_OVERLAY")
topSequence += HitRdoOverlayExample

#
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel      = 3
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = EvtMax
# Number of input events to be skipped
ServiceMgr.EventSelector.SkipEvents = SkipEvents
