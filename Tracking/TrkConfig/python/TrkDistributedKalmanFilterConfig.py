# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of TrkDistributedKalmanFilter package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def DistributedKalmanFilterCfg(flags,
                               name="DistributedKalmanFilter",
                               **kwargs):
    acc = ComponentAccumulator()

    if 'ExtrapolatorTool' not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
        kwargs.setdefault('ExtrapolatorTool',
                          acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags)))

    if 'ROTcreator' not in kwargs:
        from TrkConfig.TrkRIO_OnTrackCreatorConfig import InDetRotCreatorCfg
        InDetRotCreator = acc.popToolsAndMerge(InDetRotCreatorCfg(flags))
        kwargs.setdefault('ROTcreator', InDetRotCreator)

    DistributedKalmanFilter = CompFactory.Trk.DistributedKalmanFilter(name, **kwargs)
    acc.setPrivateTools(DistributedKalmanFilter)
    return acc


def ITkDistributedKalmanFilterCfg(flags,
                                  name="ITkDistributedKalmanFilter",
                                  **kwargs):
    acc = ComponentAccumulator()

    if 'ExtrapolatorTool' not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
        kwargs.setdefault('ExtrapolatorTool',
                          acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags)))

    if 'ROTcreator' not in kwargs:
        from TrkConfig.TrkRIO_OnTrackCreatorConfig import ITkRotCreatorCfg
        ITkRotCreator = acc.popToolsAndMerge(ITkRotCreatorCfg(flags))
        kwargs.setdefault('ROTcreator', ITkRotCreator)

    DistributedKalmanFilter = CompFactory.Trk.DistributedKalmanFilter(name, **kwargs)
    acc.setPrivateTools(DistributedKalmanFilter)
    return acc
