#**************************************************************
#
# jopOptions file for TileCal testbeam analysis of simulated data
#
#==============================================================

from AthenaCommon.AppMgr import theApp
svcMgr = theApp.serviceMgr()

#---  Output printout level ----------------------------------- 
#output threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
if not 'OutputLevel' in dir():
    OutputLevel = 3
svcMgr.MessageSvc.OutputLevel = OutputLevel
svcMgr.MessageSvc.defaultLimit = 1000000
svcMgr.MessageSvc.Format = "% F%60W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.useColors = False

#--- Number of events to be processed (default is 10) ---------
if not 'EvtMax' in dir():
    EvtMax = 10000
theApp.EvtMax = EvtMax

# get a handle on topalg
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags    import DetFlags

# - Select detectors
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.em_setOff()
DetFlags.FCal_setOff()
DetFlags.HEC_setOff()
DetFlags.Tile_setOn()
DetFlags.Muon_setOff()
DetFlags.Truth_setOn()


#==============================================================
# Job configuration
#==============================================================

# - switch off tasks
DetFlags.pileup.all_setOff()
DetFlags.simulate.all_setOff()
DetFlags.makeRIO.all_setOff()
DetFlags.writeBS.all_setOff()
DetFlags.readRDOBS.all_setOff()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRIOPool.all_setOff()
DetFlags.writeRIOPool.all_setOff()
DetFlags.simulateLVL1.all_setOff()
DetFlags.readRDOPool.Tile_setOn()
DetFlags.writeRDOPool.Tile_setOff()
# - print flags 
DetFlags.Print()

# flags for TileCal digitization
doSim=True
Tile2004=False
Tile2003Noise=True; # 1.2 ADC counts HG noise istead of 1.6 ADC counts
# change TileBarrelOnly to True for 3 barrels setup
TileBarrelOnly=False

#---  Load PartProp into the Detector store ---------------
if not hasattr(svcMgr, 'PartPropSvc'):
    from PartPropSvc.PartPropSvcConf import PartPropSvc
    svcMgr += PartPropSvc()
#--- Pool specific --------------------------------------------
# - General Pool converters
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
# - Pool input (Change this to use a different file)
svcMgr.EventSelector.InputCollections = [ "Tile_MyOutputFile-Dig.root" ]

#--- GeoModel stuff -------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value_and_Lock('geant4')
globalflags.DetGeo.set_Value_and_Lock('ctbh8')

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
if TileBarrelOnly:
    GeoModelSvc.TileVersionOverride = "TileTB-3B-00"
else:
    GeoModelSvc.TileVersionOverride = "TileTB-2B2EB-00"

#--- Tile Conditions ------------------------------------------
include( "TileConditions/TileTBConditions_jobOptions.py" )

# Create various ntuples with hits or rawChannels amplitudes
# inside CBNTAA or in standalone ntuples
# CBNTAA is switched off by default
# Hit energy is the energy which comes from Geant4 without sampling correction 
# and RawChannel amplitude is normally in pCb
# check carefully which pCb/GeV factor is applied in digitization (1.05 or 1.0)

if not 'doCBNT' in dir():
    doCBNT = False

if doCBNT:
    #Store everything in CBNTAA
    CBNTAthenaAware = True

    if not hasattr(svcMgr,"THistSvc"):
        from GaudiSvc.GaudiSvcConf import THistSvc
        svcMgr+=THistSvc()
    svcMgr.THistSvc.Output += ["AANT DATAFILE='tiletb_aa.root' OPT='RECREATE'" ]

    from AnalysisTools.AnalysisToolsConf import AANTupleStream
    AANTupleStream = AANTupleStream()
    topSequence += AANTupleStream
    AANTupleStream.ExtraRefNames = [ "" ]
    AANTupleStream.OutputName = "tiletb_aa.root"
    AANTupleStream.ExistDataHeader = False

    include( "CBNT_Athena/CBNT_AthenaAware_jobOptions.py" )
    include( "CBNT_Athena/CBNT_EventInfo_jobOptions.py" )

    # truth information
    from CBNT_Truth.CBNT_TruthConf import CBNTAA_Truth
    CBNT_Truth = CBNTAA_Truth()
    CBNT_AthenaAware += CBNT_Truth
    CBNT_Truth.Members += [ "CBNT_TruthSelector/All" ]

else:
    CBNTAthenaAware = False


if not 'doCalibNtuple' in dir():
    doCalibNtuple = False

if doCalibNtuple and doCBNT:
    include( "CaloCalibHitRec/CalibHit_CBNTAA_jobOptions.py" )


if not 'doHitNtuple' in dir():
    doHitNtuple = False

if doHitNtuple:
    if CBNTAthenaAware:
        from TileRec.TileRecConf import CBNTAA_TileHitVec
        CBNTAA_TileHitVec = CBNTAA_TileHitVec()
        CBNT_AthenaAware += CBNTAA_TileHitVec
    else:
        #--- Create TileCal h32 ntuple with all G4 hits ---------
        from TileRec.TileRecConf import TileHitVecToNtuple
        theTileHitToNtuple = TileHitVecToNtuple()
        topSequence += theTileHitToNtuple
        theTileHitToNtuple.MaxLength=99999
        theTileHitToNtuple.TileHitVector = "TileHitVec"


if not 'doTileNtuple' in dir():
    doTileNtuple = True

if doTileNtuple:
    #--- Create TileCal h1000 ntuple with RawChannels ---------
    include( "TileTBRec/TileTBDefaults_jobOptions.py" )
    include( "TileTBRec/TileTBNtuple_jobOptions.py" )


theApp.HistogramPersistency = "ROOT"

if not doCBNT and (doHitNtuple or doTileNtuple):
    if not hasattr(svcMgr, 'NtupleSvc'):
        from GaudiSvc.GaudiSvcConf import NTupleSvc
        svcMgr += NTupleSvc()
    NTupleSvc = svcMgr.NTupleSvc
    NTupleSvc.Output = [ "FILE1 DATAFILE='tiletb.root' OPT='NEW'" ]
