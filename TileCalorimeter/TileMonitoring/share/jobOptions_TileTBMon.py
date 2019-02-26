#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

#*****************************************************************
#
# """topOptions file for Tile Laser Reconstruciton and Monitoring in Athena""" 
# """This topOptions is intended to test the monitoring code"""
#=================================================================


from os import system, popen
from AthenaCommon.Logging import logging
log = logging.getLogger( 'jobOptions_TileTBMon.py' )


MonitorOutput = 'Tile'
TileDCS = False

if not 'MaxEnergy' in dir():
    MaxEnergy = 60.0
if not 'MaxTotalEnergy' in dir():
    MaxTotalEnergy = 100.0

if not 'TestOnline' in dir():
    TestOnline = False

if TestOnline:
    doOnline = True;
    storeHisto = True;

if not 'TileFELIX' in dir():
     if 'PublishName' in dir():
         TileFELIX = True
     else:
         TileFELIX = False


if not 'UseDemoCabling' in dir():
    UseDemoCabling = 2016

if TileFELIX:
    UseDemoCabling = 2017

if not 'InputDirectory' in dir():
    InputDirectory = "/data1/daq"    

if not 'TileBiGainRun' in dir():
    TileBiGainRun = False

if not 'TileMonoRun' in dir():
    TileMonoRun = False

if TileMonoRun:
    TileRunType = 8
    TileBiGainRun = False

if not 'TilePedRun' in dir():
    TilePedRun = False

if TilePedRun:
    TileRunType = 4
    TileBiGainRun = True

if not 'TileCisRun' in dir():
    TileCisRun = False

if TileCisRun:
    TileRunType = 8
    TileBiGainRun = True



from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
if not 'TileSummaryUpdateFrequency' in dir():
    if athenaCommonFlags.isOnline():
        TileSummaryUpdateFrequency = 100
    else:
        TileSummaryUpdateFrequency = 0

def FindFile(path, runinput):

    run = str(runinput)

    while len(run) < 7:
        run = '0' + run
        
    files = []
    fullname = []

    if path.startswith("/castor") :
        for f in popen('nsls %(path)s | grep %(run)s' % {'path': path, 'run':run }):
            files.append(f)

    elif path.startswith("/eos") :
        for f in popen('eos ls %(path)s | grep %(run)s' % {'path': path, 'run':run }):
            files.append(f)

    else:
        for f in popen('ls  %(path)s | grep %(run)s' % {'path': path, 'run':run }):
            files.append(f)
            

    for nn in range(len(files)):
        temp = files[nn].split('\n')
        fullname.append(path + '/' + temp[0])

    return [fullname,run]

# include Flags jobOption
include( "TileMonitoring/TileRec_FlagOptions.py" )

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon import CfgMgr
toolSvc = CfgMgr.ToolSvc()


# set global flags
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('commis')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')

from AthenaCommon.BeamFlags import jobproperties     
jobproperties.Beam.beamType.set_Value_and_Lock(beamType)

# reset everything which is not needed
from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()  #Switched off to avoid geometry
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.Muon_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOn()
DetFlags.readRDOBS.Tile_setOn()

# Switch off DCS
DetFlags.dcs.Tile_setOff()

DetFlags.Print()

from RecExConfig.RecFlags import rec
rec.doLArg = False

# set online flag if neeed

if athenaCommonFlags.isOnline() or doOnline or doStateless:
    athenaCommonFlags.isOnline=True
    log.info( 'athenaCommonFlags.isOnline = True : Online Mode' )
    if doStateless:
        athenaCommonFlags.isOnlineStateless=True
        log.info( 'athenaCommonFlags.isOnlineStateless = True : Stateless Online Mode' )


#-----------------
# ByteSream Input 
#-----------------

if not athenaCommonFlags.isOnline() or TestOnline:

    include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
    include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )

    if not 'InputDirectory' in dir():
        InputDirectory="/castor/cern.ch/grid/atlas/t0/perm/DAQ"
    if not 'RunNumber' in dir():
        RunNumber=0
    if not 'RunFromLocal' in dir():
        if InputDirectory=="." or RunNumber<10:
            RunFromLocal=True
        else:
            RunFromLocal=False
   
    if not 'FileNameVec' in dir():
        if not 'FileName' in dir():

            tmp = FindFile(InputDirectory,RunNumber)
            FileNameVec  = tmp[0]
            FormattedRunNumber = tmp[1]
            
        else:
            FileNameVec = [ InputDirectory+'/'+FileName ]
            FormattedRunNumber = RunNumber
    else:
       FormattedRunNumber = RunNumber

    svcMgr.EventSelector.SkipEvents = EvtMin
    theApp.EvtMax = EvtMax


    log.info( "InputDirectory is " + str(InputDirectory) )
    log.info( "RunNumber is " + str(FormattedRunNumber) )
    log.info( "FullFileName is " + str(FileNameVec) )
    log.info( "Skip Events is " + str(EvtMin) )
    log.info( "Max events is " + str(EvtMax) )

    svcMgr.ByteStreamInputSvc.FullFileName = FileNameVec
    svcMgr.EventSelector.MaxBadEvents = MaxBadEvents
   
    athenaCommonFlags.FilesInput = FileNameVec

    projectName = FileNameVec[0].split('/').pop().split('.')[0]
    log.info( "Project name is " + projectName )
    rec.projectName = projectName


# init DetDescr
from AthenaCommon.GlobalFlags import jobproperties
if not 'DetDescrVersion' in dir():
    DetDescrVersion = 'ATLAS-R2-2015-04-00-00'
jobproperties.Global.DetDescrVersion = DetDescrVersion 
log.info( "DetDescrVersion = %s" % (jobproperties.Global.DetDescrVersion() ))

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.IgnoreTagDifference = True
log.info( "GeoModelSvc.AtlasVersion = %s" % (GeoModelSvc.AtlasVersion) )

# Setup Db stuff
if TileUseCOOL:
    from IOVDbSvc.CondDB import conddb
    log.info( 'Tile COOL tag: ' + tileCOOLtag )
    conddb.setGlobalTag(tileCOOLtag)

    db = '/afs/cern.ch/user/t/tiledemo/public/efmon/condb/tileSqlite.db'
    from TileConditions.TileCoolMgr import tileCoolMgr
    tileCoolMgr.setGlobalDbConn(db)




# setting option to build frag->ROB mapping at the begin of run
ByteStreamCnvSvc = Service( "ByteStreamCnvSvc" )

ByteStreamCnvSvc.ROD2ROBmap = [
    # frag-to-ROB mapping, bypassing ROD ID which is not unique
    "0xff",  "0x500000",
    "0x13",  "0x500000",
    "0x1",   "0x500000",
    "0x2",   "0x500000",
    "0x3",   "0x500000",
    "0x200", "0x0",
    "0x201", "0x0",
    "0x402", "0x0",
    ]

if RunNumber >= 600000 and RunNumber < 611300:
    ByteStreamCnvSvc.ROD2ROBmap += ["0x101", "0x1"]
elif RunNumber >= 611300:
    ByteStreamCnvSvc.ROD2ROBmap += ["0x100", "0x1"]
if TileFELIX:
    ByteStreamCnvSvc.ROD2ROBmap += ["0x203", "0x500006"]


topSequence += CfgMgr.xAODMaker__EventInfoCnvAlg()

TileCorrectTime = False    
doTileOptATLAS = False

if TileMonoRun or TileCisRun:
    doTileOpt2 = False
    doTileFit = True

if TileFELIX:
    doTileOpt2 = False
    doTileFit = True
    topSequence += CfgMgr.TileDigitsGainFilter(HighGainThreshold = 4095)

# load conditions data
include( "TileRec/TileDefaults_jobOptions.py" )
include( "TileConditions/TileConditions_jobOptions.py" )


# set reconstruction flags and reconstruct data
from TileRecUtils.TileRecFlags import jobproperties
jobproperties.TileRecFlags.calibrateEnergy.set_Value_and_Lock(False) #don't need pC in raw channels, keep ADC counts
jobproperties.TileRecFlags.noiseFilter.set_Value_and_Lock(0) #Enable noise filter tool
jobproperties.TileRecFlags.BestPhaseFromCOOL.set_Value_and_Lock(False) #Use best phase from COOL
jobproperties.TileRecFlags.doTileOverflowFit.set_Value_and_Lock(True)
include( "TileRec/TileRec_jobOptions.py" )


if doTileCells:
    # enable interpolation for dead cells
    doCaloNeighborsCorr = False
    if TileBiGainRun:
        include( "TileRec/TileCellMaker_jobOptions_doublegain.py" )
        ToolSvc.TileCellBuilderLG.SkipGain = 1
        ToolSvc.TileCellBuilderHG.SkipGain = 0
    else:
        include('TileRec/TileCellMaker_jobOptions.py')
        ToolSvc.TileCellBuilder.UseDemoCabling = UseDemoCabling
        ToolSvc.TileCellBuilder.maskBadChannels = False

from TileRecUtils.TileDQstatusAlgDefault import TileDQstatusAlgDefault
TileDQstatusAlgDefault()

#----------------
# TileMonitoring
#----------------
if doMonitoring:

    if TileBiGainRun:
        CellContainerMonitored = 'AllCaloHG'
    else:
        CellContainerMonitored = 'AllCalo'

    topSequence += CfgMgr.AthenaMonManager( "TileTBMonManager"
                                            , ManualRunLBSetup    = True
                                            , ManualDataTypeSetup = True
                                            , Environment         = "online"
                                            , FileKey             = MonitorOutput
                                            , Run                 = RunNumber
                                            , LumiBlock           = 1)


    #-------------------------------
    #   Tile raw channel time monitoring
    #-------------------------------
    TileTBMonTool = CfgMgr.TileTBMonTool ( name                  = 'TileTBMonTool'
                                           , histoPathBase       = '/Tile/TestBeam'
                                           # , doOnline            = athenaCommonFlags.isOnline()
                                           , CellsContainerID  = '' # used to check if the current event is collision
                                           , MBTSCellContainerID = '' # used to check if the current event is collision
                                           # Masked format: 'module gain channel,channel' (channels are separated by comma)
                                           , Masked = ['LBC02 0 46', 'LBC02 1 46', 
                                                       'LBC04 0 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47',
                                                       'LBC04 1 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47']
                                           , CellContainer       = CellContainerMonitored
                                           , MaxTotalEnergy      = MaxTotalEnergy)


    topSequence.TileTBMonManager.AthenaMonTools += [ TileTBMonTool ]
    
    TileTBBeamMonTool = CfgMgr.TileTBBeamMonTool ( name                  = 'TileTBBeamMonTool'
                                                   , histoPathBase       = '/Tile/TestBeam/BeamElements'
                                                   # , doOnline            = athenaCommonFlags.isOnline()
                                                   , CellsContainerID  = '' # used to check if the current event is collision
                                                   , MBTSCellContainerID = '' # used to check if the current event is collision
                                                   , CutEnergyMin = 40
                                                   , CutEnergyMax = 70 
                                                   , CellContainer       = CellContainerMonitored );



    topSequence.TileTBMonManager.AthenaMonTools += [ TileTBBeamMonTool ]
    


    if TileBiGainRun:
        TileTBCellMonToolHG = CfgMgr.TileTBCellMonTool ( name                  = 'TileTBCellMonToolHG'
                                                         , histoPathBase       = '/Tile/TestBeam'
                                                         # , doOnline            = athenaCommonFlags.isOnline()
                                                         , CellsContainerID  = '' # used to check if the current event is collision
                                                         , MBTSCellContainerID = '' # used to check if the current event is collision
                                                         , cellsContainerName       = 'AllCaloHG'
                                                         , FillTimeHistograms = True
                                                         , energyThresholdForTime = 1.0
                                                         , MaxEnergy = MaxEnergy
                                                         , MaxTotalEnergy = MaxTotalEnergy)
        
        topSequence.TileTBMonManager.AthenaMonTools += [ TileTBCellMonToolHG ]
        
        
        TileTBCellMonToolLG = CfgMgr.TileTBCellMonTool ( name                  = 'TileTBCellMonToolLG'
                                                         , histoPathBase       = '/Tile/TestBeam/LG'
                                                         # , doOnline            = athenaCommonFlags.isOnline()
                                                         , CellsContainerID  = '' # used to check if the current event is collision
                                                         , MBTSCellContainerID = '' # used to check if the current event is collision
                                                         , cellsContainerName       = 'AllCaloLG'
                                                         , FillTimeHistograms = True
                                                         , energyThresholdForTime = 1.0
                                                         , MaxEnergy = MaxEnergy
                                                         , MaxTotalEnergy = MaxTotalEnergy)
        
        topSequence.TileTBMonManager.AthenaMonTools += [ TileTBCellMonToolLG ]

    else:
        TileTBCellMonTool = CfgMgr.TileTBCellMonTool ( name                  = 'TileTBCellMonTool'
                                                       , histoPathBase       = '/Tile/TestBeam'
                                                       # , doOnline            = athenaCommonFlags.isOnline()
                                                       , CellsContainerID  = '' # used to check if the current event is collision
                                                       , MBTSCellContainerID = '' # used to check if the current event is collision
                                                       , cellsContainerName       = 'AllCalo'
                                                       , FillTimeHistograms = True
                                                       , energyThresholdForTime = 1.0
                                                       , MaxEnergy = MaxEnergy
                                                       , MaxTotalEnergy = MaxTotalEnergy)

        topSequence.TileTBMonManager.AthenaMonTools += [ TileTBCellMonTool ]

    

    if TileMonoRun or TileCisRun or TilePedRun:
        TileRawChannelMon = CfgMgr.TileRawChannelMonTool ( name              = "TileRawChannelMon"
                                                           , OutputLevel     = WARNING
                                                           , histoPathBase   = "/Tile/RawChannel"
                                                           , book2D          = False
                                                           , PlotDSP         = False
                                                           , runType         = 9 if TileMonoRun else TileRunType
                                                           , TileRawChannelContainer = 'TileRawChannelOpt2' if TilePedRun else 'TileRawChannelFit'
                                                           , SummaryUpdateFrequency = TileSummaryUpdateFrequency)
        topSequence.TileTBMonManager.AthenaMonTools += [ TileRawChannelMon ]


    if TilePedRun:
        TileDigiNoiseMon = CfgMgr.TileDigiNoiseMonTool(name               = 'TileDigiNoiseMon'
                                                       , OutputLevel        = WARNING
                                                       , TileDigitsContainer = "TileDigitsCnt"
                                                       , histoPathBase = "/Tile/DigiNoise"
                                                       , FillEmptyFromDB = False
                                                       , FillPedestalDifference = False
                                                       , CheckDCS           = TileUseDCS
                                                       , SummaryUpdateFrequency = TileSummaryUpdateFrequency );
        
        topSequence.TileTBMonManager.AthenaMonTools += [ TileDigiNoiseMon ];

    print topSequence.TileTBMonManager


import os
# -- use root histos --
# THistService for native root in Athena
if not  athenaCommonFlags.isOnline() or storeHisto or athenaCommonFlags.isOnlineStateless():
    if not hasattr(svcMgr, 'THistSvc'):
        from GaudiSvc.GaudiSvcConf import THistSvc
        svcMgr += THistSvc('THistSvc')
    if os.path.exists(RootHistOutputFileName):
        os.remove(RootHistOutputFileName)
    svcMgr.THistSvc.Output = [MonitorOutput + " DATAFILE='" + RootHistOutputFileName + "' OPT='RECREATE'"]
else:
    from TrigServices.TrigServicesConf import TrigMonTHistSvc
    trigmonTHistSvc = TrigMonTHistSvc("THistSvc")
    svcMgr += trigmonTHistSvc



#To read CTP RESULTS and DSP Raw Channels
if not hasattr( svcMgr, "ByteStreamAddressProviderSvc" ):
    from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
    svcMgr += ByteStreamAddressProviderSvc()

svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
                                                   "TileRawChannelContainer/TileRawChannelCnt",
                                                   "CTP_RDO/CTP_RDO",
                                                   "CTP_RIO/CTP_RIO",
                                                  ]


svcMgr.MessageSvc.defaultLimit= MsgLinesLimit
svcMgr.MessageSvc.OutputLevel = OutputLevel
svcMgr.MessageSvc.Format = "% F%35W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.useColors = useColors
#svcMgr.HistorySvc.OutputLevel = 3

theApp.EvtMax = EvtMax

from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
svcMgr += AthenaEventLoopMgr()
svcMgr.AthenaEventLoopMgr.EventPrintoutInterval = 100

if TileUseCOOL:
    from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
    svcMgr += DBReplicaSvc(UseCOOLSQLite=False)

if hasattr (svcMgr.ToolSvc, 'TileRawChannelBuilderOpt2Filter') and False:
    svcMgr.ToolSvc.TileRawChannelBuilderOpt2Filter.OutputLevel = DEBUG


if hasattr (svcMgr.ToolSvc, 'TileCellBuilder') and False:
    svcMgr.ToolSvc.TileCellBuilder.OutputLevel = DEBUG



if TileFELIX:
    ServiceMgr.TileInfoLoader.NSamples = 16
    ServiceMgr.TileInfoLoader.TrigSample = 3
    topSequence.TileRChMaker.TileDigitsContainer = 'TileDigitsFiltered'
    TileRawChannelBuilderFitFilter = topSequence.TileRChMaker.TileRawChannelBuilder['TileRawChannelBuilderFitFilter']
    TileRawChannelBuilderFitFilter.ExtraSamplesRight = 0
    TileRawChannelBuilderFitFilter.ExtraSamplesLeft = 0
    TileRawChannelBuilderFitFilter.MaxIterate = 9
    TileRawChannelBuilderFitFilter.SaturatedSample = 4095
    #TileRawChannelBuilderFitFilter.OutputLevel = DEBUG

#toolSvc += CfgMgr.TileROD_Decoder(OutputLevel = VERBOSE)
