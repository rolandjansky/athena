#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

if not 'doTileMonDQ' in dir():
    doTileMonDQ = True

if not 'doTileDigiNoiseMon' in dir():
    doTileDigiNoiseMon = True

if not 'doTileRawChannelNoiseMon' in dir():
    doTileRawChannelNoiseMon = True


if not 'TileNoiseUpdateFrequency' in dir():
    if athenaCommonFlags.isOnline():
        TileNoiseUpdateFrequency = 1000
    else:
        TileNoiseUpdateFrequency = 0

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon import CfgMgr

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from TileRecUtils.TileDQstatusAlgDefault import TileDQstatusAlgDefault
TileDQstatusAlgDefault()

topSequence += CfgMgr.AthenaMonManager( "TileNoiseMon"
                                       , ManualRunLBSetup    = True
                                       , ManualDataTypeSetup = True
                                       , Environment         = "online"
                                       , FileKey             = MonitorOutput
                                       , Run                 = RunNumber
                                       , LumiBlock           = 1 )

if doTileMonDQ:
    TileDQFragMon = CfgMgr.TileDQFragMonTool(name               = 'TileDQFragMon'
                                             , OutputLevel        = WARNING
                                             , TileRawChannelContainerDSP    = "TileRawChannelCnt"
                                             , TileRawChannelContainerOffl   = jobproperties.TileRecFlags.TileRawChannelContainer()
                                             , TileDigitsContainer           = "TileDigitsCnt"
                                             , NegAmpHG           = -200.
                                             , NegAmpLG           = -15.
                                             , SkipMasked         = True
                                             , SkipGapCells       = True
                                             , doOnline           = athenaCommonFlags.isOnline()
                                             , doPlots            = False
                                             , CheckDCS           = TileUseDCS
                                             , histoPathBase      = "/Tile/DMUErrors");




    topSequence.TileNoiseMon.AthenaMonTools += [ TileDQFragMon ];
    print TileDQFragMon


if doTileDigiNoiseMon:
    TileDigiNoiseMon = CfgMgr.TileDigiNoiseMonTool(name               = 'TileDigiNoiseMon'
                                                   , OutputLevel        = WARNING
                                                   , TileDigitsContainer = "TileDigitsCnt"
                                                   , histoPathBase = "/Tile/DigiNoise"
                                                   , FillEmptyFromDB = False
                                                   , SummaryUpdateFrequency = TileNoiseUpdateFrequency
                                                   , CheckDCS           = TileUseDCS
                                                   , TriggerTypes        = [ 0x82 ] )

    topSequence.TileNoiseMon.AthenaMonTools += [ TileDigiNoiseMon ];
    print TileDigiNoiseMon;
    
if doTileRawChannelNoiseMon:
    TileRawChannelNoiseMon = CfgMgr.TileRawChannelNoiseMonTool(name               = 'TileRawChannelNoiseMon'
                                                               , OutputLevel        = WARNING
                                                               , TileRawChannelContainer = jobproperties.TileRecFlags.TileRawChannelContainer()
                                                               , doFit = True
                                                               , do2GFit = False
                                                               , Gain = "HG"
                                                               , Xmin = -20.25
                                                               , Xmax = 20.25
                                                               , NBins = 81
                                                               , histoPathBase = "/Tile/RawChannelNoise"
                                                               , SummaryUpdateFrequency = TileNoiseUpdateFrequency 
                                                               , CheckDCS           = TileUseDCS
                                                               , TriggerTypes        = [ 0x82 ]);

    topSequence.TileNoiseMon.AthenaMonTools += [ TileRawChannelNoiseMon ];
    print TileRawChannelNoiseMon;

print topSequence.TileNoiseMon

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
    svcMgr.THistSvc.Output = [MonitorOutput+" DATAFILE='"+RootHistOutputFileName+"' OPT='RECREATE'"]

else:
    from TrigServices.TrigServicesConf import TrigMonTHistSvc
    trigmonTHistSvc = TrigMonTHistSvc("THistSvc")
    svcMgr += trigmonTHistSvc
