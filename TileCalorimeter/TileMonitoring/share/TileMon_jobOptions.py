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
        
    ToolSvc += TileCellMon;    
    ManagedAthenaTileMon.AthenaMonTools += [ TileCellMon ];


    include ("TileMonitoring/TileMonTower_jobOptions.py")

    ToolSvc += CfgMgr.TileTowerMonTool(name                  = 'TileTowerMon'
                                       , OutputLevel         = INFO
                                       , towersContainerName = "TileTower"
                                       , FillHistogramsForL1Triggers = []
                                       , histoPathBase       = "/Tile/Tower");

    ManagedAthenaTileMon.AthenaMonTools += [ ToolSvc.TileTowerMon ];


    include ("TileMonitoring/TileMonTopoCluster_jobOptions.py")

    ToolSvc += CfgMgr.TileClusterMonTool(name                    = 'TileClusterMon'
                                         , OutputLevel           = INFO
                                         , clustersContainerName = "TileTopoCluster"
                                         , FillHistogramsForL1Triggers = []
                                         , histoPathBase         = "/Tile/Cluster");

    ManagedAthenaTileMon.AthenaMonTools += [ ToolSvc.TileClusterMon ];
    
    if (jobproperties.Beam.beamType() == 'cosmics' or jobproperties.Beam.beamType() == 'singlebeam'):

        ToolSvc += CfgMgr.TileMuonFitMonTool(name                  = 'TileMuonFitMon'
                                             , OutputLevel         = INFO
                                             , UseLVL1             = DQMonFlags.useTrigger()
                                             , FillHistogramsForL1Triggers = []
                                             , histoPathBase       = "/Tile/MuonFit")

        ManagedAthenaTileMon.AthenaMonTools += [ ToolSvc.TileMuonFitMon ]


    ToolSvc += CfgMgr.TileMuIdMonTool(name                  = 'TileMuIdMon'
                                      , OutputLevel         = INFO
                                      , FillHistogramsForL1Triggers = []
                                      , histoPathBase       = "/Tile/Muid")

    ManagedAthenaTileMon.AthenaMonTools += [ ToolSvc.TileMuIdMon ];


    ToolSvc += CfgMgr.TileL2MonTool(name                  = 'TileL2MuMon'
                                    , OutputLevel         = INFO
                                    , FillHistogramsForL1Triggers = []
                                    , histoPathBase       = "/Tile/L2Muon")

    ManagedAthenaTileMon.AthenaMonTools += [ ToolSvc.TileL2MuMon ];


    if (jobproperties.Beam.beamType() == 'collisions'):
        jetPtMin = 20000.
        ToolSvc += CfgMgr.TileJetMonTool(name                = 'TileJetMonTool'
                                         , OutputLevel       = INFO
                                         , jetPtMin          = jetPtMin
                                         , jetEtaMax         = 1.6
                                         , jetCollectionName = 'AntiKt4EMTopoJets'
                                         , do_1dim_histos    = False
                                         , do_2dim_histos    = False
                                         , do_enediff_histos = False
                                         , energyChanMin     = 2000 # Default: 2000
                                         , energyChanMax     = 4000 # Default: 4000
                                         , enediff_threshold = 2000
                                         , do_energy_profiles= True
                                         , do_event_cleaning = True
                                         , do_jet_cleaning   = False
                                         # , useJVTTool        = jvt
                                         # , useJetCleaning    = cleaning
                                         , jet_JVT_threshold = 0.59
                                         , jet_JVT_pTmax     = 120000 # MeV
                                         , histoPathBase     = "/Tile/Jet")

        from JetRec.JetRecFlags import jetFlags
        if jetFlags.useTracks():
            jet_tracking_eta_limit = 2.4
            jvt = CfgMgr.JetVertexTaggerTool('JVT')
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
            ToolSvc.TileJetMonTool.do_jet_cleaning        = True
            ToolSvc.TileJetMonTool.useJVTTool             = jvt
            ToolSvc.TileJetMonTool.useJetCleaning         = cleaning
            ToolSvc.TileJetMonTool.useEventCleaning       = ecTool
            ToolSvc.TileJetMonTool.jet_tracking_eta_limit = jet_tracking_eta_limit
        if DQMonFlags.monManDataType == 'heavyioncollisions':
            if not rec.doHIP(): 
                ToolSvc.TileJetMonTool.jetCollectionName = 'AntiKt4HIJets'
            ToolSvc.TileJetMonTool.do_event_cleaning = False
            ToolSvc.TileJetMonTool.do_jet_cleaning   = False
        
        ManagedAthenaTileMon.AthenaMonTools += [ ToolSvc.TileJetMonTool ]

    if (not 'doTileTMDBRawChannelMon' in dir() or doTileTMDBRawChannelMon)  and (DQMonFlags.useTrigger() and rec.doTrigger()):
        from TileConditions.TileCondToolConf import getTileCondToolTMDB
        tileCondToolTMDB = getTileCondToolTMDB('COOL')
        if tileCondToolTMDB:
            ToolSvc += tileCondToolTMDB
            ToolSvc += CfgMgr.TileTMDBRawChannelMonTool(name            = 'TileTMDBRawChannelDspMon'
                                                        , OutputLevel   = INFO
                                                        , NotDSP        = False
                                                        , Efficiency    = True
                                                        , TileRawChannelContainer = "MuRcvRawChCnt"
                                                        , TileCondToolTMDB = tileCondToolTMDB
                                                        , histoPathBase = "/Tile/TMDBRawChannel/Dsp")
            ManagedAthenaTileMon.AthenaMonTools += [ ToolSvc.TileTMDBRawChannelDspMon ];



if  tileRawMon:

    TileMBTSMon = CfgMgr.TileMBTSMonTool(name              = 'TileMBTSMon'
                                         , OutputLevel     = INFO
                                         , histoPathBase   = "/Tile/MBTS"
                                         , LVL1ConfigSvc   = "TrigConf::TrigConfigSvc/TrigConfigSvc"
                                         , doOnline        = athenaCommonFlags.isOnline()
                                         , readTrigger     = False
                                         , UseTrigger      = False
                                         , FillHistogramsPerMBTS = False)
#                                         , readTrigger     = DQMonFlags.useTrigger());


    from AthenaCommon.GlobalFlags import globalflags
    if globalflags.InputFormat() == 'pool':
        TileMBTSMon.TileDigitsContainerName = 'TileDigitsFlt'

    ToolSvc += TileMBTSMon;
    ManagedAthenaTileMon.AthenaMonTools += [ TileMBTSMon ]

    from TileRecUtils.TileRecFlags import jobproperties
    if jobproperties.TileRecFlags.readDigits():
        if 'doTileRODMon' in dir() and doTileRODMon:
            ToolSvc += CfgMgr.TileRODMonTool( name              = 'TileRODMon'
                                              , OutputLevel     = INFO
                                              , histoPathBase   = "/Tile/ROD"
                                              , FillHistogramsForL1Triggers = []
                                              , doOnline        = athenaCommonFlags.isOnline())

            ManagedAthenaTileMon.AthenaMonTools += [ ToolSvc.TileRODMon ]

        ToolSvc += CfgMgr.TileDigiNoiseMonTool(name                  = 'TileDigiNoiseMon'
                                               , OutputLevel         = INFO
                                               , TileDigitsContainer = "TileDigitsCnt"
                                               , histoPathBase       = "/Tile/DigiNoise" 
                                               , TriggerChain        = "HLT_noalg_cosmiccalo_L1RD1_EMPTY"
                                               , TrigDecisionTool    = getattr(ToolSvc, DQMonFlags.nameTrigDecTool()) if DQMonFlags.useTrigger() else ""
                                               , TriggerTypes        = [ 0x82 ]);

        if globalflags.InputFormat() == 'pool':
            ToolSvc.TileDigiNoiseMon.TileDigitsContainer = 'TileDigitsFlt'

        ManagedAthenaTileMon.AthenaMonTools += [ ToolSvc.TileDigiNoiseMon ]


    ToolSvc += CfgMgr.TileDQFragLWMonTool(name                       = 'TileDQFragMon'
                                          , OutputLevel              = INFO
                                          , TileRawChannelContainer  = jobproperties.TileRecFlags.TileRawChannelContainer()
                                          , TileDigitsContainer      = "TileDigitsCnt"
                                          , NegAmpHG                 = -200.
                                          , NegAmpLG                 = -15.
                                          , SkipMasked               = True
                                          , SkipGapCells             = True
                                          , doOnline                 = athenaCommonFlags.isOnline()
                                          , CheckDCS                 = hasattr(ServiceMgr, "TileDCSSvc")
                                          , histoPathBase            = "/Tile/DMUErrors")


    if globalflags.InputFormat() == 'pool':
        ToolSvc.TileDQFragMon.TileDigitsContainer = 'TileDigitsFlt'

    ManagedAthenaTileMon.AthenaMonTools += [ ToolSvc.TileDQFragMon ]

topSequence += ManagedAthenaTileMon;
print ManagedAthenaTileMon;
