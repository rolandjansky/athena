# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def EMBremCollectionBuilderCfg(flags, 
                                name="EMBremCollectionBuilder", 
                                **kwargs):
    acc = ComponentAccumulator()
    if "TrackParticleCreatorTool" not in kwargs:
        from InDetConfig.TrackRecoConfig import TrackParticleCreatorToolCfg
        kwargs["TrackParticleCreatorTool"] = acc.popToolsAndMerge(TrackParticleCreatorToolCfg(flags, "GSFBuildInDetParticleCreatorTool"))
    # TBC
    # if "TrackRefitTool" not in kwargs:
    #     kwargs["TrackRefitTool"] = acc.popToolsAndMerge( )

    alg = CompFactory.EMBremCollectionBuilder(name, **kwargs)
    acc.addEventAlgo(alg)
    return acc