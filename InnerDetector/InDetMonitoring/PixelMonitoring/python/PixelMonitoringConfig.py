#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.Enums import BeamType

def PixelMonitoringConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    forceOnline = False # for testing of online monitoring and 100LB histograms
    doHitMonAlg       = True
    doClusterMonAlg   = True
    doErrorMonAlg     = True
    doMVAMonAlg       = True #allowed only if online, see below

    from InDetRecExample.InDetKeys          import InDetKeys
    # run on RAW only
    if flags.DQ.Environment in ('online', 'tier0', 'tier0Raw'):
        if forceOnline: 
            isOnline = True
        else:
            isOnline = flags.Common.isOnline
        if not isOnline: doMVAMonAlg=False
        kwargsHitMonAlg = { 'doOnline'        : isOnline,      #Histograms for online (GlobalMonitoring) running
                            'doLumiBlock'     : not isOnline,  #Turn on/off histograms stored every 1(20) lumi block(s)
                            'doFEPlots'       : True,                       #Turn on/off per FE-I3 histograms
                            'RDOName'         : InDetKeys.PixelRDOs()       #'PixelRDOs'
        }

        kwargsClusMonAlg = { 'doOnline'        : isOnline,      #Histograms for online (GlobalMonitoring) running
                             'doLumiBlock'     : not isOnline,  #Turn on/off histograms stored every 1(20) lumi block(s)
                             'doLowOccupancy'  : flags.Beam.Type is BeamType.Cosmics, #Setting up 1D histogram ranges and binnings, if False, high occupancy i.e. collisions settings will be used
                             'doHeavyIonMon'   : flags.Reco.EnableHI,     #Setting up 1D histogram ranges and binnings for heavy ions
                             'doFEPlots'       : True,                       #Turn on/off per FE-I3 histograms
                             'ClusterName'     : InDetKeys.PixelClusters(),  #'PixelClusters'
                             'TrackName'       : InDetKeys.Tracks()          #'Tracks'
        }

        kwargsErrMonAlg = { 'doOnline'        : isOnline,        #Histograms for online (GlobalMonitoring) running
                            'doLumiBlock'     : not isOnline     #Turn on/off histograms stored every 1(20) lumi block(s)
        }

        kwargsMVAMonAlg = { 'calibFolder'     : '20220314',
                            'RDOName'         : InDetKeys.PixelRDOs(),      #'PixelRDOs'
                            'ClusterName'     : InDetKeys.PixelClusters(),  #'PixelClusters'
                            'TrackName'       : InDetKeys.Tracks()          #'Tracks'
        }

        from AthenaMonitoring import AthMonitorCfgHelper
        helper = AthMonitorCfgHelper(flags, "NewPixelMonitoring")

        from AthenaConfiguration.ComponentFactory import CompFactory
        from InDetConfig.TrackingCommonConfig import InDetTrackSummaryToolCfg
        from AthenaMonitoring.FilledBunchFilterToolConfig import FilledBunchFilterToolCfg

        if doHitMonAlg:
            from PixelMonitoring.PixelAthHitMonAlgCfg import PixelAthHitMonAlgCfg
            pixelAthHitMonAlg = helper.addAlgorithm(CompFactory.PixelAthHitMonAlg, 'PixelAthHitMonAlg', addFilterTools = [FilledBunchFilterToolCfg(flags)])
            kwargsHitMonAlg.setdefault(  'PixelDetElStatus',           'PixelDetectorElementStatus' )
            kwargsHitMonAlg.setdefault(  'PixelDetElStatusActiveOnly', 'PixelDetectorElementStatusActiveOnly')
            for k, v in kwargsHitMonAlg.items():
                setattr(pixelAthHitMonAlg, k, v)
            PixelAthHitMonAlgCfg(helper, pixelAthHitMonAlg, **kwargsHitMonAlg)

        if doClusterMonAlg:
            from PixelMonitoring.PixelAthClusterMonAlgCfg import PixelAthClusterMonAlgCfg
            pixelAthClusterMonAlg = helper.addAlgorithm(CompFactory.PixelAthClusterMonAlg, 'PixelAthClusterMonAlg', addFilterTools = [FilledBunchFilterToolCfg(flags)])
            kwargsClusMonAlg.setdefault(  'PixelDetElStatus',           'PixelDetectorElementStatus' )
            kwargsClusMonAlg.setdefault(  'PixelDetElStatusActiveOnly', 'PixelDetectorElementStatusActiveOnly')
            for k, v in kwargsClusMonAlg.items():
                setattr(pixelAthClusterMonAlg, k, v)
            pixelAthClusterMonAlg.TrackSelectionTool = CompFactory.InDet.InDetTrackSelectionTool('PixelAthClusterMonAlg_TrackSelectionTool')
            pixelAthClusterMonAlg.TrackSelectionTool.UseTrkTrackTools = True
            pixelAthClusterMonAlg.TrackSelectionTool.CutLevel         = "TightPrimary"
            pixelAthClusterMonAlg.TrackSelectionTool.maxNPixelHoles   = 1
            if flags.Beam.Type is not BeamType.Cosmics:
                pixelAthClusterMonAlg.TrackSelectionTool.maxD0            = 2
                pixelAthClusterMonAlg.TrackSelectionTool.maxZ0            = 150
            pixelAthClusterMonAlg.TrackSelectionTool.TrackSummaryTool = acc.getPrimaryAndMerge(InDetTrackSummaryToolCfg(flags))
            pixelAthClusterMonAlg.TrackSelectionTool.Extrapolator     = acc.getPublicTool("InDetExtrapolator")

            PixelAthClusterMonAlgCfg(helper, pixelAthClusterMonAlg, **kwargsClusMonAlg)

        if doErrorMonAlg:
            from PixelMonitoring.PixelAthErrorMonAlgCfg import PixelAthErrorMonAlgCfg
            pixelAthMonAlgErrorMonAlg = helper.addAlgorithm(CompFactory.PixelAthErrorMonAlg, 'PixelAthErrorMonAlg', addFilterTools = [FilledBunchFilterToolCfg(flags)])
            kwargsErrMonAlg.setdefault(  'PixelDetElStatusActiveOnly', 'PixelDetectorElementStatusActiveOnly')
            kwargsErrMonAlg.setdefault(  'PixelDetElStatusActiveOnly', 'PixelDetectorElementStatusActiveOnly')
            kwargsErrMonAlg.setdefault(  'PixelByteStreamErrs', 'PixelByteStreamErrs')
            kwargsErrMonAlg.setdefault(  'UseByteStreamFEI4', not flags.Input.isMC)
            kwargsErrMonAlg.setdefault(  'UseByteStreamFEI3', not flags.Input.isMC)
            kwargsErrMonAlg.setdefault(  'UseByteStreamRD53', False)
            for k, v in kwargsErrMonAlg.items():
                setattr(pixelAthMonAlgErrorMonAlg, k, v)
            PixelAthErrorMonAlgCfg(helper, pixelAthMonAlgErrorMonAlg, **kwargsErrMonAlg)

        if doMVAMonAlg:
            from PixelMonitoring.PixelAthMVAMonAlgCfg import PixelAthMVAMonAlgCfg
            pixelAthMVAMonAlg = helper.addAlgorithm(CompFactory.PixelAthMVAMonAlg, 'PixelAthMVAMonAlg', addFilterTools = [FilledBunchFilterToolCfg(flags)])
            for k, v in kwargsMVAMonAlg.items():
                setattr(pixelAthMVAMonAlg, k, v)
            pixelAthMVAMonAlg.TrackSelectionTool = CompFactory.InDet.InDetTrackSelectionTool('PixelAthMVAMonAlg_TrackSelectionTool')
            pixelAthMVAMonAlg.TrackSelectionTool.UseTrkTrackTools = True
            pixelAthMVAMonAlg.TrackSelectionTool.CutLevel         = "TightPrimary"
            pixelAthMVAMonAlg.TrackSelectionTool.maxNPixelHoles   = 1
            if flags.Beam.Type is not BeamType.Cosmics:
                pixelAthMVAMonAlg.TrackSelectionTool.maxD0            = 2
                pixelAthMVAMonAlg.TrackSelectionTool.maxZ0            = 150

            pixelAthMVAMonAlg.TrackSelectionTool.TrackSummaryTool = acc.getPrimaryAndMerge(InDetTrackSummaryToolCfg(flags))
            pixelAthMVAMonAlg.TrackSelectionTool.Extrapolator     = acc.getPublicTool("InDetExtrapolator")
            pixelAthMVAMonAlg.Extrapolator                        = acc.getPublicTool("InDetExtrapolator")
            PixelAthMVAMonAlgCfg(helper, pixelAthMVAMonAlg, **kwargsMVAMonAlg)

        acc.merge(helper.result())

    return acc
    
