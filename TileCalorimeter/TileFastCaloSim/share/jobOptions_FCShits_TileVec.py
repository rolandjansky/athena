#==============================================================
#
# Job options file to create ntuples from HITS POOL file
#
#==============================================================

from AthenaCommon.AppMgr import theApp
svcMgr = theApp.serviceMgr()

from AthenaCommon.Logging import logging
logDigitization_flags = logging.getLogger( 'Tile_Digitization' )

#---  Output printout level -----------------------------------
#output threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
if not 'OutputLevel' in dir():
    OutputLevel = 3
svcMgr.MessageSvc.OutputLevel = OutputLevel
svcMgr.MessageSvc.defaultLimit = 1000000
svcMgr.MessageSvc.Format = "% F%60W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.useColors = False

# get a handle on topalg
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# configuration flags
#--------------------------------------------------------------

if not 'EvtMax' in dir():
    EvtMax = -1

if not 'InputFile' in dir():
    InputFile = 'HITS.pool.root'

if not 'ConddbTag' in dir():
    ConddbTag = 'OFLCOND-MC12-SDR-27'

if not 'DetDescrVersion' in dir():
    DetDescrVersion = 'ATLAS-GEO-21-00-03'

# Ushape=-1 - only copy hits; =0 - linear shape (0.7-0.3); >=1  - Tile_1D_profile shape;
if not 'Ushape' in dir():
    Ushape = 0

if not 'doD3PD' in dir():
    doD3PD = False

if not 'doHitNtuple' in dir():
    doHitNtuple = True

if not 'doCreatePool' in dir():
    doCreatePool = False


#--------------------------------------------------------------------
# DetFlags. Use to turn on/off individual subdetector or LVL1 trigger
#--------------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags

DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.LAr_setOff()
DetFlags.Tile_setOn()
DetFlags.Muon_setOff()
DetFlags.LVL1_setOff()
DetFlags.Truth_setOn()

# - switch off tasks
DetFlags.pileup.all_setOff()
DetFlags.simulate.all_setOff()
DetFlags.makeRIO.all_setOff()
DetFlags.writeBS.all_setOff()
DetFlags.readRDOBS.all_setOff()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRDOPool.all_setOff()
DetFlags.writeRDOPool.all_setOff()
DetFlags.readRIOPool.all_setOff()
DetFlags.writeRIOPool.all_setOff()
DetFlags.simulateLVL1.all_setOff()


#--------------------------------------------------------------
# Global flags. Like eg the DD version:
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DataSource.set_Value_and_Lock('geant4')
globalflags.InputFormat.set_Value_and_Lock('pool')

from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.beamType.set_Value_and_Lock('collisions')

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag(ConddbTag);
log.info( "ConddbTag = %s" % (ConddbTag) )

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion = DetDescrVersion
log.info( "DetDescrVersion = %s" % (jobproperties.Global.DetDescrVersion()) )

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.IgnoreTagDifference = True
log.info( "GeoModelSvc.AtlasVersion = %s" % (GeoModelSvc.AtlasVersion) )

from AtlasGeoModel import TileGM
GeoModelSvc.DetectorTools[ "TileDetectorTool" ].Ushape = Ushape

include( "TileConditions/TileConditions_jobOptions.py" )
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

theApp.EvtMax=EvtMax
svcMgr.EventSelector.InputCollections = [ InputFile ]
svcMgr.StatusCodeSvc.AbortOnError = False
svcMgr.StatusCodeSvc.SuppressCheck = True
svcMgr.MessageSvc.OutputLevel = OutputLevel


## Creating TileHitVector from FCS_StepInfoCollection
from TileFastCaloSim.TileFastCaloSimConf import TileFCSmStepToTileHitVec
theTileFCSmStepToTileHitVec = TileFCSmStepToTileHitVec()
topSequence += theTileFCSmStepToTileHitVec
theTileFCSmStepToTileHitVec.Ushape = Ushape


#--------------------------------------------------------------
# Creating ntuples
#--------------------------------------------------------------

theApp.HistogramPersistency = "ROOT"

if doD3PD:

    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    alg = MSMgr.NewRootStream( "AANT", "tile_d3pd.root", "truth" )

    from TruthD3PDMaker.TruthD3PDMakerKeys import TruthD3PDKeys
    from TruthD3PDMaker.TruthD3PDMakerFlags import TruthD3PDFlags

    from TruthD3PDAnalysis.GenObjectsFilterTool import *

    from TruthD3PDMaker.GenVertexD3PDObject import GenVertexD3PDObject
    alg += GenVertexD3PDObject( 10, filter = AllTrackFilterTool() )

    from TruthD3PDMaker.GenParticleD3PDObject import GenParticleD3PDObject
    alg += GenParticleD3PDObject( 10, filter = AllTrackFilterTool() )

    from CaloD3PDMaker.TileHitD3PDObject import TileHitD3PDObject
    alg += TileHitD3PDObject( 0 )

if doHitNtuple:

    if not hasattr(svcMgr, 'NtupleSvc'):
        from GaudiSvc.GaudiSvcConf import NTupleSvc
        svcMgr += NTupleSvc()
    NTupleSvc = svcMgr.NTupleSvc
    NTupleSvc.Output = [ "AANT DATAFILE='tile_ntup.root' OPT='NEW'" ]

    # #--- Create TileCal h32 ntuple with all G4 hits ---------
    from TileRec.TileRecConf import TileHitVecToNtuple
    theTileHitToNtuple = TileHitVecToNtuple()
    topSequence += theTileHitToNtuple
    theTileHitToNtuple.MaxLength=99999
    theTileHitToNtuple.TileHitVector = "TileHitVec"
    theTileHitToNtuple.NTupleID = "h32"
    theTileHitToNtuple.NTupleLoc = "/AANT"

    # #--- Create TileCal h33 ntuple with all merged G4 hits ---------
    theTileHitToNtuple2 = TileHitVecToNtuple("TileHitVecToNtuple2")
    topSequence += theTileHitToNtuple2
    theTileHitToNtuple2.MaxLength=99999
    theTileHitToNtuple2.TileHitVector = "TileHitVec_FCS"
    theTileHitToNtuple2.NTupleID = "h33"
    theTileHitToNtuple2.NTupleLoc = "/AANT"


#--------------------------------------------------------------
# Creating POOL file
#--------------------------------------------------------------

if doCreatePool:
    # Pool Output
    include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )
    from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
    FileName = "HITS.merge.pool.root"
    Stream1 = AthenaPoolOutputStream( "Stream1", FileName )

    # list of output objects
    Stream1.ItemList += [ "TileHitVector#*" ]
    Stream1.ItemList += [ "TileDigitsContainer#*" ]
    Stream1.ItemList += [ "TileBeamElemContainer#*" ]
    Stream1.ItemList += [ "TileRawChannelContainer#*" ]
    Stream1.ItemList += [ "TileMuContainer#*" ]
    Stream1.ItemList += [ "TileL2Container#*" ]
    Stream1.ItemList += [ "TileCosmicMuonContainer#*" ]
    Stream1.ItemList += [ "TileCellContainer#*" ]
    Stream1.ItemList += [ "CaloCellContainer#*" ]

