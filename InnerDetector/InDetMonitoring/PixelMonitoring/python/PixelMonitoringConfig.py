#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

def PixelMonitoringConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    forceOnline = False # for testing of online monitoring and 100LB histograms
    from InDetRecExample.InDetKeys          import InDetKeys
    from InDetRecExample.InDetJobProperties import InDetFlags
    # run on RAW only
    if flags.DQ.Environment in ('online', 'tier0', 'tier0Raw'):
        if forceOnline: 
            isOnline = True
        else:
            isOnline = flags.Common.isOnline
        kwargsHitMonAlg = { 'doOnline'        : isOnline,      #Histograms for online (GlobalMonitoring) running
                            'doLumiBlock'     : not isOnline,  #Turn on/off histograms stored every 1(20) lumi block(s)
                            'doFEPlots'       : True,                       #Turn on/off per FE-I3 histograms
                            'RDOName'         : InDetKeys.PixelRDOs()       #'PixelRDOs'
        }

        kwargsClusMonAlg = { 'doOnline'        : isOnline,      #Histograms for online (GlobalMonitoring) running
                             'doLumiBlock'     : not isOnline,  #Turn on/off histograms stored every 1(20) lumi block(s)
                             'doLowOccupancy'  : InDetFlags.doCosmics(), #Setting up 1D histogram ranges and binnings, if False, high occupancy i.e. collisions settings will be used
                             'doHeavyIonMon'   : InDetFlags.doHeavyIon(),     #Setting up 1D histogram ranges and binnings for heavy ions
                             'doFEPlots'       : True,                       #Turn on/off per FE-I3 histograms
                             'ClusterName'     : InDetKeys.PixelClusters(),  #'PixelClusters'
                             'TrackName'       : InDetKeys.Tracks()          #'Tracks'
        }

        kwargsErrMonAlg = { 'doOnline'        : isOnline,        #Histograms for online (GlobalMonitoring) running
                            'doLumiBlock'     : not isOnline     #Turn on/off histograms stored every 1(20) lumi block(s)
        }
        from AthenaMonitoring import AthMonitorCfgHelper
        helper = AthMonitorCfgHelper(flags, "NewPixelMonitoring")

        from AthenaConfiguration.ComponentFactory import CompFactory
        from PixelMonitoring.PixelAthHitMonAlgCfg import PixelAthHitMonAlgCfg        
        pixelAthHitMonAlg = helper.addAlgorithm(CompFactory.PixelAthHitMonAlg, 'PixelAthHitMonAlg')
        for k, v in kwargsHitMonAlg.items():
            setattr(pixelAthHitMonAlg, k, v)
        PixelAthHitMonAlgCfg(helper, pixelAthHitMonAlg, **kwargsHitMonAlg)

        from PixelMonitoring.PixelAthClusterMonAlgCfg import PixelAthClusterMonAlgCfg 
        pixelAthClusterMonAlg = helper.addAlgorithm(CompFactory.PixelAthClusterMonAlg, 'PixelAthClusterMonAlg')
        for k, v in kwargsClusMonAlg.items():
            setattr(pixelAthClusterMonAlg, k, v)
        pixelAthClusterMonAlg.TrackSelectionTool.UseTrkTrackTools = True
        pixelAthClusterMonAlg.TrackSelectionTool.CutLevel         = "TightPrimary"
        pixelAthClusterMonAlg.TrackSelectionTool.maxNPixelHoles   = 1
        pixelAthClusterMonAlg.TrackSelectionTool.maxD0            = 2
        pixelAthClusterMonAlg.TrackSelectionTool.maxZ0            = 150

        from InDetConfig.TrackingCommonConfig import InDetTrackSummaryToolCfg
        pixelAthClusterMonAlg.TrackSelectionTool.TrackSummaryTool = acc.getPrimaryAndMerge(InDetTrackSummaryToolCfg(flags))
        pixelAthClusterMonAlg.TrackSelectionTool.Extrapolator     = acc.getPublicTool("InDetExtrapolator")
        
        
        PixelAthClusterMonAlgCfg(helper, pixelAthClusterMonAlg, **kwargsClusMonAlg)

        from PixelMonitoring.PixelAthErrorMonAlgCfg import PixelAthErrorMonAlgCfg
        pixelAthMonAlgErrorMonAlg = helper.addAlgorithm(CompFactory.PixelAthErrorMonAlg, 'PixelAthErrorMonAlg')
        for k, v in kwargsErrMonAlg.items():
            setattr(pixelAthMonAlgErrorMonAlg, k, v)
        PixelAthErrorMonAlgCfg(helper, pixelAthMonAlgErrorMonAlg, **kwargsErrMonAlg)
        acc.merge(helper.result())

    return acc
    
