# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def BadBatmanToolCfg(ConfigFlags):
    """Configure the bad batman augmentation tool"""
    acc = ComponentAccumulator()
    badBatmanTool = CompFactory.DerivationFramework.BadBatmanAugmentationTool("BadBatmanAugmentationTool")
    acc.addPublicTool(badBatmanTool, primary=True)
    return acc

def DistanceInTrainToolCfg(ConfigFlags):
    """Configure the distance in train augmentation tool"""
    acc = ComponentAccumulator()
    from LumiBlockComps.BunchCrossingCondAlgConfig import BunchCrossingCondAlgCfg 
    acc.merge(BunchCrossingCondAlgCfg(ConfigFlags))
    distanceInTrainTool = CompFactory.DerivationFramework.DistanceInTrainAugmentationTool("DistanceInTrainAugmentationTool")
    acc.addPublicTool(distanceInTrainTool, primary=True)
    return acc
