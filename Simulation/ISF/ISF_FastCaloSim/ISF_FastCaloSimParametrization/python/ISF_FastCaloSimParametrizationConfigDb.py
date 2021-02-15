# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for ISF_FastCaloSimParametrization
"""

from AthenaCommon.CfgGetter import addTool

# Common tools, services and algorithms used by jobs
addTool("ISF_FastCaloSimParametrization.ISF_FastCaloSimParametrizationConfig.getFastCaloSimCaloExtrapolation" , "FastCaloSimCaloExtrapolation"   )
addTool("ISF_FastCaloSimParametrization.ISF_FastCaloSimParametrizationConfig.getFastCaloSimGeometryHelper" , "FastCaloSimGeometryHelper"   )
