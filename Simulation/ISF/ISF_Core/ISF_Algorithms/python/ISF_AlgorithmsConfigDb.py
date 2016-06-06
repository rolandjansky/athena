# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for ISF_Algorithms package
"""

from AthenaCommon.CfgGetter import addAlgorithm

# Common tools, services and algorithms used by jobs
addAlgorithm("ISF_Algorithms.ISF_AlgorithmsConfig.getCollectionMerger",  "ISF_CollectionMerger")
