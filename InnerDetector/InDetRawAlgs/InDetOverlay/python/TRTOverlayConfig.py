"""Define methods to construct configured TRT overlay algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def TRTRawDataProviderAlgCfg(flags, name="TRTRawDataProvider", **kwargs):
    """Return a ComponentAccumulator for TRT raw data provider"""
    # Temporary until available in the central location
    acc = ComponentAccumulator()

    kwargs.setdefault("RDOKey", flags.Overlay.BkgPrefix + "TRT_RDOs")

    from RegionSelector.RegSelToolConfig import regSelTool_TRT_Cfg
    kwargs.setdefault("RegSelTool", acc.popToolsAndMerge(regSelTool_TRT_Cfg(flags)))

    TRTRawDataProvider = CompFactory.TRTRawDataProvider
    alg = TRTRawDataProvider(name, **kwargs)
    acc.addEventAlgo(alg)

    ByteStreamAddressProviderSvc = CompFactory.ByteStreamAddressProviderSvc
    bsAddressProviderSvc = ByteStreamAddressProviderSvc(TypeNames=[
        "InDet::TRT_DriftCircleContainer/TRT_DriftCircle",
    ])
    acc.addService(bsAddressProviderSvc)

    return acc


def TRTDataOverlayExtraCfg(flags, **kwargs):
    """Return a ComponentAccumulator with TRT data overlay specifics"""
    acc = ComponentAccumulator()

    # We need to convert BS to RDO for data overlay
    acc.merge(TRTRawDataProviderAlgCfg(flags))

    # Add additional conditions infrastructure
    from InDetOverlay.TRT_ConditionsConfig import TRT_CablingSvcCfg, TRT_OnlineFoldersCfg
    acc.merge(TRT_CablingSvcCfg(flags))
    acc.merge(TRT_OnlineFoldersCfg(flags))

    return acc


def TRTOverlayAlgCfg(flags, name="TRTOverlay", **kwargs):
    """Return a ComponentAccumulator for TRTOverlay algorithm"""
    acc = ComponentAccumulator()
    from TRT_GeoModel.TRT_GeoModelConfig import TRT_GeometryCfg
    acc.merge(TRT_GeometryCfg(flags))
    from InDetOverlay.TRT_ConditionsConfig import TRTStrawCondAlgCfg
    acc.merge(TRTStrawCondAlgCfg(flags))

    kwargs.setdefault("SortBkgInput", flags.Overlay.DataOverlay)
    kwargs.setdefault("BkgInputKey", flags.Overlay.BkgPrefix + "TRT_RDOs")
    kwargs.setdefault("SignalInputKey", flags.Overlay.SigPrefix + "TRT_RDOs")
    kwargs.setdefault("SignalInputSDOKey", flags.Overlay.SigPrefix + "TRT_SDO_Map")
    kwargs.setdefault("OutputKey", "TRT_RDOs")

    # HT hit correction fraction
    kwargs.setdefault("TRT_HT_OccupancyCorrectionBarrel", 0.110)
    kwargs.setdefault("TRT_HT_OccupancyCorrectionEndcap", 0.090)
    kwargs.setdefault("TRT_HT_OccupancyCorrectionBarrelNoE", 0.060)
    kwargs.setdefault("TRT_HT_OccupancyCorrectionEndcapNoE", 0.050)

    # Do TRT overlay
    TRTOverlay = CompFactory.TRTOverlay
    alg = TRTOverlay(name, **kwargs)

    from InDetOverlay.TRT_ConditionsConfig import TRT_LocalOccupancyCfg, TRT_StrawStatusSummaryToolCfg
    alg.TRT_LocalOccupancyTool = acc.popToolsAndMerge(
        TRT_LocalOccupancyCfg(flags))
    alg.TRTStrawSummaryTool = acc.popToolsAndMerge(
        TRT_StrawStatusSummaryToolCfg(flags))
    acc.addEventAlgo(alg)

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "TRT_RDO_Container#TRT_RDOs"
        ]))

        if flags.Overlay.DataOverlay:
            acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
                "TRT_BSErrContainer#TRT_ByteStreamErrs"
            ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            "TRT_RDO_Container#" + flags.Overlay.SigPrefix + "TRT_RDOs"
        ]))

    return acc


def TRTTruthOverlayCfg(flags, name="TRTSDOOverlay", **kwargs):
    """Return a ComponentAccumulator for the TRT SDO overlay algorithm"""
    acc = ComponentAccumulator()

    # We do not need background TRT SDOs for data overlay
    if flags.Overlay.DataOverlay:
        kwargs.setdefault("BkgInputKey", "")
    else:
        kwargs.setdefault("BkgInputKey", flags.Overlay.BkgPrefix + "TRT_SDO_Map")

    kwargs.setdefault("SignalInputKey",
                      flags.Overlay.SigPrefix + "TRT_SDO_Map")
    kwargs.setdefault("OutputKey", "TRT_SDO_Map")

    # Do TRT truth overlay
    InDetSDOOverlay = CompFactory.InDetSDOOverlay
    alg = InDetSDOOverlay(name, **kwargs)
    acc.addEventAlgo(alg)

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "InDetSimDataCollection#TRT_SDO_Map"
        ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            "InDetSimDataCollection#" + flags.Overlay.SigPrefix + "TRT_SDO_Map"
        ]))

    return acc


def TRTOverlayCfg(flags):
    """Configure and return a ComponentAccumulator for TRT overlay"""
    acc = ComponentAccumulator()

    # Add data overlay specifics
    if flags.Overlay.DataOverlay:
        acc.merge(TRTDataOverlayExtraCfg(flags))

    # Add TRT overlay digitization algorithm
    from TRT_Digitization.TRT_DigitizationConfigNew import TRT_OverlayDigitizationBasicCfg
    acc.merge(TRT_OverlayDigitizationBasicCfg(flags))
    # Add TRT overlay algorithm
    acc.merge(TRTOverlayAlgCfg(flags))
    # Add TRT truth overlay
    acc.merge(TRTTruthOverlayCfg(flags))

    return acc
