# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Retrieve the default container names from the old-style configuration file
from TrackVertexAssociationTool.getTTVAToolForReco import (
    _DEFAULT_TRACK_CONT,
    _DEFAULT_VERTEX_CONT,
    _VERTEX_DECO,
    _WEIGHT_DECO,
)

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from InDetUsedInVertexFitTrackDecorator.UsedInVertexFitTrackDecoratorCfg import getUsedInVertexFitTrackDecoratorAlg


def _assertPropertyValue(kwargs, key, force_value):
    if kwargs.setdefault(key, force_value) != force_value:
        raise ValueError(
            f"{key} property must be set to {force_value} (provided value is '{kwargs[key]}')"
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
        acc.addEventAlgo( getUsedInVertexFitTrackDecoratorAlg(tracks, vertices) )
    return acc
