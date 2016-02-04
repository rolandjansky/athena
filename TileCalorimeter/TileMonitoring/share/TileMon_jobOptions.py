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
                                       , histoPathBase       = "/Tile/Tower");

    ManagedAthenaTileMon.AthenaMonTools += [ ToolSvc.TileTowerMon ];


    include ("TileMonitoring/TileMonTopoCluster_jobOptions.py")

    ToolSvc += CfgMgr.TileClusterMonTool(name                    = 'TileClusterMon'
                                         , OutputLevel           = INFO
                                         , clustersContainerName = "TileTopoCluster"
                                         , histoPathBase         = "/Tile/Cluster");

    ManagedAthenaTileMon.AthenaMonTools += [ ToolSvc.TileClusterMon ];
    
    if (jobproperties.Beam.beamType() == 'cosmics' or jobproperties.Beam.beamType() == 'singlebeam'):

        ToolSvc += CfgMgr.TileMuonFitMonTool(name                  = 'TileMuonFitMon'
                                             , OutputLevel         = INFO
                                             , UseLVL1             = DQMonFlags.useTrigger()
                                             , histoPathBase       = "/Tile/MuonFit")

        ManagedAthenaTileMon.AthenaMonTools += [ ToolSvc.TileMuonFitMon ]


    ToolSvc += CfgMgr.TileMuIdMonTool(name                  = 'TileMuIdMon'
                                      , OutputLevel         = INFO
                                      , histoPathBase       = "/Tile/Muid")

    ManagedAthenaTileMon.AthenaMonTools += [ ToolSvc.TileMuIdMon ];


    ToolSvc += CfgMgr.TileL2MonTool(name                  = 'TileL2MuMon'
                                    , OutputLevel         = INFO
                                    , histoPathBase       = "/Tile/L2Muon")

    ManagedAthenaTileMon.AthenaMonTools += [ ToolSvc.TileL2MuMon ];


    if (jobproperties.Beam.beamType() == 'collisions'):

        jvt = CfgMgr.JetVertexTaggerTool('JVT')
        ToolSvc += jvt
        cleaning = CfgMgr.JetCleaningTool("MyCleaningTool")
        cleaning.CutLevel = "LooseBad"
        cleaning.DoUgly = False
        ToolSvc += cleaning
        ToolSvc += CfgMgr.TileJetMonTool(name                = 'TileJetMonTool'
                                         , OutputLevel       = INFO
                                         , jetPtMin          = 20000.0
                                         , jetEtaMax         = 1.6
                                         , jetCollectionName = 'AntiKt4EMTopoJets'
                                         , do_1dim_histos    = False
                                         , do_2dim_histos    = False
                                         , do_enediff_histos = False
                                         , energyChanMin     = 2000
                                         , energyChanMax     = 4000
                                         , enediff_threshold = 2000
                                         , do_event_cleaning = True
                                         , do_jet_cleaning   = True
                                         , useJVTTool        = jvt
                                         , useJetCleaning    = cleaning
                                         , jet_JVT_threshold = 0.64
                                         , histoPathBase     = "/Tile/Jet")

        if DQMonFlags.monManDataType == 'heavyioncollisions':
            ToolSvc.TileJetMonTool.jetCollectionName = 'AntiKt4HIJets'
            ToolSvc.TileJetMonTool.do_event_cleaning = False
            ToolSvc.TileJetMonTool.do_jet_cleaning   = False
        
        ManagedAthenaTileMon.AthenaMonTools += [ ToolSvc.TileJetMonTool ]



if  tileRawMon:

    TileMBTSMon = CfgMgr.TileMBTSMonTool(name              = 'TileMBTSMon'
                                         , OutputLevel     = INFO
                                         , histoPathBase   = "/Tile/MBTS"
                                         , LVL1ConfigSvc   = "TrigConf::TrigConfigSvc/TrigConfigSvc"
                                         , doOnline        = athenaCommonFlags.isOnline()
                                         , readTrigger     = DQMonFlags.useTrigger());

    from AthenaCommon.GlobalFlags import globalflags
    if globalflags.InputFormat() == 'pool':
        TileMBTSMon.TileDigitsContainerName = 'TileDigitsFlt'

    ToolSvc += TileMBTSMon;
    ManagedAthenaTileMon.AthenaMonTools += [ TileMBTSMon ]

    from TileRecUtils.TileRecFlags import jobproperties
    if jobproperties.TileRecFlags.readDigits():

        ToolSvc += CfgMgr.TileRODMonTool( name              = 'TileRODMon'
                                          , OutputLevel     = INFO
                                          , histoPathBase   = "/Tile/ROD"
                                          , doOnline        = athenaCommonFlags.isOnline())

        ManagedAthenaTileMon.AthenaMonTools += [ ToolSvc.TileRODMon ]

        ToolSvc += CfgMgr.TileDigiNoiseMonTool(name                  = 'TileDigiNoiseMon'
                                               , OutputLevel         = INFO
                                               , TileDigitsContainer = "TileDigitsCnt"
                                               , histoPathBase       = "/Tile/DigiNoise" 
                                               , TriggerTypes        = [ 0x82 ]);

        if globalflags.InputFormat() == 'pool':
            ToolSvc.TileDigiNoiseMon.TileDigitsContainer = 'TileDigitsFlt'

        ManagedAthenaTileMon.AthenaMonTools += [ ToolSvc.TileDigiNoiseMon ]


    ToolSvc += CfgMgr.TileDQFragMonTool(name                            = 'TileDQFragMon'
                                        , OutputLevel                   = INFO
                                        , TileRawChannelContainerDSP    = "TileRawChannelCnt"
                                        , TileRawChannelContainerOffl   = jobproperties.TileRecFlags.TileRawChannelContainer()
                                        , TileDigitsContainer           = "TileDigitsCnt"
                                        , NegAmpHG                      = -200.
                                        , NegAmpLG                      = -15.
                                        , SkipMasked                    = True
                                        , SkipGapCells                  = True
                                        , doPlots                       = False
                                        , doOnline                      = athenaCommonFlags.isOnline()
                                        , CheckDCS                      = hasattr(ServiceMgr, "TileDCSSvc")
                                        , histoPathBase                 = "/Tile/DMUErrors")


    ManagedAthenaTileMon.AthenaMonTools += [ ToolSvc.TileDQFragMon ]

topSequence += ManagedAthenaTileMon;
print ManagedAthenaTileMon;
