# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def InDetUsedInFitTrackDecoratorToolCfg(ConfigFlags, name, **kwargs):
    """Configure the InDetUsedInFitTrackDecoratorTool"""
    acc = ComponentAccumulator()
    InDetUsedInFitTrackDecoratorTool = CompFactory.InDet.InDetUsedInFitTrackDecoratorTool
    acc.setPrivateTools(InDetUsedInFitTrackDecoratorTool(name, **kwargs))
    return acc
