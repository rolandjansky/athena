#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


from ActsTrkFittingTools.ActsTrkFittingToolsConfig import ActsKalmanFitterCfg

def ActsReFitterAlgCfg(flags, name="ActsReFitterAlg", **kwargs):
    result = ComponentAccumulator()

    actsKF = result.popToolsAndMerge(ActsKalmanFitterCfg(flags))

    kwargs.setdefault("ActsKalmanFitter", actsKF)
    kwargs.setdefault("TrackName", "ResolvedTracks")

    result.addEventAlgo(
        CompFactory.ActsReFitterAlg(
            name=name,
            **kwargs,
        )
    )

    return result



