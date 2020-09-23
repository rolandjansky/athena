#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def PixelMonitoringConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()

    # run on RAW only
    if flags.DQ.Environment in ('online', 'tier0', 'tier0Raw'):
        kwargsHitMonAlg = { 'doOnline'        : flags.Common.isOnline,      #Histograms for online (athenaPT) running
                            'doModules'       : flags.Common.isOnline, #Turn on/off the sets of 1744 module histograms (for dqmf)
                            'doLumiBlock'     : not flags.Common.isOnline,       #Turn on/off histograms stored for each lumi block
                            'doLowOccupancy'  : False,      #Turn on/off histograms with binning for cosmics/single beam                    
                            'doHighOccupancy' : True,       #Turn on/off histograms with binning for collisions
                            #'doHeavyIonMon'   : InDetFlags.doHeavyIon(),   # Histogram modification for heavy ion monitoring
                            #'RDOName'         : InDetKeys.PixelRDOs()
                            'doHeavyIonMon'   : False,      #Until new config ready
                            'RDOName'         : 'PixelRDOs',#Until new config ready
        }

        kwargsClusMonAlg = { 'doOnline'        : flags.Common.isOnline,      #Histograms for online (athenaPT) running
                             'doModules'       : flags.Common.isOnline, #Turn on/off the sets of 1744 module histograms (for dqmf)
                             'doLumiBlock'     : not flags.Common.isOnline,       #Turn on/off histograms stored for each lumi block
                             'doLowOccupancy'  : False,      #Turn on/off histograms with binning for cosmics/single beam
                             'doHighOccupancy' : True,       #Turn on/off histograms with binning for collisions
                             #'doHeavyIonMon'   : InDetFlags.doHeavyIon(),   # Histogram modification for heavy ion monitoring
                             'doHeavyIonMon'   : False,   #Until new config ready
                             #                  'doStatus'        : True,       #Turn on/off histograms with Module Status information
                             #'ClusterName'     : InDetKeys.PixelClusters(),
                             'ClusterName'     : 'PixelClusters', #Until new config ready
                             #'TrackName'       : InDetKeys.Tracks()
                             'TrackName'       : 'Tracks', #Until new config ready
        }

        kwargsErrMonAlg = { 'doOnline'        : flags.Common.isOnline,      #Histograms for online (athenaPT) running
                            'doModules'       : flags.Common.isOnline, #Turn on/off the sets of 1744 module histograms (for dqmf)
                            'doLumiBlock'     : not flags.Common.isOnline,       #Turn on/off histograms stored for each lumi block
                            'doLowOccupancy'  : False,      #Turn on/off histograms with binning for cosmics/single beam                    
                            'doHighOccupancy' : True,       #Turn on/off histograms with binning for collisions
                            #'doHeavyIonMon'   : InDetFlags.doHeavyIon()
                            'doHeavyIonMon'   : False, #Until new config is ready
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

        # Run 3 configs - stolen from SCT
        from SCT_Monitoring.TrackSummaryToolWorkaround import TrackSummaryToolWorkaround
        pixelAthClusterMonAlg.TrackSelectionTool.TrackSummaryTool = acc.popToolsAndMerge(TrackSummaryToolWorkaround(flags))
        pixelAthClusterMonAlg.TrackSelectionTool.Extrapolator     = acc.getPublicTool("InDetExtrapolator")
        
        
        PixelAthClusterMonAlgCfg(helper, pixelAthClusterMonAlg, **kwargsClusMonAlg)

        from PixelMonitoring.PixelAthErrorMonAlgCfg import PixelAthErrorMonAlgCfg
        pixelAthMonAlgErrorMonAlg = helper.addAlgorithm(CompFactory.PixelAthErrorMonAlg, 'PixelAthErrorMonAlg')
        for k, v in kwargsErrMonAlg.items():
            setattr(pixelAthMonAlgErrorMonAlg, k, v)
        PixelAthErrorMonAlgCfg(helper, pixelAthMonAlgErrorMonAlg, **kwargsErrMonAlg)
        acc.merge(helper.result())

    return acc
    
