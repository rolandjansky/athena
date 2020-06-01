#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

#**************************************************************
#
# jopOptions file for Tile Monitoring in Athena 
#
#==============================================================

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags 
from AthenaCommon import CfgMgr
from AthenaCommon.BeamFlags import jobproperties

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AlgSequence import AthSequencer
condSequence = AthSequencer("AthCondSeq")
tileCheckDCS = hasattr(condSequence, 'TileDCSCondAlg')

from AthenaMonitoring.DQMonFlags import DQMonFlags

if not 'rec' in dir():
    from RecExConfig.RecFlags import rec

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
ManagedAthenaTileMon = AthenaMonManager( "ManagedAthenaTileMon" ) #topSequence.ManagedAthenaTileMon

from AthenaCommon.AppMgr import ServiceMgr

ManagedAthenaTileMon.FileKey = DQMonFlags.monManFileKey()
ManagedAthenaTileMon.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
ManagedAthenaTileMon.DataType            = DQMonFlags.monManDataType()
ManagedAthenaTileMon.Environment         = DQMonFlags.monManEnvironment()
ManagedAthenaTileMon.ManualRunLBSetup    = DQMonFlags.monManManualRunLBSetup()
ManagedAthenaTileMon.Run                 = DQMonFlags.monManRun()
ManagedAthenaTileMon.LumiBlock           = DQMonFlags.monManLumiBlock()

if DQMonFlags.monManEnvironment() == 'tier0Raw':
    tileRawMon=True
    tileESDMon=False
elif DQMonFlags.monManEnvironment() == 'tier0ESD':
    tileRawMon=False
    tileESDMon=True
else: # it should be 'tier0'
    tileRawMon=True
    tileESDMon=True

from TileRecUtils.TileDQstatusAlgDefault import TileDQstatusAlgDefault
TileDQstatusAlgDefault()

if tileESDMon:

    TileCellMon = CfgMgr.TileCellMonTool(name                 = 'TileCellMon'
                                         , OutputLevel        = INFO
                                         , doOnline           = athenaCommonFlags.isOnline()
                                         , cellsContainerName = "AllCalo"
                                         , FillHistogramsForL1Triggers = []
                                         , FillChannelTimePerSampleHistograms = False
                                         , FillCellTimeAndEnergyDifferenceHistograms = False
                                         , FillDigitizerEnergyVsLBHistograms = False
                                         , histoPathBase      = "/Tile/Cell")

    if (jobproperties.Beam.beamType() == 'cosmics' or jobproperties.Beam.beamType() == 'singlebeam'):
        TileCellMon.FillTimeHistograms = True
        TileCellMon.energyThresholdForTime = 150.0
        
    ManagedAthenaTileMon.AthenaMonTools += [ TileCellMon ];


    include ("TileMonitoring/TileMonTower_jobOptions.py")

    TileTowerMon = CfgMgr.TileTowerMonTool(name                  = 'TileTowerMon'
                                       , OutputLevel         = INFO
                                       , towersContainerName = "TileTower"
                                       , FillHistogramsForL1Triggers = []
                                       , histoPathBase       = "/Tile/Tower");

    ManagedAthenaTileMon.AthenaMonTools += [ TileTowerMon ];


    include ("TileMonitoring/TileMonTopoCluster_jobOptions.py")

    TileClusterMon = CfgMgr.TileClusterMonTool(name                    = 'TileClusterMon'
                                         , OutputLevel           = INFO
                                         , clustersContainerName = "TileTopoCluster"
                                         , FillHistogramsForL1Triggers = []
                                         , histoPathBase         = "/Tile/Cluster");

    ManagedAthenaTileMon.AthenaMonTools += [ TileClusterMon ];
    
    if (jobproperties.Beam.beamType() == 'cosmics' or jobproperties.Beam.beamType() == 'singlebeam'):

        TileMuonFitMon = CfgMgr.TileMuonFitMonTool(name                  = 'TileMuonFitMon'
                                             , OutputLevel         = INFO
                                             , UseLVL1             = DQMonFlags.useTrigger()
                                             , FillHistogramsForL1Triggers = []
                                             , histoPathBase       = "/Tile/MuonFit")

        ManagedAthenaTileMon.AthenaMonTools += [ TileMuonFitMon ]


    TileMuIdMon = CfgMgr.TileMuIdMonTool(name                  = 'TileMuIdMon'
                                      , OutputLevel         = INFO
                                      , FillHistogramsForL1Triggers = []
                                      , histoPathBase       = "/Tile/Muid")

    ManagedAthenaTileMon.AthenaMonTools += [ TileMuIdMon ];


    TileL2MuMon = CfgMgr.TileL2MonTool(name                  = 'TileL2MuMon'
                                    , OutputLevel         = INFO
                                    , FillHistogramsForL1Triggers = []
                                    , histoPathBase       = "/Tile/L2Muon")

    ManagedAthenaTileMon.AthenaMonTools += [ TileL2MuMon ];


    if (jobproperties.Beam.beamType() == 'collisions'):
        from JetRec.JetRecFlags import jetFlags
        jetPtMin = 20000.
        jetContainer = 'AntiKt4EMTopoJets'
        doJetCleaning = jetFlags.useTracks()
        doEventCleaning = True

        if DQMonFlags.monManDataType == 'heavyioncollisions':
            if not rec.doHIP():
                jetContainer = 'AntiKt4HIJets'
            doEventCleaning = False
            doJetCleaning   = False


        TileJetMonTool = CfgMgr.TileJetMonTool(name                = 'TileJetMonTool'
                                         , OutputLevel       = INFO
                                         , jetPtMin          = jetPtMin
                                         , jetEtaMax         = 1.6
                                         , jetCollectionName = jetContainer
                                         , do_1dim_histos    = False
                                         , do_2dim_histos    = False
                                         , do_enediff_histos = False
                                         , energyChanMin     = 2000 # Default: 2000
                                         , energyChanMax     = 4000 # Default: 4000
                                         , enediff_threshold = 2000
                                         , do_energy_profiles= True
                                         , do_event_cleaning = doEventCleaning
                                         , do_jet_cleaning   = doJetCleaning
                                         # , useJVTTool        = jvt
                                         # , useJetCleaning    = cleaning
                                         , jet_JVT_threshold = 0.59
                                         , jet_JVT_pTmax     = 120000 # MeV
                                         , histoPathBase     = "/Tile/Jet")

        if doJetCleaning:
            jet_tracking_eta_limit = 2.4
            jvt = CfgMgr.JetVertexTaggerTool('JVT')
            jvt.JetContainer = jetContainer
            ToolSvc += jvt
            cleaning = CfgMgr.JetCleaningTool("MyCleaningTool")
            cleaning.CutLevel = "LooseBad"
            cleaning.DoUgly = False
            ToolSvc += cleaning
            ecTool                 = CfgMgr.ECUtils__EventCleaningTool("MyEventCleaningTool")
            ecTool.JetCleaningTool = cleaning
            ecTool.PtCut           = jetPtMin
            ecTool.EtaCut          = jet_tracking_eta_limit
            ecTool.JvtDecorator    = "passJvt"
            ecTool.OrDecorator     = "passOR"
            ecTool.CleaningLevel   = cleaning.CutLevel
            ToolSvc += ecTool
            TileJetMonTool.useJVTTool             = jvt
            TileJetMonTool.useJetCleaning         = cleaning
            TileJetMonTool.useEventCleaning       = ecTool
            TileJetMonTool.jet_tracking_eta_limit = jet_tracking_eta_limit


        ManagedAthenaTileMon.AthenaMonTools += [ TileJetMonTool ]

    if (not 'doTileTMDBRawChannelMon' in dir() or doTileTMDBRawChannelMon)  and (DQMonFlags.useTrigger() and rec.doTrigger()):
        from TileConditions.TileCondToolConf import getTileCondToolTMDB
        tileCondToolTMDB = getTileCondToolTMDB('COOL')
        if tileCondToolTMDB:
            ToolSvc += tileCondToolTMDB
            TileTMDBRawChannelDspMon = CfgMgr.TileTMDBRawChannelMonTool(name            = 'TileTMDBRawChannelDspMon'
                                                        , OutputLevel   = INFO
                                                        , NotDSP        = False
                                                        , Efficiency    = True
                                                        , TileRawChannelContainer = "MuRcvRawChCnt"
                                                        , TileCondToolTMDB = tileCondToolTMDB
                                                        , histoPathBase = "/Tile/TMDBRawChannel/Dsp")
            ManagedAthenaTileMon.AthenaMonTools += [ TileTMDBRawChannelDspMon ];



if  tileRawMon:

    if 'doTileMonOld' in dir() and doTileMonOld:
        TileMBTSMon = CfgMgr.TileMBTSMonTool(name              = 'TileMBTSMon'
                                             , OutputLevel     = INFO
                                             , histoPathBase   = "/Tile/MBTS_OLD"
                                             , LVL1ConfigSvc   = "TrigConf::TrigConfigSvc/TrigConfigSvc"
                                             , doOnline        = athenaCommonFlags.isOnline()
                                             , readTrigger     = False
                                             , UseTrigger      = False
                                             , FillHistogramsPerMBTS = False)
        #                                         , readTrigger     = DQMonFlags.useTrigger());


        from AthenaCommon.GlobalFlags import globalflags
        if globalflags.InputFormat() == 'pool':
            TileMBTSMon.TileDigitsContainerName = 'TileDigitsFlt'

        ManagedAthenaTileMon.AthenaMonTools += [ TileMBTSMon ]

    from TileMonitoring.TileMBTSMonitorAlgorithm import TileMBTSMonitoringConfigOld
    topSequence += TileMBTSMonitoringConfigOld(DQMonFlags)


    from TileRecUtils.TileRecFlags import jobproperties
    if jobproperties.TileRecFlags.readDigits():
        if 'doTileRODMon' in dir() and doTileRODMon:
            TileRODMon = CfgMgr.TileRODMonTool( name              = 'TileRODMon'
                                              , OutputLevel     = INFO
                                              , histoPathBase   = "/Tile/ROD"
                                              , FillHistogramsForL1Triggers = []
                                              , doOnline        = athenaCommonFlags.isOnline())

            ManagedAthenaTileMon.AthenaMonTools += [ TileRODMon ]

        TileDigiNoiseMon = CfgMgr.TileDigiNoiseMonTool(name                  = 'TileDigiNoiseMon'
                                               , OutputLevel         = INFO
                                               , TileDigitsContainer = "TileDigitsCnt"
                                               , histoPathBase       = "/Tile/DigiNoise" 
                                               , TriggerChain        = "HLT_noalg_cosmiccalo_L1RD1_EMPTY"
                                               , TrigDecisionTool    = getattr(ToolSvc, DQMonFlags.nameTrigDecTool()) if DQMonFlags.useTrigger() else ""
                                               , TriggerTypes        = [ 0x82 ]
                                               , CheckDCS                 = tileCheckDCS)

        if globalflags.InputFormat() == 'pool':
            TileDigiNoiseMon.TileDigitsContainer = 'TileDigitsFlt'

        ManagedAthenaTileMon.AthenaMonTools += [ TileDigiNoiseMon ]


    if 'doTileMonOld' in dir() and doTileMonOld:
        TileDQFragMon = CfgMgr.TileDQFragLWMonTool(name                       = 'TileDQFragMon'
                                                   , OutputLevel              = INFO
                                                   , TileRawChannelContainer  = jobproperties.TileRecFlags.TileRawChannelContainer()
                                                   , TileDigitsContainer      = "TileDigitsCnt"
                                                   , NegAmpHG                 = -200.
                                                   , NegAmpLG                 = -15.
                                                   , SkipMasked               = True
                                                   , SkipGapCells             = True
                                                   , doOnline                 = athenaCommonFlags.isOnline()
                                                   , CheckDCS                 = tileCheckDCS
                                                   , histoPathBase            = "/Tile/DMUErrors_OLD")


        if globalflags.InputFormat() == 'pool':
            TileDQFragMon.TileDigitsContainer = 'TileDigitsFlt'

        ManagedAthenaTileMon.AthenaMonTools += [ TileDQFragMon ]


    from TileMonitoring.TileDQFragMonitorAlgorithm import TileDQFragMonitoringConfigOld
    topSequence += TileDQFragMonitoringConfigOld(DQMonFlags)

if jp.ConcurrencyFlags.NumThreads() == 0:
    topSequence += ManagedAthenaTileMon
    printfunc (ManagedAthenaTileMon)
