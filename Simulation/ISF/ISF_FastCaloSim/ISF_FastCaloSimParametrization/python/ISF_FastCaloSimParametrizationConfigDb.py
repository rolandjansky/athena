# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for ISF_FastCaloSimParametrization
"""

from AthenaCommon.CfgGetter import addTool

# Common tools, services and algorithms used by jobs
addTool("ISF_FastCaloSimParametrization.ISF_FastCaloSimParametrizationConfigLegacy.getFastCaloSimCaloExtrapolation" , "FastCaloSimCaloExtrapolation"   )
addTool("ISF_FastCaloSimParametrization.ISF_FastCaloSimParametrizationConfigLegacy.getFastCaloSimGeometryHelper" , "FastCaloSimGeometryHelper"   )
addTool("ISF_FastCaloSimParametrization.ISF_FastCaloSimParametrizationConfigLegacy.getCaloCellContainerSD",           "CaloCellContainerSD")
