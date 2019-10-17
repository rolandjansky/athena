#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def PixelMonitoringConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()

    # run on RAW only
    if flags.DQ.Environment in ('online', 'tier0', 'tier0Raw'):
        kwargsHitMonTool = { 'doOnline'        : flags.Common.isOnline,      #Histograms for online (athenaPT) running
                             'doModules'       : flags.Common.isOnline, #Turn on/off the sets of 1744 module histograms (for dqmf)
                             'doLumiBlock'     : not flags.Common.isOnline,       #Turn on/off histograms stored for each lumi block
                             'doLowOccupancy'  : False,      #Turn on/off histograms with binning for cosmics/single beam                    
                             'doHighOccupancy' : True,       #Turn on/off histograms with binning for collisions
                             #'doHeavyIonMon'   : InDetFlags.doHeavyIon(),   # Histogram modification for heavy ion monitoring
                             #'RDOName'         : InDetKeys.PixelRDOs()
                             'doHeavyIonMon'   : False,      #Until new config ready
                             'RDOName'         : 'PixelRDOs',#Until new config ready
        }

        kwargsClusMonTool = { 'doOnline'        : flags.Common.isOnline,      #Histograms for online (athenaPT) running
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

        kwargsErrMonTool = { 'doOnline'        : flags.Common.isOnline,      #Histograms for online (athenaPT) running
                             'doModules'       : flags.Common.isOnline, #Turn on/off the sets of 1744 module histograms (for dqmf)
                             'doLumiBlock'     : not flags.Common.isOnline,       #Turn on/off histograms stored for each lumi block
                             'doLowOccupancy'  : False,      #Turn on/off histograms with binning for cosmics/single beam                    
                             'doHighOccupancy' : True,       #Turn on/off histograms with binning for collisions
                             #'doHeavyIonMon'   : InDetFlags.doHeavyIon()
                             'doHeavyIonMon'   : False, #Until new config is ready
        }
        
        from AthenaMonitoring import AthMonitorCfgHelper
        helper = AthMonitorCfgHelper(DQMonFlags, "NewPixelMonitoring")

        pixelAthMonAlgHitMonTool = helper.addAlgorithm(PixelAthHitMonTool, 'PixelAthHitMonTool', 'PixelAthHitMonTool')
        for k, v in kwargsHitMonTool.items():
            setattr(pixelAthMonAlgHitMonTool, k, v)
            PixelAthHitMonToolCfg(helper, pixelAthMonAlgHitMonTool, **kwargsHitMonTool)

 
        pixelAthClusterMonTool = helper.addAlgorithm(PixelAthClusterMonTool, 'PixelAthClusterMonTool', 'PixelAthClusterMonTool')
        for k, v in kwargsClusMonTool.items():
            setattr(pixelAthClusterMonTool, k, v)
        pixelAthClusterMonTool.TrackSelectionTool.UseTrkTrackTools = True
        pixelAthClusterMonTool.TrackSelectionTool.CutLevel         = "TightPrimary"
        pixelAthClusterMonTool.TrackSelectionTool.maxNPixelHoles   = 1
        pixelAthClusterMonTool.TrackSelectionTool.maxD0            = 2
        pixelAthClusterMonTool.TrackSelectionTool.maxZ0            = 150
        pixelAthClusterMonTool.TrackSelectionTool.TrackSummaryTool = InDetTrackSummaryTool
        pixelAthClusterMonTool.TrackSelectionTool.Extrapolator     = InDetExtrapolator
        
        #print getattr(pixelAthClusterMonTool, 'onTrack') 
        PixelAthClusterMonToolCfg(helper, pixelAthClusterMonTool, **kwargsClusMonTool)
 
        pixelAthMonAlgErrorMonTool = helper.addAlgorithm(PixelAthErrorMonTool, 'PixelAthErrorMonTool', 'PixelAthErrorMonTool')
        for k, v in kwargsErrMonTool.items():
            setattr(pixelAthMonAlgErrorMonTool, k, v)
        PixelAthErrorMonToolCfg(helper, pixelAthMonAlgErrorMonTool, **kwargsErrMonTool)
        acc.merge(helper.result())

    return acc
    
