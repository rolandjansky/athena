#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def InDetHardScatterSelectionToolCfg(ConfigFlags, name):
    """Configure the InDet hard scatter selection tool"""
    acc = ComponentAccumulator()
    InDetHardScatterSelectionTool = CompFactory.InDet.InDetHardScatterSelectionTool
    acc.setPrivateTools(InDetHardScatterSelectionTool(name))
    return acc
