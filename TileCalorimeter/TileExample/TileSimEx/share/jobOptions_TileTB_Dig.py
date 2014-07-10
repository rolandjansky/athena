#==============================================================
#
# Job options file for Digitization
#
# Standalone Tile TB during 2000-2003 
#
#==============================================================

from AthenaCommon.Logging import logging
logDigitization_flags = logging.getLogger( 'Digitization' )

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


#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------

# - number of events to be processed (all events by default)
if not 'EvtMax' in dir():
    EvtMax = -1
theApp.EvtMax = EvtMax

# - input file with hits 
if not 'PoolHitsInput' in dir():
    PoolHitsInput = 'Tile_MyOutputFile.root'
# - output file with digits 
if not 'PoolRDOOutput' in dir():
    PoolRDOOutput = 'Tile_MyOutputFile-Dig.root'

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.AllowIgnoreConfigError=False #This job will stop if an include fails.
athenaCommonFlags.EvtMax = EvtMax
athenaCommonFlags.PoolHitsInput=[ PoolHitsInput ]
athenaCommonFlags.PoolRDOOutput=PoolRDOOutput
athenaCommonFlags.FilesInput=athenaCommonFlags.PoolHitsInput.get_Value()


#--------------------------------------------------------------
# Detector flags
#--------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags

# - Select detectors
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.em_setOff()
DetFlags.FCal_setOff()
DetFlags.HEC_setOff()
DetFlags.Tile_setOn()
DetFlags.Muon_setOff()
DetFlags.Truth_setOn()

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
DetFlags.readRDOPool.Tile_setOff()
DetFlags.writeRDOPool.Tile_setOn()
# - print flags 
DetFlags.Print()


#--------------------------------------------------------------
# GeoModel stuff
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource = 'geant4'
globalflags.InputFormat = 'pool'
globalflags.DetGeo = "ctbh8"

# change TileBarrelOnly to True for 3 barrels setup
if not 'TileBarrelOnly' in dir():
    TileBarrelOnly=False

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
if TileBarrelOnly:
    GeoModelSvc.TileVersionOverride = "TileTB-3B-00"
else:
    GeoModelSvc.TileVersionOverride = "TileTB-2B2EB-00"


#--------------------------------------------------------------
# Digitization
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags
# - Disable noise if needed
#digitizationFlags.doCaloNoise.set_Value(False)

StreamRDO_ItemList = []

# - flags for TileCal digitization
doSim=True
Tile2004=False
Tile2003Noise=True; # 1.2 ADC counts HG noise istead of 1.6 ADC counts
# Set the following line to True to get full noise pattern with coherent noise info
doTileHitToRawChannelCoherNoise=False

# special settings for TileConditions, to make sure that COOL is not used
TileUseCOOL=False
OfcFromCOOL=False
from TileConditions.TileCondProxyConf import getTileCondProxy
from TileConditions.TileCondToolConf import getTileCondToolNoiseSample
if not hasattr(ToolSvc, "TileCondToolNoiseSample"):
    ToolSvc += getTileCondToolNoiseSample('FILE')

# digitization
include( "TileSimAlgs/TileTBDigitization_jobOptions.py" )

# important !!! don't put positive TriggerTime here (unless you know what you are doing) !!!
#theTileHitVecToCnt.HitTimeFlag=2;       # shift time of all hits so that
#theTileHitVecToCnt.TriggerTime = -25.0; # time of the earliest hit in event is at -25 ns

if DetFlags.writeRDOPool.Tile_on():
    #StreamRDO_ItemList+=["TileDigitsContainer#*"]
    StreamRDO_ItemList+=["TileRawChannelContainer#*"]
    StreamRDO_ItemList+=["TileBeamElemContainer#*"]
    

#--------------------------------------------------------------
# Read Simulation MetaData (unless override flag set to True)
#--------------------------------------------------------------
if 'ALL' in digitizationFlags.overrideMetadata.get_Value():
    logDigitization_flags.info("Skipping input file MetaData check.")
else:
    include("Digitization/DigitizationReadMetaData.py")


#--------------------------------------------------------------
# MC Truth info
#--------------------------------------------------------------
if DetFlags.Truth_on():
    include( "Digitization/TruthDigitization.py" )


#--------------------------------------------------------------
# Write Digitization MetaData
#--------------------------------------------------------------
if DetFlags.writeRDOPool.any_on():
   include("Digitization/DigitizationWriteMetaData.py")


#--------------------------------------------------------------
# Pool specific
#--------------------------------------------------------------
# Pool input (Change this to use a different file)
if not hasattr(svcMgr, 'EventSelector'):
    import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = athenaCommonFlags.PoolHitsInput()

# Pool Output (Change this to use a different file)
if DetFlags.writeRDOPool.any_on():
    StreamRDO_ItemList+=["EventInfo#*"];           
    StreamRDO_ItemList+=["McEventCollection#*"]
    StreamRDO_ItemList+=["CaloCalibrationHitContainer#*"]
    from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
    streamRDO = AthenaPoolOutputStream("StreamRDO", athenaCommonFlags.PoolRDOOutput.get_Value(), asAlg=True)
    streamRDO.ForceRead = True
    streamRDO.ItemList = StreamRDO_ItemList
    streamRDO.OutputFile = athenaCommonFlags.PoolRDOOutput()
    del StreamRDO_ItemList
    #--- Persistency of test beam-layout in POOL -----------------
    #include( "TBDetDescrAlg/TBDetDescrLoader_jobOptions.py" )


#--------------------------------------------------------------
# D3PD and/or standalone ntuples
#--------------------------------------------------------------

# Create various ntuples with hits or rawChannels amplitudes
# Hit energy is the energy which comes from Geant4 without sampling correction 
# and RawChannel amplitude is normally in pCb
# check carefully which pCb/GeV factor is applied in digitization (1.05 or 1.0)

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

if not 'doCBNT' in dir():
    doCBNT = False

if doCBNT:
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


if not 'doCalibNtuple' in dir():
    doCalibNtuple = False

if doCalibNtuple and doCBNT:
    include( "CaloCalibHitRec/CalibHit_CBNTAA_jobOptions.py" )


if not 'doHitNtuple' in dir():
    doHitNtuple = True

if doHitNtuple:
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

