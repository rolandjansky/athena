# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


"""
Tools configurations for ISF_FastCaloSimParametrization
"""
from AthenaCommon import CfgMgr

def getFastCaloSimCaloExtrapolation(name="FastCaloSimCaloExtrapolation", **kwargs):
    kwargs.setdefault("CaloBoundaryR"             , [1148.0, 120.0, 41.0] )
    kwargs.setdefault("CaloBoundaryZ"             , [3550.0, 4587.0, 4587.0] )
    kwargs.setdefault("CaloMargin"                , 100    )
    kwargs.setdefault("Extrapolator"              , "TimedExtrapolator" )
    kwargs.setdefault("CaloGeometryHelper"        , "FastCaloSimGeometryHelper" )
    kwargs.setdefault("CaloEntrance"              , "InDet::Containers::InnerDetector"     )

    return CfgMgr.FastCaloSimCaloExtrapolation(name, **kwargs)


def getFastCaloSimGeometryHelper(name="FastCaloSimGeometryHelper", **kwargs):
    return CfgMgr.FastCaloSimGeometryHelper(name, **kwargs)
    

def getCaloCellContainerSD(name='CaloCellContainerSD', **kwargs):
    
    kwargs.setdefault ('NoVolumes', True)
    kwargs.setdefault("OutputCollectionNames", ["DefaultCaloCellContainer"])
    
    # Add ISF_FastHitConvertTool, which will also take care 
    # of creating mergable FastCaloSim HIT collections
    kwargs.setdefault("FastHitConvertTool", "ISF_FastHitConvertTool")

    return CfgMgr.CaloCellContainerSDTool(name, **kwargs)