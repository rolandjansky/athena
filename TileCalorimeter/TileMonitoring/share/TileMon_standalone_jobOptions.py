#**************************************************************
#
# jopOptions file for Tile Monitoring in Athena 
#
#==============================================================

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "ManagedAthenaTileMon" )
ManagedAthenaTileMon = topSequence.ManagedAthenaTileMon

from AthenaCommon.AppMgr import ServiceMgr

ManagedAthenaTileMon.FileKey = "SHIFT"
ManagedAthenaTileMon.ManualDataTypeSetup = True
ManagedAthenaTileMon.DataType            = "cosmics"
if athenaCommonFlags.isOnline():
    ManagedAthenaTileMon.Environment         = "online"
else:
    ManagedAthenaTileMon.Environment         = "online"
    #ManagedAthenaTileMon.Environment         = "tier0"

#ManagedAthenaTileMon.ManualRunLBSetup    = False
ManagedAthenaTileMon.ManualRunLBSetup    = True
ManagedAthenaTileMon.Run                 = RunNumber
ManagedAthenaTileMon.LumiBlock           = 1


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
                                  doOnline           = athenaCommonFlags.isOnline(),
                                  doPlots            = False,   
                                  CheckDCS           = hasattr(ServiceMgr, "TileDCSSvc"),
                                  histoPathBase      = "Tile/DMUErrors");

ToolSvc += TileDQFragMon;
ManagedAthenaTileMon.AthenaMonTools += [ TileDQFragMon ];

from TileMonitoring.TileMonitoringConf import TileRODMonTool
TileRODMon = TileRODMonTool( name             = 'TileRODMon',
                             histoStreamName  = "/SHIFT",
                             OutputLevel      = 3,
                             histoPathBase    = "/Tile/ROD",
                             doOnline         =  athenaCommonFlags.isOnline());
ToolSvc += TileRODMon;
ManagedAthenaTileMon.AthenaMonTools += [ TileRODMon ];


if doTileL2Mu:

    from TileMonitoring.TileMonitoringConf import TileL2MonTool
    TileL2MuMon = TileL2MonTool(name                = 'TileL2MuMon',
                                histoStreamName     = "/SHIFT",
                                OutputLevel         = 3, 
                                histoPathBase       = "/Tile/L2Muon");
    ToolSvc += TileL2MuMon;
    ManagedAthenaTileMon.AthenaMonTools += [ TileL2MuMon ];


if doTileCells:
    
    from TileMonitoring.TileMonitoringConf import TileCellMonTool
    TileCellMon = TileCellMonTool(name               = 'TileCellMon',
                                  histoStreamName    = "/SHIFT",
                                  OutputLevel        = 3, 
                                  doOnline           = athenaCommonFlags.isOnline(),
                                  cellsContainerName = "AllCalo",
                                  histoPathBase      = "/Tile/Cell");

    ToolSvc += TileCellMon;
    ManagedAthenaTileMon.AthenaMonTools += [ TileCellMon ];

    if doTowers:
        from TileMonitoring.TileMonitoringConf import TileTowerMonTool
        TileTowerMon = TileTowerMonTool(name                = 'TileTowerMon',
                                        histoStreamName     = "/SHIFT",
                                        OutputLevel         = 3, 
                                        towersContainerName = "TileTower",
                                        histoPathBase       = "/Tile/Tower");
        ToolSvc += TileTowerMon;
        ManagedAthenaTileMon.AthenaMonTools += [ TileTowerMon ];

    if doClusters:
        from TileMonitoring.TileMonitoringConf import TileClusterMonTool
        TileClusterMon = TileClusterMonTool(name              = 'TileClusterMon',
                                            histoStreamName       = "/SHIFT",
                                            OutputLevel           = 3,
                                            clustersContainerName = "TileTopoCluster",
                                            histoPathBase       = "/Tile/Cluster");
        ToolSvc += TileClusterMon;
        ManagedAthenaTileMon.AthenaMonTools += [ TileClusterMon ];

    if doTileMuId:
        from TileMonitoring.TileMonitoringConf import TileMuIdMonTool
        TileMuIdMon = TileMuIdMonTool(name                = 'TileMuIdMon',
                                      histoStreamName     = "/SHIFT",
                                      OutputLevel         = 3, 
                                      histoPathBase       = "/Tile/Muid");
        ToolSvc += TileMuIdMon;
        ManagedAthenaTileMon.AthenaMonTools += [ TileMuIdMon ];

    if doTileMuonFit:
        from TileMonitoring.TileMonitoringConf import TileMuonFitMonTool
        TileMuonFitMon = TileMuonFitMonTool(name                = 'TileMuonFitMon',
                                            histoStreamName     = "/SHIFT",
                                            OutputLevel         = 3, 
                                            histoPathBase       = "/Tile/MuonFit");
        ToolSvc += TileMuonFitMon;
        ManagedAthenaTileMon.AthenaMonTools += [ TileMuonFitMon ];


    # MBTS monitoring
    if doMBTS:
        topSequence += AthenaMonManager( "ManagedAthenaMBTSMon" )
        ManagedAthenaMBTSMon = topSequence.ManagedAthenaMBTSMon

        ManagedAthenaMBTSMon.FileKey = "SHIFT"
        ManagedAthenaMBTSMon.ManualDataTypeSetup = True
        ManagedAthenaMBTSMon.DataType            = "cosmics"
        if athenaCommonFlags.isOnline():
            ManagedAthenaMBTSMon.Environment         = "online"
        else:
            ManagedAthenaMBTSMon.Environment         = "online"
            #ManagedAthenaTileMon.Environment         = "tier0"

        #ManagedAthenaTileMon.ManualRunLBSetup    = False
        ManagedAthenaMBTSMon.ManualRunLBSetup    = True
        ManagedAthenaMBTSMon.Run                 = RunNumber
        ManagedAthenaMBTSMon.LumiBlock           = 1

        from TileMonitoring.TileMonitoringConf import TileMBTSMonTool
        TileMBTSMon = TileMBTSMonTool(  name            = 'TileMBTSMon',
                                        histoStreamName = "/SHIFT",
                                        OutputLevel     = 3,
                                        histoPathBase   = "/Tile/MBTS",
                                        LVL1ConfigSvc   = "TrigConf::TrigConfigSvc/TrigConfigSvc",
                                        doOnline        = athenaCommonFlags.isOnline(),
                                        readTrigger     = doTrigger);

        ToolSvc += TileMBTSMon;
        ManagedAthenaMBTSMon.AthenaMonTools += [ TileMBTSMon ];

        print ManagedAthenaMBTSMon;

print ManagedAthenaTileMon;


# -- use root histos --
# THistService for native root in Athena
if not  athenaCommonFlags.isOnline() or storeHisto or athenaCommonFlags.isOnlineStateless():
    from GaudiSvc.GaudiSvcConf import THistSvc
    svcMgr += THistSvc("THistSvc")
    tTHistSvc = svcMgr.THistSvc
    tTHistSvc.Output = [MonitorOutput+" DATAFILE='"+RootHistOutputFileName+"' OPT='RECREATE'"]
    #tTHistSvc.Output += ["EXPERT DATAFILE='"+RootHistOutputFileName+"' OPT='RECREATE'"]
    #tTHistSvc.Output += ["EXPERT DATAFILE='pippo.root' OPT='RECREATE'"]
    #tTHistSvc.Output += ["RUNSTAT DATAFILE='"+RootHistOutputFileName+"' OPT='RECREATE'"]
    #THistSvc.OutputLevel = DEBUG
else:
    from TrigServices.TrigServicesConf import TrigMonTHistSvc
    trigmonTHistSvc = TrigMonTHistSvc("THistSvc")
    svcMgr += trigmonTHistSvc
    #include ("TrigServices/TrigMonTHistSvc.py")
    #THistSvc = Service("THistSvc")
    #trigmonTHistSvc.OutputLevel = VERBOSE
