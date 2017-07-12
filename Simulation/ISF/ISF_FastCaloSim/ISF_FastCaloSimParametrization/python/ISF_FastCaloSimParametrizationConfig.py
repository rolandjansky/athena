# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


"""
Tools configurations for ISF_FastCaloSimParametrization
"""
from AthenaCommon import CfgMgr

from AthenaCommon.Constants import *  # FATAL,ERROR etc.
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.DetFlags import DetFlags

def getFastCaloSimCaloExtrapolation(name="FastCaloSimCaloExtrapolation", **kwargs):
    from ISF_FastCaloSimParametrization.ISF_FastCaloSimParametrizationConf import FastCaloSimCaloExtrapolation
    return CfgMgr.FastCaloSimCaloExtrapolation(name, **kwargs)

def getFastCaloSimGeometryHelper(name="FastCaloSimGeometryHelper", **kwargs):
    from ISF_FastCaloSimParametrization.ISF_FastCaloSimParametrizationConf import FastCaloSimGeometryHelper
    return CfgMgr.FastCaloSimGeometryHelper(name, **kwargs)
