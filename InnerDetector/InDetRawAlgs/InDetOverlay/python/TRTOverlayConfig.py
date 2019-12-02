"""Define methods to construct configured TRT overlay algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def TRTOverlayAlgCfg(flags, name = "TRTOverlay", **kwargs):
    """Return a ComponentAccumulator for TRTOverlay algorithm"""
    acc = ComponentAccumulator()
    from TRT_GeoModel.TRT_GeoModelConfig import TRT_GeometryCfg
    acc.merge(TRT_GeometryCfg( flags ))
    from InDetOverlay.TRT_ConditionsConfig import TRTStrawCondAlgCfg
    acc.merge(TRTStrawCondAlgCfg(flags))

    kwargs.setdefault("BkgInputKey", flags.Overlay.BkgPrefix + "TRT_RDOs")
    kwargs.setdefault("SignalInputKey", flags.Overlay.SigPrefix + "TRT_RDOs")
    kwargs.setdefault("OutputKey", "TRT_RDOs")

    kwargs.setdefault("includeBkg", True)

    # HT hit correction fraction
    kwargs.setdefault("TRT_HT_OccupancyCorrectionBarrel", 0.110)
    kwargs.setdefault("TRT_HT_OccupancyCorrectionEndcap", 0.090)
    kwargs.setdefault("TRT_HT_OccupancyCorrectionBarrelNoE", 0.060)
    kwargs.setdefault("TRT_HT_OccupancyCorrectionEndcapNoE", 0.050)

    # Do TRT overlay
    from InDetOverlay.InDetOverlayConf import TRTOverlay
    alg = TRTOverlay(name, **kwargs)

    from InDetOverlay.TRT_ConditionsConfig import TRT_LocalOccupancyCfg, TRT_StrawStatusSummaryToolCfg
    alg.TRT_LocalOccupancyTool = acc.popToolsAndMerge(TRT_LocalOccupancyCfg(flags))
    alg.TRTStrawSummaryTool = acc.popToolsAndMerge(TRT_StrawStatusSummaryToolCfg(flags))
    acc.addEventAlgo(alg)

    # Setup output
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(flags, "RDO", ItemList = [
        "TRT_RDO_Container#TRT_RDOs"
    ]))

    return acc


def TRTTruthOverlayCfg(flags, name = "TRTSDOOverlay", **kwargs):
    """Return a ComponentAccumulator for the TRT SDO overlay algorithm"""
    acc = ComponentAccumulator()

    # We do not need background TRT SDOs
    kwargs.setdefault("BkgInputKey", "")

    kwargs.setdefault("SignalInputKey", flags.Overlay.BkgPrefix + "TRT_SDO_Map")
    kwargs.setdefault("OutputKey", "TRT_SDO_Map")

    # Do TRT truth overlay
    from InDetOverlay.InDetOverlayConf import InDetSDOOverlay
    alg = InDetSDOOverlay(name, **kwargs)
    acc.addEventAlgo(alg)

    # Setup output
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(flags, "RDO", ItemList = [
        "InDetSimDataCollection#TRT_SDO_Map"
    ]))

    return acc


def TRTOverlayCfg(flags):
    """Configure and return a ComponentAccumulator for TRT overlay"""
    acc = ComponentAccumulator()

    # Add TRT overlay digitization algorithm
    from TRT_Digitization.TRT_DigitizationConfigNew import TRT_OverlayDigitizationCfg
    acc.merge(TRT_OverlayDigitizationCfg(flags))
    # Add TRT overlay algorithm
    acc.merge(TRTOverlayAlgCfg(flags))
    # Add TRT truth overlay
    acc.merge(TRTTruthOverlayCfg(flags))

    return acc
