"""Define methods to construct configured HGTD overlay algorithms

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def HGTD_OverlayAlgCfg(flags, name="HGTD_Overlay", **kwargs):
    """Return a ComponentAccumulator for HGTD overlay algorithm"""
    acc = ComponentAccumulator()

    kwargs.setdefault("BkgInputKey", f"{flags.Overlay.BkgPrefix}HGTD_RDOs")
    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}HGTD_RDOs")
    kwargs.setdefault("OutputKey", "HGTD_RDOs")

    if not flags.Overlay.DataOverlay:
        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
        acc.merge(SGInputLoaderCfg(flags, [f'HGTD_RDO_Container#{kwargs["BkgInputKey"]}']))

    # Do HGTD overlay
    acc.addEventAlgo(CompFactory.HGTD_Overlay(name, **kwargs))

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "HGTD_RDO_Container#HGTD_RDOs"
        ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"HGTD_RDO_Container#{flags.Overlay.SigPrefix}HGTD_RDOs"
        ]))

    return acc


def HGTD_TruthOverlayCfg(flags, name="HGTD_SDOOverlay", **kwargs):
    """Return a ComponentAccumulator for the HGTD SDO overlay algorithm"""
    acc = ComponentAccumulator()

    # We do not need background Pixel SDOs
    kwargs.setdefault("BkgInputKey", "")

    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}HGTD_SDO_Map")
    kwargs.setdefault("OutputKey", "HGTD_SDO_Map")

    # Do Pixel truth overlay
    acc.addEventAlgo(CompFactory.InDetSDOOverlay(name, **kwargs))

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "InDetSimDataCollection#HGTD_SDO_Map"
        ]))
    
    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"InDetSimDataCollection#{flags.Overlay.SigPrefix}HGTD_SDO_Map"
        ]))

    return acc


def HGTD_OverlayCfg(flags):
    """Configure and return a ComponentAccumulator for HGTD overlay"""
    acc = ComponentAccumulator()

    # Add HGTD overlay digitization algorithm
    from HGTD_Digitization.HGTD_DigitizationConfig import HGTD_OverlayDigitizationBasicCfg
    acc.merge(HGTD_OverlayDigitizationBasicCfg(flags))
    # Add HGTD overlay algorithm
    acc.merge(HGTD_OverlayAlgCfg(flags))
    # Add HGTD truth overlay
    acc.merge(HGTD_TruthOverlayCfg(flags))

    return acc
