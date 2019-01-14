#==============================================================
#
# Job options file for Digitization
#
# Full ATLAS or TileCal Testbeam setup, TileCal only digitization
#
#==============================================================

from AthenaCommon.AppMgr import theApp
svcMgr = theApp.serviceMgr()

from AthenaCommon import CfgMgr

from AthenaCommon.Logging import logging
logTileDig = logging.getLogger( 'jobOptions_Tile_Dig.py' )

#---  Output printout level ----------------------------------- 
#output threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
if not 'OutputLevel' in dir():
    OutputLevel = 3
svcMgr.MessageSvc.OutputLevel = OutputLevel
svcMgr.MessageSvc.defaultLimit = 1000000
svcMgr.MessageSvc.Format = '% F%60W%S%7W%R%T %0W%M'
svcMgr.MessageSvc.useColors = False

# get a handle on topalg
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# configuration flags
#--------------------------------------------------------------

# - Number of events to be processed
if not 'EvtMax' in dir():
    EvtMax = -1

if not 'RunNumber' in dir():
    RunNumber = 310000

if not 'FileSuffix' in dir():
    FileSuffix = ''

if not 'TileTB' in dir():
    TileTB = ('Geo' in dir() and (Geo=='5B' or Geo=='3B' or Geo=='2B2EB' or Geo=='2B1EB'))

if TileTB:
    ConddbTag = 'OFLCOND-MC12-SDR-27'
    DetGeo = 'ctbh8'
    DetDescrVersion = 'ATLAS-CTB-01'
    if not 'Geo' in dir():
        Geo = '5B'
    TileVersionOverride = 'TileTB-%s-00' % Geo
    if not 'doHitNtuple' in dir():
        doHitNtuple = True

    if not 'doD3PD' in dir():
        doD3PD = True
        doD3PDHit = False
        doD3PDHitInfo = False
        doD3PDCellInfo = False

    if not 'PoolHitsInput' in dir():
        PoolHitsInput = 'tiletb%s.HITS.pool.root' % FileSuffix

    if not 'PoolRDOOutput' in dir():
        PoolRDOOutput = 'tiletb%s.DIGITS.pool.root' % FileSuffix

    FileSuffix = 'tb'+FileSuffix

    if not 'TruthSGKey' in dir():
        TruthSGKey = 'TruthEvent'

# - input file with hits 
if not 'PoolHitsInput' in dir():
    PoolHitsInput = 'HITS%s.pool.root' % FileSuffix

# - output file with digits 
if not 'PoolRDOOutput' in dir():
    PoolRDOOutput = 'DIGITS%s.pool.root' % FileSuffix

if not 'NTUPOutput' in dir():
    NTUPOutput = 'tile%s.ntup.root' % FileSuffix

if not 'AANTOutput' in dir():
    AANTOutput = 'tile%s.aant.root' % FileSuffix

if not 'D3PDOutput' in dir():
    D3PDOutput = 'tile%s.d3pd.root' % FileSuffix

if not 'ConddbTag' in dir():
    ConddbTag = 'OFLCOND-MC16-SDR-25'

if not 'DetGeo' in dir():
    DetGeo = 'atlas'

if not 'DetDescrVersion' in dir():
    DetDescrVersion = 'ATLAS-R2-2015-03-01-00'

# commented out - do not set any override by default
#if not 'TileVersionOverride' in dir():
#    TileVersionOverride = 'TileCal-GEO-08'

if not 'doD3PD' in dir():
    doD3PD = False

if not 'doD3PDHit' in dir():
    doD3PDHit = False

if not 'doD3PDHitInfo' in dir():
    doD3PDHitInfo = False

if not 'doD3PDDigit' in dir():
    doD3PDDigit = False

if not 'doD3PDRawChannel' in dir():
    doD3PDRawChannel = False

if not 'doD3PDCell' in dir():
    doD3PDCell = False

if not 'doD3PDCellInfo' in dir():
    doD3PDCellInfo = True

if not 'doD3PDMBTS' in dir():
    doD3PDMBTS = False

if not 'doHitNtuple' in dir():
    doHitNtuple = False

if not 'doDigitsNtuple' in dir():
    doDigitsNtuple = False

if not 'doRawChannelNtuple' in dir():
    doRawChannelNtuple = False

if not 'doTileNtuple' in dir():
    doTileNtuple = False

if not 'doRDO' in dir():
    doRDO = False

if not 'doCaloNoise' in dir():
    doCaloNoise = True

#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import jobproperties
#jobproperties.AthenaCommonFlags.AllowIgnoreConfigError=False #This job will stop if an include fails.

theApp.EvtMax = EvtMax
jobproperties.AthenaCommonFlags.EvtMax = EvtMax
jobproperties.AthenaCommonFlags.PoolHitsInput=[ PoolHitsInput ]
jobproperties.AthenaCommonFlags.PoolRDOOutput=PoolRDOOutput

#--------------------------------------------------------------------
# DetFlags. Use to turn on/off individual subdetector or LVL1 trigger
#--------------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags

DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.LVL1_setOff()
#DetFlags.sTGCMicromegas_setOff()

DetFlags.Truth_setOn()

DetFlags.Tile_setOn()

# - switch off tasks
DetFlags.pileup.all_setOff()
DetFlags.simulate.all_setOff()
DetFlags.makeRIO.all_setOff()
DetFlags.writeBS.all_setOff()
DetFlags.readRDOBS.all_setOff()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRDOPool.all_setOff()
#DetFlags.writeRDOPool.all_setOn()
DetFlags.readRIOPool.all_setOff()
DetFlags.writeRIOPool.all_setOff()
DetFlags.simulateLVL1.all_setOff()

if doRDO:
    DetFlags.writeRDOPool.Truth_setOn()
    DetFlags.writeRDOPool.Tile_setOn()
else:
    DetFlags.writeRDOPool.all_setOff()

# - print flags 
DetFlags.Print()


#--------------------------------------------------------------
# Global flags. Like eg the DD version:
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DataSource.set_Value_and_Lock('geant4')
globalflags.InputFormat.set_Value_and_Lock('pool')

from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.beamType.set_Value_and_Lock('collisions')

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag(ConddbTag);
logTileDig.info( 'ConddbTag = %s' % (ConddbTag) )

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion = DetDescrVersion 
logTileDig.info( 'DetDescrVersion = %s' % (jobproperties.Global.DetDescrVersion()) )

from AtlasGeoModel import SetGeometryVersion
jobproperties.Global.DetGeo = DetGeo
from AtlasGeoModel import GeoModelInit
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.IgnoreTagDifference = True
logTileDig.info( 'GeoModelSvc.AtlasVersion = %s' % (GeoModelSvc.AtlasVersion) )
if 'TileVersionOverride' in dir():
    GeoModelSvc.TileVersionOverride = TileVersionOverride
    logTileDig.info( 'GeoModelSvc.TileVersionOverride = %s' % (GeoModelSvc.TileVersionOverride) )

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import jobproperties
# jobproperties.Digitization.doInDetNoise=True
jobproperties.Digitization.doCaloNoise = doCaloNoise
# This tag must be specified for dowstream jobs
jobproperties.Digitization.IOVDbGlobalTag = ConddbTag
if not TileTB:
    jobproperties.Digitization.simRunNumber = RunNumber
    jobproperties.Digitization.dataRunNumber = RunNumber
# jobproperties.Digitization.doMuonNoise=True
# jobproperties.Digitization.doMinimumBias=True
# jobproperties.Digitization.numberOfCollisions=2.3
# jobproperties.Digitization.minBiasInputCols=['', '', '' ]
# jobproperties.Digitization.doCavern=True
# jobproperties.Digitization.numberOfCavern=2
# jobproperties.Digitization.cavernInputCols=['', '']
# jobproperties.Digitization.doBeamGas=True
# jobproperties.Digitization.numberOfBeamGas=0.5
# jobproperties.Digitization.beamGasInputCols=['', '']

topSequence += CfgMgr.xAODMaker__EventInfoCnvAlg()

if doD3PDHit or doD3PDDigit or doD3PDRawChannel or doD3PDCell or doD3PDCellInfo or doD3PDMBTS or doDigitsNtuple or doRawChannelNtuple or doTileNtuple or doRDO:

    include.block ( 'TileL2Algs/TileL2Algs_jobOptions.py' )

    if TileTB:
        # special settings for TileConditions, to make sure that COOL is not used
        if not 'TileUseCOOL' in dir():
            TileUseCOOL=False
        elif TileUseCOOL:
            jobproperties.Digitization.simRunNumber = RunNumber
            jobproperties.Digitization.dataRunNumber = RunNumber

        # setting Fit and Opt2 method only
        from TileRecUtils.TileRecFlags import jobproperties
        jobproperties.TileRecFlags.doTileFit = True
        jobproperties.TileRecFlags.doTileOpt2 = True

    include( 'Digitization/Digitization.py' )

    if TileTB:
        jobproperties.TileRecFlags.TileRawChannelContainer = "TileRawChannelFit"
        # avoid MBTS hits
        ToolSvc.TileHitVecToCntTool.TileHitVectors=['TileHitVec']
        # change threshold in fit method
        ToolSvc.TileRawChannelBuilderFitFilter.NoiseThresholdRMS = 3.

else:
    include( "TileConditions/TileConditions_jobOptions.py" )
    include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
    svcMgr.EventSelector.InputCollections = [ PoolHitsInput ]

if doD3PDCell or doD3PDCellInfo or doD3PDMBTS :
    # create TileCell from TileRawChannel and store it in CaloCellContainer
    include( 'TileRec/TileCellMaker_jobOptions.py' )
    ToolSvc.TileCellBuilder.maskBadChannels = False

# write all digits
#if DetFlags.writeRDOPool.Tile_on():
#    streamRDO.ItemList+=['TileDigitsContainer#TileDigitsCnt']

svcMgr.MessageSvc.OutputLevel = OutputLevel
svcMgr.AthenaPoolCnvSvc.MaxFileSizes=['16000000000']
#svcMgr.StatusCodeSvc.SuppressCheck = True
svcMgr.StatusCodeSvc.AbortOnError = False
#print svcMgr.StatusCodeSvc


#--------------------------------------------------------------
# Creating ntuples
#--------------------------------------------------------------

theApp.HistogramPersistency = 'ROOT'

if doD3PD:
    
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    alg = MSMgr.NewRootStream( 'D3PD', D3PDOutput, 'truth' )


    if 'TruthSGKey' in dir():
        from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
        D3PDMakerFlags.TruthSGKey = TruthSGKey

    from TruthD3PDMaker.TruthD3PDMakerKeys import TruthD3PDKeys
    from TruthD3PDMaker.TruthD3PDMakerFlags import TruthD3PDFlags

    from TruthD3PDAnalysis.GenObjectsFilterTool import *

    from TruthD3PDMaker.GenVertexD3PDObject import GenVertexD3PDObject
    alg += GenVertexD3PDObject( 10, filter = AllTrackFilterTool() )

    from TruthD3PDMaker.GenParticleD3PDObject import GenParticleD3PDObject
    alg += GenParticleD3PDObject( 10, filter = AllTrackFilterTool() )

    if doD3PDHitInfo:
        from CaloD3PDMaker.TileHitInfoD3PDObject import TileHitInfoD3PDObject
        alg += TileHitInfoD3PDObject ( 0, sgkey='TileHitVec', prefix='Tile_')

    if doD3PDHit:
        from CaloD3PDMaker.TileHitD3PDObject import TileHitD3PDObject
        alg += TileHitD3PDObject ( 0,prefix='TileHit_' )

    if doD3PDDigit:
        from CaloD3PDMaker.TileDigitD3PDObject import TileDigitD3PDObject
        alg += TileDigitD3PDObject ( 2, prefix='tiledigit_',sgkey='TileDigitsCnt')

    if doD3PDRawChannel:
        from CaloD3PDMaker.TileRawChannelD3PDObject import TileRawChannelD3PDObject
        alg += TileRawChannelD3PDObject ( 2, prefix='tileraw_',sgkey='TileRawChannelCnt')

    if doD3PDCellInfo:
        from CaloD3PDMaker.CaloInfoD3PDObject import CaloInfoD3PDObject
        alg += CaloInfoD3PDObject ( 0, sgkey='AllCalo', prefix='calo_')

    if doD3PDCell:
        from CaloD3PDMaker.TileDetailsD3PDObject import TileDetailsD3PDObject
        alg += TileDetailsD3PDObject ( 1, sgkey='AllCalo', prefix='tile_', Kinematics_WriteEtaPhi=True)

    if doD3PDMBTS:
        from CaloD3PDMaker.MBTSD3PDObject import MBTSD3PDObject
        alg += MBTSD3PDObject ( 1,  prefix='mbts_', sgkey='MBTSContainer')


if doTileNtuple:
    
    if not hasattr(svcMgr, 'THistSvc'):
        from GaudiSvc.GaudiSvcConf import THistSvc
        svcMgr += THistSvc()
    svcMgr.THistSvc.Output += [ "AANT DATAFILE='"+AANTOutput+"' OPT='RECREATE' " ]
    #svcMgr.THistSvc.MaxFileSize = 32768
    

if (doHitNtuple or doDigitsNtuple or doRawChannelNtuple):

    if not hasattr(svcMgr, 'NtupleSvc'):
        from GaudiSvc.GaudiSvcConf import NTupleSvc
        svcMgr += NTupleSvc()
    NTupleSvc = svcMgr.NTupleSvc
    NTupleSvc.Output = [ "NTUP DATAFILE='"+NTUPOutput+"' OPT='NEW' " ]


if doHitNtuple:
    #--- Create TileCal h32 ntuple with all G4 hits ---------
    from TileRec.TileRecConf import TileHitVecToNtuple
    theTileHitToNtuple = TileHitVecToNtuple()
    topSequence += theTileHitToNtuple
    theTileHitToNtuple.MaxLength=99999
    theTileHitToNtuple.TileHitVector = 'TileHitVec'
    theTileHitToNtuple.NTupleLoc = '/NTUP'


if doDigitsNtuple:
    #--- Create TileCal h40 ntuple with all digits ---------
    from TileRec.TileRecConf import TileDigitsToNtuple
    theTileDigitsToNtuple = TileDigitsToNtuple()
    topSequence += theTileDigitsToNtuple
    theTileDigitsToNtuple.TileDigitsContainer = 'TileDigitsCnt'
    theTileDigitsToNtuple.NTupleLoc = '/NTUP'


if doRawChannelNtuple:
    #--- Create TileCal h70 ntuple with all raw channels ---------
    from TileRec.TileRecConf import TileRawChannelToNtuple
    theTileRawChannelToNtuple = TileRawChannelToNtuple()
    topSequence += theTileRawChannelToNtuple
    theTileRawChannelToNtuple.TileRawChannelContainer = 'TileRawChannelFit'
    theTileRawChannelToNtuple.NTupleLoc = '/NTUP'


from TileRecUtils.TileDQstatusAlgDefault import TileDQstatusAlgDefault
dqStatus = TileDQstatusAlgDefault()

if doTileNtuple:
    doSim = True
    if TileTB:
        #--- Create TileCal h1000 ntuple with RawChannels ---------
        topSequence += CfgMgr.TileTBAANtuple( TBperiod = 2003, NSamples = 7, BSInput = False, 
                                              CompleteNtuple = False, UnpackAdder = False,
                                              TileRawChannelContainerFlat = "", 
                                              TileRawChannelContainerOpt = "TileRawChannelCnt",
                                              TileRawChannelContainerFit = "TileRawChannelFit",
                                              TileBeamElemContainer = "",
                                              TileLaserObject = "",
                                              beamFragList = [ ]  )
        dqStatus.TileBeamElemContainer = ''
        dqStatus.TileDigitsContainer = 'TileDigitsCnt'
    else:
        #--- Create TileCal h2000 ntuple with RawChannels ---------
        include( 'TileRec/TileDefaults_jobOptions.py' )
        include( 'TileRec/TileNtuple_jobOptions.py' )
        TileNtuple.TileRawChannelContainer  = 'TileRawChannelCnt'
        TileNtuple.TileBeamElemContainer = ''
        TileNtuple.TileDigitsContainer = 'TileDigitsCnt'
        TileNtuple.TileDigitsContainerFlt = ''
        TileNtuple.StreamName  = 'AANT'

# from TileSimAlgs.TileSimAlgsConf import *
# theTilePulseForTileMuonReceiver=TilePulseForTileMuonReceiver()
# topSequence += theTilePulseForTileMuonReceiver
