#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def IsolationSelectionToolCfg(name, **kwargs):
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.CP.IsolationSelectionTool("IsolationSelectionTool", **kwargs))
    return acc


def MuonPhysValIsolationSelCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("MuonWP", "PflowTight_FixedRad")
    tool = acc.popToolsAndMerge(IsolationSelectionToolCfg("IsolationSelectionTool", **kwargs))
    acc.setPrivateTools(tool)
    return acc