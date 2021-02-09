# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for ISF_Algorithms package
"""

from AthenaCommon.CfgGetter import addAlgorithm

# Common tools, services and algorithms used by jobs
addAlgorithm("ISF_Algorithms.ISF_AlgorithmsConfig.getCollectionMerger",  "ISF_CollectionMerger")
addAlgorithm("ISF_Algorithms.ISF_AlgorithmsConfig.getSimEventFilter",    "ISF_SimEventFilter")
addAlgorithm("ISF_Algorithms.ISF_AlgorithmsConfig.getInvertedSimEventFilter", "ISF_InvertedSimEventFilter")
addAlgorithm("ISF_Algorithms.ISF_AlgorithmsConfig.getRenameHitCollections","RenameHitCollections")
