"""Define functions for TTL1 Overlay with ComponentAccumulator

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from LArDigitization.LArDigitizationConfigNew import LArOverlayTriggerDigitizationBasicCfg
from TileSimAlgs.TileHitToTTL1Config import TileHitToTTL1Cfg


def OverlayTTL1Cfg(flags, name="OverlayTTL1", **kwargs):
    """OverlayTTL1 configuration using ComponentAccumulator"""
    acc = ComponentAccumulator()
    acc.merge(LArOverlayTriggerDigitizationBasicCfg(flags))
    acc.merge(TileHitToTTL1Cfg(flags))

    kwargs.setdefault("BkgEmTTL1Key",
                      flags.Overlay.BkgPrefix + "LArTTL1EM")
    kwargs.setdefault("SignalEmTTL1Key",
                      flags.Overlay.SigPrefix + "LArTTL1EM")
    kwargs.setdefault("OutputEmTTL1Key", "LArTTL1EM")

    kwargs.setdefault("BkgHadTTL1Key",
                      flags.Overlay.BkgPrefix + "LArTTL1HAD")
    kwargs.setdefault("SignalHadTTL1Key",
                      flags.Overlay.SigPrefix + "LArTTL1HAD")
    kwargs.setdefault("OutputHadTTL1Key", "LArTTL1HAD")

    kwargs.setdefault("BkgTileTTL1Key",
                      flags.Overlay.BkgPrefix + "TileTTL1Cnt")
    kwargs.setdefault("SignalTileTTL1Key",
                      flags.Overlay.SigPrefix + "TileTTL1Cnt")
    kwargs.setdefault("OutputTileTTL1Key", "TileTTL1Cnt")

    kwargs.setdefault("BkgTileMBTSTTL1Key",
                      flags.Overlay.BkgPrefix + "TileTTL1MBTS")
    kwargs.setdefault("SignalTileMBTSTTL1Key",
                      flags.Overlay.SigPrefix + "TileTTL1MBTS")
    kwargs.setdefault("OutputTileMBTSTTL1Key", "TileTTL1MBTS")

    OverlayTTL1 = CompFactory.LVL1.OverlayTTL1
    acc.addEventAlgo(OverlayTTL1(name, **kwargs))

    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, streamName='RDO', ItemList=[
            'LArTTL1Container#LArTTL1EM',
            'LArTTL1Container#LArTTL1HAD',
            'TileTTL1Container#TileTTL1Cnt',
            'TileTTL1Container#TileTTL1MBTS',
        ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, streamName='RDO_SGNL', ItemList=[
            'LArTTL1Container#' + flags.Overlay.SigPrefix + 'LArTTL1EM',
            'LArTTL1Container#' + flags.Overlay.SigPrefix + 'LArTTL1HAD',
            'TileTTL1Container#' + flags.Overlay.SigPrefix + 'TileTTL1Cnt',
            'TileTTL1Container#' + flags.Overlay.SigPrefix + 'TileTTL1MBTS',
        ]))

    return acc
