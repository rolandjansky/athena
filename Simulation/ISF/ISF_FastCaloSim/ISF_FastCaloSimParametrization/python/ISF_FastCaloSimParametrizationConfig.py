# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


"""
Tools configurations for ISF_FastCaloSimParametrization
"""
from AthenaCommon import CfgMgr

def getFastCaloSimCaloExtrapolation(name="FastCaloSimCaloExtrapolation", **kwargs):
    from ISF_FastCaloSimParametrization.ISF_FastCaloSimParametrizationConf import FastCaloSimCaloExtrapolation  # noqa: F401

    kwargs.setdefault("CaloBoundaryR"             , [1148.0, 120.0, 41.0] )
    kwargs.setdefault("CaloBoundaryZ"             , [3550.0, 4587.0, 4587.0] )
    kwargs.setdefault("CaloMargin"                , 100    )
    kwargs.setdefault("Extrapolator"              , "TimedExtrapolator" )
    kwargs.setdefault("CaloGeometryHelper"        , "FastCaloSimGeometryHelper" )
    kwargs.setdefault("CaloEntrance"              , "InDet::Containers::InnerDetector"     )

    return CfgMgr.FastCaloSimCaloExtrapolation(name, **kwargs)


def getFastCaloSimGeometryHelper(name="FastCaloSimGeometryHelper", **kwargs):
    from ISF_FastCaloSimParametrization.ISF_FastCaloSimParametrizationConf import FastCaloSimGeometryHelper  # noqa: F401
    return CfgMgr.FastCaloSimGeometryHelper(name, **kwargs)
