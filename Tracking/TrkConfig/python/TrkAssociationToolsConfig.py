# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of TrkAssociationTools package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def PRDtoTrackMapToolCfg(flags, name='PRDtoTrackMapTool', **kwargs) :
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.Trk.PRDtoTrackMapTool(name, **kwargs))
    return acc
