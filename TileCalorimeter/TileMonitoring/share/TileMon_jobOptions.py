#**************************************************************
#
# jopOptions file for Tile Monitoring in Athena 
#
#==============================================================

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags 

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

    from TileMonitoring.TileMonitoringConf import TileCellMonTool
    TileCellMon = TileCellMonTool(name               = 'TileCellMon',
                                  histoStreamName    = "/SHIFT",
                                  OutputLevel        = 3, 
                                  doOnline           = athenaCommonFlags.isOnline(),
                                  cellsContainerName = "AllCalo",
                                  histoPathBase      = "/Tile/Cell");

    ToolSvc += TileCellMon;    
    ManagedAthenaTileMon.AthenaMonTools += [ TileCellMon ];


    include ("TileMonitoring/TileMonTower_jobOptions.py")
    from TileMonitoring.TileMonitoringConf import TileTowerMonTool
    TileTowerMon = TileTowerMonTool(name                = 'TileTowerMon',
                                    histoStreamName     = "/SHIFT",
                                    OutputLevel         = 3, 
                                    towersContainerName = "TileTower",
                                    histoPathBase       = "/Tile/Tower");
    ToolSvc += TileTowerMon;
    ManagedAthenaTileMon.AthenaMonTools += [ TileTowerMon ];


    include ("TileMonitoring/TileMonTopoCluster_jobOptions.py")
    from TileMonitoring.TileMonitoringConf import TileClusterMonTool
    TileClusterMon = TileClusterMonTool(name                  = 'TileClusterMon',
                                        histoStreamName       = "/SHIFT",
                                        OutputLevel           = 3,
                                        clustersContainerName = "TileTopoCluster",
                                        histoPathBase         = "/Tile/Cluster");
    ToolSvc += TileClusterMon;
    ManagedAthenaTileMon.AthenaMonTools += [ TileClusterMon ];
    
    if (jobproperties.Beam.beamType() == 'cosmics' or jobproperties.Beam.beamType() == 'singlebeam') and DQMonFlags.useTrigger() :

        from TileMonitoring.TileMonitoringConf import TileMuonFitMonTool
        TileMuonFitMon = TileMuonFitMonTool(name                = 'TileMuonFitMon',
                                            histoStreamName     = "/SHIFT",
                                            OutputLevel         = 3, 
                                            histoPathBase       = "/Tile/MuonFit");
        ToolSvc += TileMuonFitMon;
        ManagedAthenaTileMon.AthenaMonTools += [ TileMuonFitMon ];


    from TileMonitoring.TileMonitoringConf import TileMuIdMonTool
    TileMuIdMon = TileMuIdMonTool(name                = 'TileMuIdMon',
                                  histoStreamName     = "/SHIFT",
                                  OutputLevel         = 3, 
                                  histoPathBase       = "/Tile/Muid");
    ToolSvc += TileMuIdMon;
    ManagedAthenaTileMon.AthenaMonTools += [ TileMuIdMon ];


    from TileMonitoring.TileMonitoringConf import TileL2MonTool
    TileL2MuMon = TileL2MonTool(name                = 'TileL2MuMon',
                                histoStreamName     = "/SHIFT",
                                OutputLevel         = 3, 
                                histoPathBase       = "/Tile/L2Muon");
    ToolSvc += TileL2MuMon;
    ManagedAthenaTileMon.AthenaMonTools += [ TileL2MuMon ];


if  tileRawMon:

    from TileMonitoring.TileMonitoringConf import TileMBTSMonTool
    TileMBTSMon = TileMBTSMonTool(  name            = 'TileMBTSMon',
                                    histoStreamName = "/SHIFT",
                                    OutputLevel     = 3,
                                    histoPathBase   = "/Tile/MBTS",
                                    LVL1ConfigSvc   = "TrigConf::TrigConfigSvc/TrigConfigSvc",
                                    doOnline        = athenaCommonFlags.isOnline(),
                                    readTrigger     = DQMonFlags.useTrigger());

    ToolSvc += TileMBTSMon;
    ManagedAthenaTileMon.AthenaMonTools += [ TileMBTSMon ];

    from TileRecUtils.TileRecFlags import jobproperties
    if jobproperties.TileRecFlags.readDigits():

        from TileMonitoring.TileMonitoringConf import TileRODMonTool
        TileRODMon = TileRODMonTool( name            = 'TileRODMon',
                                     histoStreamName = "/SHIFT",
                                     OutputLevel     = 3,
                                     histoPathBase   = "/Tile/ROD",
                                     doOnline        = athenaCommonFlags.isOnline());

        ToolSvc += TileRODMon;
        ManagedAthenaTileMon.AthenaMonTools += [ TileRODMon ];

    from TileMonitoring.TileMonitoringConf import TileDQFragMonTool
    TileDQFragMon = TileDQFragMonTool(name               = 'TileDQFragMon',
                                      histoStreamName    = "/SHIFT",
                                      OutputLevel        = 3, 
                                      TileRawChannelContainerDSP    = "TileRawChannelCnt",
                                      TileRawChannelContainerOffl   = jobproperties.TileRecFlags.TileRawChannelContainer(),
                                      TileDigitsContainer           = "TileDigitsCnt",
                                      NegAmpHG           = -200.,
                                      NegAmpLG           = -15.,
                                      SkipMasked         = True,
                                      SkipGapCells       = True,
                                      doPlots            = False,
                                      doOnline           = athenaCommonFlags.isOnline(),
                                      CheckDCS           = hasattr(ServiceMgr, "TileDCSSvc"),
                                      histoPathBase      = "/Tile/DMUErrors");

    ToolSvc += TileDQFragMon;
    ManagedAthenaTileMon.AthenaMonTools += [ TileDQFragMon ];

topSequence += ManagedAthenaTileMon;
print ManagedAthenaTileMon;
