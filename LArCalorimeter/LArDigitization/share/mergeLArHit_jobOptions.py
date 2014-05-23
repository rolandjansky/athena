# =========================================================================================================================
#
# Example jobOptions to merge together LArhits from N subevents at the same time to produce a merged event corresponding
#  to one bunch crossing
#
#  Ncoll is average number of collisions in the bunch crossing (Poisson fluctuated)
#
# =============================================================================================================================


# Overall common flags
include( "AthenaCommon/AthenaCommonFlags.py" )

PoolHITsOutput="mergedHits_23.pool.root"

Ncoll=23

from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.EvtMax=100

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-GEO-02-01-00'

#--------------------------------------------------------------------
# Perfmon configuration
#--------------------------------------------------------------------
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True
jobproperties.PerfMonFlags.doPersistencyMonitoring = True


PoolHitsInput = ["rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00090.pool.root",
                 "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00129.pool.root",
                 "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00138.pool.root",
                 "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00140.pool.root"]

minBiasInputCols=["rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00030.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00067.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00069.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00070.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00076.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00090.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00129.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00138.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00140.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00030.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00067.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00069.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00070.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00076.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00090.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00129.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00138.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00140.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00030.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00067.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00069.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00070.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00076.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00090.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00129.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00138.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00140.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00030.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00067.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00069.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00070.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00076.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00090.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00129.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00138.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00140.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00030.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00067.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00069.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00070.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00076.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00090.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00129.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00138.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00140.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00030.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00067.pool.root",
                  "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00069.pool.root"]

from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()

#Pool input
import AthenaPoolCnvSvc.ReadAthenaPool
# EventInfo Converters
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

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
from Digitization import PileUpEventType
from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import EventSelectorAthenaPool
minBiasCache = BkgStreamsCache("MinBiasCache")
minBiasCache.CollPerXing=Ncoll
minBiasCache.CollDistribution="Poisson"
minBiasCache.ReadDownscaleFactor=1  # read one new event every event
minBiasCache.RndmGenSvc=jobproperties.Digitization.rndmSvc()
minBiasCache.RndmStreamName=pupStream
minBiasEvtSel = EventSelectorAthenaPool("minBiasEventSelector")
minBiasEvtSel.InputCollections = minBiasInputCols
minBiasEvtSel.KeepInputFilesOpen = True
ServiceMgr += minBiasEvtSel
minBiasCache.EventSelector="minBiasEventSelector"
pileUpEventLoopMgr.bkgCaches += [ minBiasCache ]

#define output stream
pileUpEventLoopMgr.OutStreamType = "AthenaOutputStream"

#define time range to be studied. t0 at t=0, xing=0
pileUpEventLoopMgr.XingFrequency=25;
pileUpEventLoopMgr.firstXing=0
pileUpEventLoopMgr.lastXing=0

pileUpEventLoopMgr.OutputLevel=VERBOSE

ServiceMgr += pileUpEventLoopMgr

# in any case we need the PileUpMergeSvc for the digitize algos
from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, 'PileUpMergeSvc'):
    from PileUpTools.PileUpToolsConf import PileUpMergeSvc
    ServiceMgr += PileUpMergeSvc()


# Pool input (Change this to use a different file)
from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, 'EventSelector'):
   import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = PoolHitsInput

# Pool Output
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream("Stream1")
Stream1.OutputFile = PoolHITsOutput
Stream1.ItemList+=["EventInfo#*"];
Stream1.ItemList+=["LArHitContainer#*"];


# the LAr and Calo detector description package
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_joboptions.py" )
# Pool Converters
include( "LArAthenaPool/LArAthenaPool_joboptions.py" )

#algorithm
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

from LArDigitization.LArDigitizationConf import LArHitMerger
LArHitMerger = LArHitMerger("LArHitMerger")

topSequence += LArHitMerger

#
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel      = 3
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = jobproperties.AthenaCommonFlags.EvtMax()
