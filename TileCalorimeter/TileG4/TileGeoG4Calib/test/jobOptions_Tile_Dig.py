# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#==============================================================
#
# Job options file for Digitization
#
# Full ATLAS setup, TileCal only digitization
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

# - Number of events to be processed
if not 'EvtMax' in dir():
    EvtMax = -1

# - input file with hits 
if not 'PoolHitsInput' in dir():
    PoolHitsInput = 'Hits.pool.root'

# - output file with digits 
if not 'PoolRDOOutput' in dir():
    PoolRDOOutput = 'DIGITS.pool.root'

if not 'ConddbTag' in dir():
    ConddbTag = 'OFLCOND-MC12-SDR-27'

if not 'DetDescrVersion' in dir():
    DetDescrVersion = 'ATLAS-GEO-20-00-01'

if not 'doD3PD' in dir():
    doD3PD = False

if not 'doHitNtuple' in dir():
    doHitNtuple = True

if not 'doRawChannelNtuple' in dir():
    doRawChannelNtuple = True

if not 'doTileNtuple' in dir():
    doTileNtuple = True

#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import jobproperties
#jobproperties.AthenaCommonFlags.AllowIgnoreConfigError=False #This job will stop if an include fails.

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


DetFlags.writeRDOPool.Truth_setOn()
DetFlags.writeRDOPool.Tile_setOn()

# - print flags 
DetFlags.Print()


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

#from IOVDbSvc.CondDB import conddb
#conddb.setGlobalTag(ConddbTag);
#log.info( "ConddbTag = %s" % (ConddbTag) )

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion = DetDescrVersion 
log.info( "DetDescrVersion = %s" % (jobproperties.Global.DetDescrVersion()) )
TileCablingType = 4 

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
# GeoModelSvc.IgnoreTagDifference = True
log.info( "GeoModelSvc.AtlasVersion = %s" % (GeoModelSvc.AtlasVersion) )
GeoModelSvc.TileVersionOverride = "TileCal-GEO-08"
log.info( "GeoModelSvc.TileVersionOverride = %s" % (GeoModelSvc.TileVersionOverride) )

GeoModelSvc.DetectorTools["TileDetectorTool"].GeometryConfig = "FULL"
GeoModelSvc.DetectorTools["TileDetectorTool"].OutputLevel = 1

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import jobproperties
# jobproperties.Digitization.doInDetNoise=True
jobproperties.Digitization.doCaloNoise=True
# This tag must be specified for dowstream jobs
jobproperties.Digitization.IOVDbGlobalTag = ConddbTag
# jobproperties.Digitization.doMuonNoise=True
# jobproperties.Digitization.doMinimumBias=True
# jobproperties.Digitization.numberOfCollisions=2.3
# jobproperties.Digitization.minBiasInputCols=["", "", "" ]
# jobproperties.Digitization.doCavern=True
# jobproperties.Digitization.numberOfCavern=2
# jobproperties.Digitization.cavernInputCols=["", ""]
# jobproperties.Digitization.doBeamGas=True
# jobproperties.Digitization.numberOfBeamGas=0.5
# jobproperties.Digitization.beamGasInputCols=["", ""]

include.block ( "TileL2Algs/TileL2Algs_jobOptions.py" )

include( "Digitization/Digitization.py" )

# avoid MBTS hits
#topSequence.TileHitVecToCnt.DigitizationTool.TileHitVectors=['TileHitVec']

# write all digits
#if DetFlags.writeRDOPool.Tile_on():
#    streamRDO.ItemList+=["TileDigitsContainer#TileDigitsCnt"]

svcMgr.MessageSvc.OutputLevel = OutputLevel
svcMgr.AthenaPoolCnvSvc.MaxFileSizes=["16000000000"]
#svcMgr.StatusCodeSvc.SuppressCheck = True
svcMgr.StatusCodeSvc.AbortOnError = False
#print svcMgr.StatusCodeSvc


#--------------------------------------------------------------
# Creating ntuples
#--------------------------------------------------------------

theApp.HistogramPersistency = "ROOT"

if doD3PD:
    
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    alg = MSMgr.NewRootStream( "D3PD", "tile_d3pd.root", "truth" )

    from TruthD3PDMaker.TruthD3PDMakerKeys import TruthD3PDKeys
    from TruthD3PDMaker.TruthD3PDMakerFlags import TruthD3PDFlags

    from TruthD3PDAnalysis.GenObjectsFilterTool import *

    from TruthD3PDMaker.GenVertexD3PDObject import GenVertexD3PDObject
    alg += GenVertexD3PDObject( 10, filter = AllTrackFilterTool() )

    from TruthD3PDMaker.GenParticleD3PDObject import GenParticleD3PDObject
    alg += GenParticleD3PDObject( 10, filter = AllTrackFilterTool() )

    from CaloD3PDMaker.TileHitD3PDObject import TileHitD3PDObject
    alg += TileHitD3PDObject( 0 )

    from CaloD3PDMaker.TileDigitD3PDObject import TileDigitD3PDObject
    alg += TileDigitD3PDObject(2,prefix='TileDigit_',sgkey='TileDigitsCnt')

    from CaloD3PDMaker.TileRawChannelD3PDObject import TileRawChannelD3PDObject
    alg += TileRawChannelD3PDObject(2,prefix='TileRch_',sgkey='TileRawChannelCnt')


if doTileNtuple:
    
    if not hasattr(svcMgr,"THistSvc"):
        from GaudiSvc.GaudiSvcConf import THistSvc
        svcMgr+=THistSvc()
    svcMgr.THistSvc.Output += [ "AANT DATAFILE='tile_aant.root' OPT='RECREATE' " ]
    #svcMgr.THistSvc.MaxFileSize = 32768
    

if (doHitNtuple or doRawChannelNtuple):

    if not hasattr(svcMgr, 'NtupleSvc'):
        from GaudiSvc.GaudiSvcConf import NTupleSvc
        svcMgr += NTupleSvc()
    NTupleSvc = svcMgr.NTupleSvc
    NTupleSvc.Output = [ "NTUP DATAFILE='tile_ntup.root' OPT='NEW'" ]


if doHitNtuple:
    #--- Create TileCal h32 ntuple with all G4 hits ---------
    from TileRec.TileRecConf import TileHitVecToNtuple
    theTileHitToNtuple = TileHitVecToNtuple()
    topSequence += theTileHitToNtuple
    theTileHitToNtuple.MaxLength=99999
    theTileHitToNtuple.TileHitVector = "TileHitVec"
    theTileHitToNtuple.NTupleLoc = "/NTUP"


if doRawChannelNtuple:
    #--- Create TileCal h70 ntuple with all raw channels ---------
    from TileRec.TileRecConf import TileRawChannelToNtuple
    theTileRawChannelToNtuple = TileRawChannelToNtuple()
    topSequence += theTileRawChannelToNtuple
    theTileRawChannelToNtuple.TileRawChannelContainer = "TileRawChannelCnt"
    theTileRawChannelToNtuple.NTupleLoc = "/NTUP"


if doTileNtuple:
    doSim=True
    #--- Create TileCal h2000 ntuple with RawChannels ---------
    include( "TileRec/TileDefaults_jobOptions.py" )
    include( "TileRec/TileNtuple_jobOptions.py" )
    TileNtuple.TileRawChannelContainer  = "TileRawChannelCnt"
    TileNtuple.TileBeamElemContainer = ""
    TileNtuple.TileDigitsContainer = "TileDigitsCnt"
    TileNtuple.TileDigitsContainerFlt = ""
    TileNtuple.StreamName  = "AANT"

# from TileSimAlgs.TileSimAlgsConf import *
# theTilePulseForTileMuonReceiver=TilePulseForTileMuonReceiver()
# topSequence += theTilePulseForTileMuonReceiver
