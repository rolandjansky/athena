# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def JetCleaningToolCfg(ConfigFlags, name, jetdef, cleaningLevel, useDecorations):
    """Configure the jet cleaning tool"""
    acc = ComponentAccumulator()
    JetCleaningTool = CompFactory.JetCleaningTool
    acc.setPrivateTools(JetCleaningTool(name, CutLevel=cleaningLevel, JetContainer=jetdef, UseDecorations=useDecorations))
    return acc


def EventCleaningToolCfg(ConfigFlags, name, cleaningLevel):
    """Configure the event cleaning tool"""
    acc = ComponentAccumulator()
    EventCleaningTool = CompFactory.ECUtils.EventCleaningTool
    acc.setPrivateTools(EventCleaningTool(name, CleaningLevel=cleaningLevel))
    return acc
                        
