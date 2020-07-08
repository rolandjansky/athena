"""ComponentAccumulator config of tools for ISF_FastCaloSimParametrization

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentFactory import CompFactory


def FastCaloSimCaloExtrapolationCfg(flags, name="FastCaloSimCaloExtrapolation", **kwargs):
    kwargs.setdefault("CaloBoundaryR", [1148.0, 120.0, 41.0])
    kwargs.setdefault("CaloBoundaryZ", [3550.0, 4587.0, 4587.0])
    kwargs.setdefault("CaloMargin", 100)
    kwargs.setdefault("Extrapolator", "TimedExtrapolator") # TODO prevent circular import with AdditionalConfigNew
    kwargs.setdefault("CaloGeometryHelper", "FastCaloSimGeometryHelper")
    kwargs.setdefault("CaloEntrance", "InDet::Containers::InnerDetector")

    return CompFactory.FastCaloSimCaloExtrapolation(name, **kwargs)


def FastCaloSimGeometryHelperCfg(flags, name="FastCaloSimGeometryHelper", **kwargs):
    return CompFactory.FastCaloSimGeometryHelper(name, **kwargs)
