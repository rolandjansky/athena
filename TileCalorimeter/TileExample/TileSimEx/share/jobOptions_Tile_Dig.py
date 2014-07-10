#==============================================================
#
# Job options file for Digitization
#
# Full ATLAS setup, TileCal only digitization
#
#==============================================================

from AthenaCommon.AppMgr import ServiceMgr

from AthenaCommon.Logging import logging
logDigitization_flags = logging.getLogger( 'Digitization' )

#---  Output printout level ----------------------------------- 
#output threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
if not 'OutputLevel' in dir():
    OutputLevel = 3
ServiceMgr.MessageSvc.OutputLevel = OutputLevel
ServiceMgr.MessageSvc.defaultLimit = 1000000
ServiceMgr.MessageSvc.Format = "% F%60W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.useColors = False

#--- Number of events to be processed (default is 10) ---------
if not 'EvtMax' in dir():
    EvtMax = 10000
theApp.EvtMax = EvtMax

# get a handle on topalg
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


#--- Detector flags -------------------------------------------
from AthenaCommon.GlobalFlags import GlobalFlags
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

# - input file with hits 
PoolHitsInput = 'atlas_MyOutputFile.root'
# - output file with digits 
PoolRDOOutput = 'atlas_MyOutputFile-Dig.root'

#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.AllowIgnoreConfigError=False #This job will stop if an include fails.

jobproperties.AthenaCommonFlags.EvtMax = EvtMax
jobproperties.AthenaCommonFlags.PoolHitsInput=[ PoolHitsInput ]
jobproperties.AthenaCommonFlags.PoolRDOOutput=PoolRDOOutput


# - Disable noise if needed
#from Digitization.DigitizationFlags import jobproperties
#jobproperties.Digitization.doCaloNoise.set_Value(False)
#print jobproperties.Digitization.doCaloNoise()


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
# - print flags 
DetFlags.Print()

# flags for TileCal digitization
doSim=True
Tile2004=True

#---  Load PartProp into the Detector store ---------------
if not hasattr(ServiceMgr, 'PartPropSvc'):
    from PartPropSvc.PartPropSvcConf import PartPropSvc
    ServiceMgr += PartPropSvc()
#--- Pool specific --------------------------------------------
# - General Pool converters
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
# - Pool input (Change this to use a different file)
ServiceMgr.EventSelector.InputCollections = [ PoolHitsInput ]

if DetFlags.writeRDOPool.any_on():
    # Pool Output
    include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )
    from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
    Stream1 = AthenaPoolOutputStream( "Stream1" )
    # Stream1.OutputLevel=1
    Stream1.ItemList+=["EventInfo#*"];           
    Stream1.ItemList+=["McEventCollection#*"]
    Stream1.ItemList+=["CaloCalibrationHitContainer#*"]
    Stream1.ForceRead=TRUE;  #force read of output data 
    Stream1.OutputFile = PoolRDOOutput

#--- Initial checks  -------------------------------------------
#check job configuration
if not(DetFlags.geometry.any_on()):
    AthError( "this digitization job needs some DetFlags.geometry On" )
if not(DetFlags.digitize.any_on()):
    AthError( "this *digitization* job needs some DetFlags.digitize On" )
if not(DetFlags.writeRDOPool.any_on()):
    log.warning( "this digitization job will not write any RDO object" )

#--- GeoModel stuff -------------------------------------------
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DataSource.set_geant4()
GlobalFlags.DetGeo.set_atlas()

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-GEO-06-00-00"
log.info( "DetDescrVersion = %s" % (jobproperties.Global.DetDescrVersion()) )

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
#from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
#GeoModelSvc = GeoModelSvc()
#GeoModelSvc.TileVersionOverride = "TileCal-CSC-01"
#log.info( "GeoModelSvc.AtlasVersion = %s" % (GeoModelSvc.AtlasVersion) )
#log.info( "GeoModelSvc.TileVersionOverride = %s" % (GeoModelSvc.TileVersionOverride) )

include( "TileConditions/TileConditions_jobOptions.py" ) 
include( "TileSimAlgs/TileDigitization_jobOptions.py" )

#--------------------------------------------------------------
# Read Simulation MetaData (unless override flag set to True)
#--------------------------------------------------------------
from Digitization.DigitizationFlags import jobproperties
if not ('ALL' in jobproperties.Digitization.overrideMetadata.get_Value()):
    include("Digitization/DigitizationReadMetaData.py")

#
if DetFlags.writeRDOPool.Tile_on():
    Stream1.ItemList+=["TileRawChannelContainer#*"]
    
#--- MC Truth info ---------------------------------------------
if DetFlags.Truth_on():
    include( "Digitization/TruthDigitization.py" )

#--------------------------------------------------------------
# Write Digitization MetaData
#--------------------------------------------------------------
if DetFlags.writeRDOPool.any_on():
    include("Digitization/DigitizationWriteMetaData.py")

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

    if not hasattr(ServiceMgr,"THistSvc"):
        from GaudiSvc.GaudiSvcConf import THistSvc
        ServiceMgr+=THistSvc()
    ServiceMgr.THistSvc.Output += ["AANT DATAFILE='tile_aa.root' OPT='RECREATE'" ]

    from AnalysisTools.AnalysisToolsConf import AANTupleStream
    AANTupleStream = AANTupleStream()
    topSequence += AANTupleStream
    AANTupleStream.ExtraRefNames = [ "" ]
    AANTupleStream.OutputName = "tile_aa.root"
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
    doHitNtuple = True

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


if not 'doRawChannelNtuple' in dir():
    doRawChannelNtuple = True

if doRawChannelNtuple:
    if CBNTAthenaAware:
        from TileRec.TileRecConf import CBNTAA_TileRawChannel
        CBNTAA_TileRawChannel = CBNTAA_TileRawChannel()
        CBNT_AthenaAware += CBNTAA_TileRawChannel
    else:
        #--- Create TileCal h70 ntuple with all raw channels ---------
        from TileRec.TileRecConf import TileRawChannelToNtuple
        theTileRawChannelToNtuple = TileRawChannelToNtuple()
        topSequence += theTileRawChannelToNtuple
        theTileRawChannelToNtuple.TileRawChannelContainer = "TileRawChannelCnt"


if not 'doTileNtuple' in dir():
    doTileNtuple = False

if doTileNtuple:
    doTileFlat = False
    doTileOpt = False
    doTileFit = False
    #--- Create TileCal h1000 ntuple with RawChannels ---------
    include( "TileTBRec/TileTBDefaults_jobOptions.py" )
    include( "TileTBRec/TileTBNtuple_jobOptions.py" )
    TileTBNtuple.TileDigitsContainer = "TileDigitsCnt"
    TileTBNtuple.TileBeamElemContainer = ""
    TileTBNtuple.TileRawChannelContainerFit  = ""
    TileTBNtuple.TileRawChannelContainerFlat = "TileRawChannelCnt"
    TileTBNtuple.drawerList = [ "-1" ]
    TileTBNtuple.drawerType = [ ]
    TileTBNtuple.beamFragList = [ ]

theApp.HistogramPersistency = "ROOT"

if not doCBNT and (doHitNtuple or doRawChannelNtuple or doTileNtuple):
    if not hasattr(ServiceMgr, 'NtupleSvc'):
        from GaudiSvc.GaudiSvcConf import NTupleSvc
        ServiceMgr += NTupleSvc()
    NTupleSvc = ServiceMgr.NTupleSvc
    NTupleSvc.Output = [ "FILE1 DATAFILE='tile.root' OPT='NEW'" ]
