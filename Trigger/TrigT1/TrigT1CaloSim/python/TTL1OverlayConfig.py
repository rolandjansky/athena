"""Define functions for TTL1 Overlay with ComponentAccumulator

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from LArDigitization.LArDigitizationConfig import LArOverlayTriggerDigitizationBasicCfg
from TileSimAlgs.TileHitToTTL1Config import TileHitToTTL1Cfg


def LArTTL1OverlayCfg(flags, name="LArTTL1Overlay", **kwargs):
    """LArTTL1Overlay configuration using ComponentAccumulator"""
    acc = ComponentAccumulator()
    acc.merge(LArOverlayTriggerDigitizationBasicCfg(flags))

    kwargs.setdefault("BkgEmTTL1Key", f"{flags.Overlay.BkgPrefix}LArTTL1EM")
    kwargs.setdefault("SignalEmTTL1Key", f"{flags.Overlay.SigPrefix}LArTTL1EM")
    kwargs.setdefault("OutputEmTTL1Key", "LArTTL1EM")

    kwargs.setdefault("BkgHadTTL1Key", f"{flags.Overlay.BkgPrefix}LArTTL1HAD")
    kwargs.setdefault("SignalHadTTL1Key", f"{flags.Overlay.SigPrefix}LArTTL1HAD")
    kwargs.setdefault("OutputHadTTL1Key", "LArTTL1HAD")

    from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
    acc.merge(SGInputLoaderCfg(flags, [
        f'LArTTL1Container#{kwargs["BkgEmTTL1Key"]}',
        f'LArTTL1Container#{kwargs["BkgHadTTL1Key"]}',
    ]))

    acc.addEventAlgo(CompFactory.LVL1.LArTTL1Overlay(name, **kwargs))

    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, streamName='RDO', ItemList=[
            'LArTTL1Container#LArTTL1EM',
            'LArTTL1Container#LArTTL1HAD',
        ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, streamName='RDO_SGNL', ItemList=[
            f'LArTTL1Container#{flags.Overlay.SigPrefix}LArTTL1EM',
            f'LArTTL1Container#{flags.Overlay.SigPrefix}LArTTL1HAD',
        ]))

    return acc


def TileTTL1OverlayCfg(flags, name="TileTTL1Overlay", **kwargs):
    """TileTTL1Overlay configuration using ComponentAccumulator"""
    acc = ComponentAccumulator()
    acc.merge(TileHitToTTL1Cfg(flags))

    kwargs.setdefault("BkgTileTTL1Key", f"{flags.Overlay.BkgPrefix}TileTTL1Cnt")
    kwargs.setdefault("SignalTileTTL1Key", f"{flags.Overlay.SigPrefix}TileTTL1Cnt")
    kwargs.setdefault("OutputTileTTL1Key", "TileTTL1Cnt")
    inputs = [f'TileTTL1Container#{kwargs["BkgTileTTL1Key"]}']

    if flags.Detector.EnableMBTS:
        kwargs.setdefault("BkgTileMBTSTTL1Key", f"{flags.Overlay.BkgPrefix}TileTTL1MBTS")
        kwargs.setdefault("SignalTileMBTSTTL1Key", f"{flags.Overlay.SigPrefix}TileTTL1MBTS")
        kwargs.setdefault("OutputTileMBTSTTL1Key", "TileTTL1MBTS")
        inputs.append(f'TileTTL1Container#{kwargs["BkgTileMBTSTTL1Key"]}')
    else:
        kwargs.setdefault("BkgTileMBTSTTL1Key", "")
        kwargs.setdefault("SignalTileMBTSTTL1Key", "")
        kwargs.setdefault("OutputTileMBTSTTL1Key", "")

    from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
    acc.merge(SGInputLoaderCfg(flags, inputs))

    acc.addEventAlgo(CompFactory.LVL1.TileTTL1Overlay(name, **kwargs))

    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, streamName='RDO', ItemList=[
            'TileTTL1Container#TileTTL1Cnt',
            'TileTTL1Container#TileTTL1MBTS',
        ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, streamName='RDO_SGNL', ItemList=[
            f'TileTTL1Container#{flags.Overlay.SigPrefix}TileTTL1Cnt',
            f'TileTTL1Container#{flags.Overlay.SigPrefix}TileTTL1MBTS',
        ]))

    return acc
