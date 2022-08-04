# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for ISF_Algorithms package
"""

from AthenaCommon.CfgGetter import addAlgorithm

# Common tools, services and algorithms used by jobs
addAlgorithm("ISF_Algorithms.ISF_AlgorithmsConfigLegacy.getCollectionMerger",  "ISF_CollectionMerger")
addAlgorithm("ISF_Algorithms.ISF_AlgorithmsConfigLegacy.getSimHitTreeCreator", "ISF_SimHitTreeCreator")
addAlgorithm("ISF_Algorithms.ISF_AlgorithmsConfigLegacy.getSimEventFilter",    "ISF_SimEventFilter")
addAlgorithm("ISF_Algorithms.ISF_AlgorithmsConfigLegacy.getInvertedSimEventFilter", "ISF_InvertedSimEventFilter")
addAlgorithm("ISF_Algorithms.ISF_AlgorithmsConfigLegacy.getRenameHitCollections", "RenameHitCollections")
