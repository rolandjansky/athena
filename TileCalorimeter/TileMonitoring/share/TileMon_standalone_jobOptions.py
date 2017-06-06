#**************************************************************
#
# jopOptions file for Tile Monitoring in Athena 
#
#==============================================================



from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from AthenaCommon import CfgMgr
toolSvc = CfgMgr.ToolSvc()

topSequence += CfgMgr.AthenaMonManager(name                  = 'ManagedAthenaTileMon'
                                       , FileKey             = 'SHIFT'
                                       , ManualDataTypeSetup = True
                                       , DataType            = 'cosmics'
                                       , Environment         = 'online'
                                       , ManualRunLBSetup    = True
                                       , Run                 = RunNumber
                                       , LumiBlock           = 1)

ManagedAthenaTileMon = topSequence.ManagedAthenaTileMon


toolSvc += CfgMgr.TileDQFragMonTool(name                          = 'TileDQFragMon'
                                    , OutputLevel                 = INFO
                                    , TileRawChannelContainerDSP  = "TileRawChannelCnt"
                                    , TileRawChannelContainerOffl = jobproperties.TileRecFlags.TileRawChannelContainer()
                                    , TileDigitsContainer         = "TileDigitsCnt"
                                    , NegAmpHG                    = -200.
                                    , NegAmpLG                    = -15.
                                    , SkipMasked                  = True
                                    , SkipGapCells                = True
                                    , doOnline                    = athenaCommonFlags.isOnline()
                                    , doPlots                     = False
                                    , CheckDCS                    = hasattr(svcMgr, "TileDCSSvc")
                                    , histoPathBase               = "Tile/DMUErrors")

ManagedAthenaTileMon.AthenaMonTools += [ toolSvc.TileDQFragMon ]

toolSvc += CfgMgr.TileRODMonTool( name             = 'TileRODMon'
                                  , OutputLevel    = INFO
                                  , histoPathBase  = "/Tile/ROD"
                                  , NumberOfEventsToAverageFragmentSize = 50
                                  , doOnline       =  athenaCommonFlags.isOnline())

ManagedAthenaTileMon.AthenaMonTools += [ toolSvc.TileRODMon ];


if doTileTMDBDigitsMon:
    toolSvc += CfgMgr.TileTMDBDigitsMonTool(name                  = 'TileTMDBDigitsMon'
                                            , OutputLevel         = INFO
                                            , TileDigitsContainer = "MuRcvDigitsCnt"
                                            , histoPathBase       = "/Tile/TMDBDigits")
 
    if (athenaCommonFlags.isOnline()):
        toolSvc.TileTMDBDigitsMon.SummaryUpdateFrequency = 1000

    ManagedAthenaTileMon.AthenaMonTools += [ toolSvc.TileTMDBDigitsMon ]
    print toolSvc.TileTMDBDigitsMon

if doTileTMDBRawChannelMon:
    toolSvc += CfgMgr.TileTMDBRawChannelMonTool(name		= 'TileTMDBRawChannelDspMon'
						, OutputLevel	= INFO
                                                , NotDSP           = False
						, TileRawChannelContainer = "MuRcvRawChCnt"
						, histoPathBase = "/Tile/TMDBRawChannel/Dsp")


    ManagedAthenaTileMon.AthenaMonTools += [toolSvc.TileTMDBRawChannelDspMon ]
    print toolSvc.TileTMDBRawChannelDspMon

    toolSvc += CfgMgr.TileTMDBRawChannelMonTool(name		          = 'TileTMDBRawChannelMon'
						, OutputLevel	          = INFO
						, TileRawChannelContainer = "TileMuRcvRawChannelOpt2"
                                                , NotDSP                   = True
                                                , AmplitudeThresholdForTime = 80.0
						, histoPathBase           = "/Tile/TMDBRawChannel")


    ManagedAthenaTileMon.AthenaMonTools += [toolSvc.TileTMDBRawChannelMon ]
    print toolSvc.TileTMDBRawChannelMon

if doTileL2Mu:

    toolSvc += CfgMgr.TileL2MonTool(name                = 'TileL2MuMon'
                                    , OutputLevel         = INFO
                                    , histoPathBase       = "/Tile/L2Muon")

    ManagedAthenaTileMon.AthenaMonTools += [ toolSvc.TileL2MuMon ]


if doTileCells:
    
    if TileBiGainRun:
        toolSvc += CfgMgr.TileCellMonTool(name                 = 'TileCellMonHG'
                                          , OutputLevel        = INFO
                                          , doOnline           = athenaCommonFlags.isOnline()
                                          , cellsContainerName = "AllCaloHG"
                                          , histoPathBase      = "/Tile/Cell/HG"
                                          , NumberOfLastLumiblocks4MaskedChannelsOnFly = 7
                                          , SkipNotPhysicsEvents = TilePhysRun)
        
        ManagedAthenaTileMon.AthenaMonTools += [ toolSvc.TileCellMonHG ]

        toolSvc += CfgMgr.TileCellMonTool(name                 = 'TileCellMonLG'
                                          , OutputLevel        = INFO
                                          , doOnline           = athenaCommonFlags.isOnline()
                                          , cellsContainerName = "AllCaloLG"
                                          , histoPathBase      = "/Tile/Cell/LG"
                                          , NumberOfLastLumiblocks4MaskedChannelsOnFly = 0
                                          , SkipNotPhysicsEvents = TilePhysRun)
        
        ManagedAthenaTileMon.AthenaMonTools += [ toolSvc.TileCellMonLG ]

    else:
        toolSvc += CfgMgr.TileCellMonTool(name                 = 'TileCellMon'
                                          , OutputLevel        = INFO
                                          , doOnline           = athenaCommonFlags.isOnline()
                                          , cellsContainerName = "AllCalo"
                                          , histoPathBase      = "/Tile/Cell"
                                          , NumberOfLastLumiblocks4MaskedChannelsOnFly = 7
                                          , SkipNotPhysicsEvents = TilePhysRun)
        
        if (jobproperties.Beam.beamType() == 'singlebeam'):
            toolSvc.TileCellMon.FillTimeHistograms = True
            toolSvc.TileCellMon.energyThresholdForTime = 150.0
            
        ManagedAthenaTileMon.AthenaMonTools += [ toolSvc.TileCellMon ]

    if doTowers:

        toolSvc += CfgMgr.TileTowerMonTool(name                  = 'TileTowerMon'
                                           , OutputLevel         = INFO
                                           , towersContainerName = "TileTower"
                                           , histoPathBase       = "/Tile/Tower")

        ManagedAthenaTileMon.AthenaMonTools += [ toolSvc.TileTowerMon ]

    if doClusters:

        toolSvc += CfgMgr.TileClusterMonTool(name                    = 'TileClusterMon'
                                             , OutputLevel           = INFO
                                             , clustersContainerName = "TileTopoCluster"
                                             , FillTimingHistograms = True
                                             , doOnline           = athenaCommonFlags.isOnline()
                                             , histoPathBase         = "/Tile/Cluster" )


        ManagedAthenaTileMon.AthenaMonTools += [ toolSvc.TileClusterMon ]

    if doTileMuId:

        toolSvc += CfgMgr.TileMuIdMonTool(name                  = 'TileMuIdMon'
                                          , OutputLevel         = INFO
                                          , histoPathBase       = "/Tile/Muid")

        ManagedAthenaTileMon.AthenaMonTools += [ toolSvc.TileMuIdMon ]

    if doTileMuonFit:

        toolSvc += CfgMgr.TileMuonFitMonTool(name                  = 'TileMuonFitMon'
                                             , OutputLevel         = INFO
                                             , UseLVL1             = False
                                             , histoPathBase       = "/Tile/MuonFit")

        ManagedAthenaTileMon.AthenaMonTools += [ toolSvc.TileMuonFitMon ]


    # MBTS monitoring
    if doMBTS:
        topSequence += CfgMgr.AthenaMonManager(name                  = 'ManagedAthenaMBTSMon'
                                               , FileKey             = 'SHIFT'
                                               , ManualDataTypeSetup = True
                                               , DataType            = 'cosmics'
                                               , Environment         = 'online'
                                               , ManualRunLBSetup    = True
                                               , Run                 = RunNumber
                                               , LumiBlock           = 1)

        ManagedAthenaMBTSMon = topSequence.ManagedAthenaMBTSMon


        from TileMonitoring.TileMonitoringConf import TileMBTSMonTool
        toolSvc += CfgMgr.TileMBTSMonTool( name            = 'TileMBTSMon'
                                           , OutputLevel     = INFO
                                           , histoPathBase   = "/Tile/MBTS"
                                           , LVL1ConfigSvc   = "TrigConf::TrigConfigSvc/TrigConfigSvc"
                                           , doOnline        = athenaCommonFlags.isOnline()
                                           , readTrigger     = doTrigger)

        ManagedAthenaMBTSMon.AthenaMonTools += [ toolSvc.TileMBTSMon ]

        print ManagedAthenaMBTSMon

print ManagedAthenaTileMon


import os
# -- use root histos --
# THistService for native root in Athena
if not  athenaCommonFlags.isOnline() or storeHisto or athenaCommonFlags.isOnlineStateless():
    #theApp.HistogramPersistency = "ROOT"
    if not hasattr(svcMgr,"THistSvc"):
        from GaudiSvc.GaudiSvcConf import THistSvc
        svcMgr += THistSvc("THistSvc")
    if os.path.exists(RootHistOutputFileName):
        os.remove(RootHistOutputFileName)
    svcMgr.THistSvc.Output = [MonitorOutput + " DATAFILE='" + RootHistOutputFileName + "' OPT='RECREATE'"]
else:
    from TrigServices.TrigServicesConf import TrigMonTHistSvc
    trigmonTHistSvc = TrigMonTHistSvc("THistSvc")
    svcMgr += trigmonTHistSvc
