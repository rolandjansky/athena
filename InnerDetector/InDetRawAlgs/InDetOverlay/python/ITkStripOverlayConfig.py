"""Define methods to construct configured ITk Strip overlay algorithms

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def ITkStripOverlayAlgCfg(flags, name="ITkStripOverlay", **kwargs):
    """Return a ComponentAccumulator for ITk Strip Overlay algorithm"""
    acc = ComponentAccumulator()

    kwargs.setdefault("BkgInputKey", f"{flags.Overlay.BkgPrefix}ITkStripRDOs")
    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}ITkStripRDOs")
    kwargs.setdefault("OutputKey", "ITkStripRDOs")

    if not flags.Overlay.DataOverlay:
        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
        acc.merge(SGInputLoaderCfg(flags, [f'SCT_RDO_Container#{kwargs["BkgInputKey"]}']))

    # Do SCT overlay
    acc.addEventAlgo(CompFactory.SCTOverlay(name, **kwargs))

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "SCT_RDO_Container#ITkStripRDOs"
        ]))

        if flags.Overlay.DataOverlay:
            acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
                "IDCInDetBSErrContainer#ITkStripByteStreamErrs"
            ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"SCT_RDO_Container#{flags.Overlay.SigPrefix}ITkStripRDOs"
        ]))

    return acc


def ITkStripTruthOverlayCfg(flags, name="ITkStripSDOOverlay", **kwargs):
    """Return a ComponentAccumulator for the ITk Strip SDO overlay algorithm"""
    acc = ComponentAccumulator()

    # We do not need background SCT SDOs
    kwargs.setdefault("BkgInputKey", "")

    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}ITkStripSDO_Map")
    kwargs.setdefault("OutputKey", "ITkStripSDO_Map")

    # Do SCT truth overlay
    acc.addEventAlgo(CompFactory.InDetSDOOverlay(name, **kwargs))

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "InDetSimDataCollection#ITkStripSDO_Map"
        ]))
    
    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"InDetSimDataCollection#{flags.Overlay.SigPrefix}ITkStripSDO_Map"
        ]))

    return acc


def ITkStripOverlayCfg(flags):
    """Configure and return a ComponentAccumulator for ITk Strip overlay"""
    acc = ComponentAccumulator()

    # Add ITk Strip overlay digitization algorithm
    from StripDigitization.StripDigitizationConfig import ITkStripOverlayDigitizationBasicCfg
    acc.merge(ITkStripOverlayDigitizationBasicCfg(flags))
    # Add ITk Strip overlay algorithm
    acc.merge(ITkStripOverlayAlgCfg(flags))
    # Add ITk Strip truth overlay
    acc.merge(ITkStripTruthOverlayCfg(flags))

    return acc
