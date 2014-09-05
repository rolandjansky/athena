#**************************************************************
#
# jopOptions file for Tile Monitoring in Athena 
#
# TileMon_standalone_jobOptions_NoiseMon.py
#
# C.Clement 2014-07-18 - This is a modified version of TileMon_standalone_jobOptions.py
#                        to include Digital noise monitoring (TileDigiNoiseMon)
#                        and CellNoise monitoring (TileCellNoiseMon)
#
#                        To be used to run on global runs with tags like data12_8TeV....
#                        For files like data12_tilecomm. use jobOptions_TileCalibRec_NoiseMon.py as top joboption
#
# How to use:
# - In the top jobptopn: TileRec_topOptions.py, replace TileMon_standalone_jobOptions.py 
#      with this present TileMon_standalone_jobOptions_NoiseMon.py
# - run as follows:
#   athena.py ../share/TileRec_topOptions.py -c 'EvtMax=100;RunNumber=201555; \ 
#           FileFilter="RAW"; RunFromLocal=True; \
#           InputDirectory="/afs/cern.ch/user/t/tilecomm/w0/rawdata/";
#           FileName="ALL";
#           doMonitoring=True;
#           doTileCells=True;
#           doTileDigiNoiseMon=True;
#           doTileCellNoiseMon=True'
#==============================================================

MonitorOutput='Tile'


rec.projectName="data12_tilecomm"

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "ManagedAthenaTileMon" )
ManagedAthenaTileMon = topSequence.ManagedAthenaTileMon

from AthenaCommon.AppMgr import ServiceMgr

ManagedAthenaTileMon.FileKey = "Tile"
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
ManagedAthenaTileMon.LumiBlock           = 0


from TileMonitoring.TileMonitoringConf import *

#from TileMonitoring.TileMonitoringConf import TileDQFragMonTool
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

TileDQFragMon.OutputLevel=WARNING

ToolSvc += TileDQFragMon;
ManagedAthenaTileMon.AthenaMonTools += [ TileDQFragMon ];

#from TileMonitoring.TileMonitoringConf import TileRODMonTool
TileRODMon = TileRODMonTool( name             = 'TileRODMon',
                             histoStreamName  = "/SHIFT",
                             OutputLevel      = WARNING,
                             histoPathBase    = "/Tile/ROD",
                             doOnline         =  athenaCommonFlags.isOnline());
ToolSvc += TileRODMon;
ManagedAthenaTileMon.AthenaMonTools += [ TileRODMon ];


doTileL2Mu=False

if doTileL2Mu:

    #from TileMonitoring.TileMonitoringConf import TileL2MonTool
    TileL2MuMon = TileL2MonTool(name                = 'TileL2MuMon',
                                histoStreamName     = "/SHIFT",
                                OutputLevel         = 3, 
                                histoPathBase       = "/Tile/L2Muon");
    ToolSvc += TileL2MuMon;
    ManagedAthenaTileMon.AthenaMonTools += [ TileL2MuMon ];

if doTileDigiNoiseMon:
    doTileCells=False
    doTowers=False
    doTileL2Mu=False
    doClusters=False
    doTileMuId=False
    doTileMuonFit=False
    doMBTS=False
    doTrigger=False

    #from TileMonitoring.TileMonitoringConf import TileDigiNoiseMonTool
    TileDigiNoiseMon = TileDigiNoiseMonTool(name               = 'TileDigiNoiseMon',
                                            histoStreamName    = "/Tile",
                                            OutputLevel        = WARNING, 
                                            runType            = 4, # pedestal maybe always should be...
                                            bigain             = True,
                                            TileRawChannelContainerDSP = "TileRawChannelCnt",
                                            histoPathBase      = "/Tile/DigiNoise");
    
    TileDigiNoiseMon.OutputLevel=WARNING
    ToolSvc += TileDigiNoiseMon;
    ManagedAthenaTileMon.AthenaMonTools += [ TileDigiNoiseMon ];

    print " "
    print "           TileDigiNoiseMon Information: "
    print TileDigiNoiseMon;
    print " "


if doTileCellNoiseMon:
    doTileCells=False
    doTowers=False
    doTileL2Mu=False
    doClusters=False
    doTileMuId=False
    doTileMuonFit=False
    doMBTS=False
    doTrigger=False

    #from TileMonitoring.TileMonitoringConf import TileCellNoiseMonTool
    TileCellNoiseMon = TileCellNoiseMonTool(name               = 'TileCellNoiseMon',
                                            histoStreamName    = "/Tile",
                                            OutputLevel        = DEBUG, 
                                            doOnline           = athenaCommonFlags.isOnline(),
                                            cellsContainerName = "AllCalo",
                                            histoPathBase      = "/Tile/CellNoise");


    TileCellNoiseMon.histoPathBase = "/Tile/CellNoise";
 
    ToolSvc += TileCellNoiseMon;
    ManagedAthenaTileMon.AthenaMonTools += [ TileCellNoiseMon ];
    
    TileCellNoiseMon.FileKey = "Tile"

    print " "
    print "           TileCellNoiseMon Information: "
    print TileCellNoiseMon;
    print " "

if doTileCells:
    
    #from TileMonitoring.TileMonitoringConf import TileCellMonTool
    TileCellMon = TileCellMonTool(name               = 'TileCellMon',
                                  histoStreamName    = "/SHIFT",
                                  OutputLevel        = 3, 
                                  doOnline           = athenaCommonFlags.isOnline(),
                                  cellsContainerName = "AllCalo",
                                  histoPathBase      = "/Tile/Cell");

    ToolSvc += TileCellMon;
    ManagedAthenaTileMon.AthenaMonTools += [ TileCellMon ];

    if doTowers:
        #from TileMonitoring.TileMonitoringConf import TileTowerMonTool
        TileTowerMon = TileTowerMonTool(name                = 'TileTowerMon',
                                        histoStreamName     = "/SHIFT",
                                        OutputLevel         = 3, 
                                        towersContainerName = "TileTower",
                                        histoPathBase       = "/Tile/Tower");
        ToolSvc += TileTowerMon;
        ManagedAthenaTileMon.AthenaMonTools += [ TileTowerMon ];

    if doClusters:
        #from TileMonitoring.TileMonitoringConf import TileClusterMonTool
        TileClusterMon = TileClusterMonTool(name              = 'TileClusterMon',
                                            histoStreamName       = "/SHIFT",
                                            OutputLevel           = 3,
                                            clustersContainerName = "TileTopoCluster",
                                            histoPathBase       = "/Tile/Cluster");
        ToolSvc += TileClusterMon;
        ManagedAthenaTileMon.AthenaMonTools += [ TileClusterMon ];

    if doTileMuId:
        #from TileMonitoring.TileMonitoringConf import TileMuIdMonTool
        TileMuIdMon = TileMuIdMonTool(name                = 'TileMuIdMon',
                                      histoStreamName     = "/SHIFT",
                                      OutputLevel         = 3, 
                                      histoPathBase       = "/Tile/Muid");
        ToolSvc += TileMuIdMon;
        ManagedAthenaTileMon.AthenaMonTools += [ TileMuIdMon ];

    if doTileMuonFit:
        #from TileMonitoring.TileMonitoringConf import TileMuonFitMonTool
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
        ManagedAthenaMBTSMon.LumiBlock           = 0

        #from TileMonitoring.TileMonitoringConf import TileMBTSMonTool
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
    #tTHistSvc.Output = [MonitorOutput+" DATAFILE='"+RootHistOutputFileName+"' OPT='RECREATE'"]

    tTHistSvc.Output = ["Tile DATAFILE='"+RootHistOutputFileName+"' OPT='RECREATE'"]

    #tTHistSvc.Output += ["EXPERT DATAFILE='"+RootHistOutputFileName+"' OPT='RECREATE'"]
    #tTHistSvc.Output += ["EXPERT DATAFILE='pippo.root' OPT='RECREATE'"]
    #tTHistSvc.Output += ["RUNSTAT DATAFILE='"+RootHistOutputFileName+"' OPT='RECREATE'"]
    THistSvc.OutputLevel = DEBUG
else:
    from TrigServices.TrigServicesConf import TrigMonTHistSvc
    trigmonTHistSvc = TrigMonTHistSvc("THistSvc")
    svcMgr += trigmonTHistSvc
    #include ("TrigServices/TrigMonTHistSvc.py")
    #THistSvc = Service("THistSvc")
    #trigmonTHistSvc.OutputLevel = VERBOSE
