# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Retrieve the default container names from the old-style configuration file
from TrackVertexAssociationTool.getTTVAToolForReco import (
    _DEFAULT_TRACK_CONT,
    _DEFAULT_VERTEX_CONT,
    _DECO_TOOL_NAME,
    _DECO_ALG_NAME,
    _VERTEX_DECO,
    _WEIGHT_DECO,
)

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def _assertPropertyValue(kwargs, key, force_value):
    if kwargs.setdefault(key, force_value) != force_value:
        raise ValueError(
            f"{key} property must be set to {force_value} (provided value is '{kwargs[key]}')"
        )


def usedInFitDecoratorCfg(
    flags, prefix, tracks=_DEFAULT_TRACK_CONT, vertices=_DEFAULT_VERTEX_CONT
):
    """ Create the alg to decorate the used-in-fit information for AMVF """
    return CompFactory.InDet.InDetUsedInVertexFitTrackDecorator(
        f"{prefix}_{_DECO_ALG_NAME}",
        UsedInFitDecoratorTool=CompFactory.InDet.InDetUsedInFitTrackDecoratorTool(
            f"{prefix}_{_DECO_TOOL_NAME}",
            AMVFVerticesDecoName=_VERTEX_DECO,
            AMVFWeightsDecoName=_WEIGHT_DECO,
            TrackContainer=tracks,
            VertexContainer=vertices,
        ),
    )


def TTVAToolCfg(flags, name, addDecoAlg=True, **kwargs):
    """Create a component accumulator containing a TTVA tool

    If addDecoAlg is True, also adds an algorithm for decorating the 'used-in-fit' information
    """

    acc = ComponentAccumulator()

    # First check that the user hasn't attempted to overwrite the AMVF vertices/weights decorations
    _assertPropertyValue(kwargs, "AMVFVerticesDeco", _VERTEX_DECO)
    _assertPropertyValue(kwargs, "AMVFWeightsDeco", _WEIGHT_DECO)

    tracks = kwargs.setdefault("TrackContName", _DEFAULT_TRACK_CONT)
    vertices = kwargs.pop("VertexContName", _DEFAULT_VERTEX_CONT)

    acc.setPrivateTools(
        CompFactory.CP.TrackVertexAssociationTool(
            name,
            **kwargs,
        ),
    )

    if addDecoAlg:
        acc.addEventAlgo(usedInFitDecoratorCfg(flags, name, tracks, vertices))
    return acc