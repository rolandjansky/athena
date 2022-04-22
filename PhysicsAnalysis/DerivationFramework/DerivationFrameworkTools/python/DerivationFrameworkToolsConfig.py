# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#==============================================================================
# Provides configs for the common derivation framework tools
#==============================================================================

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# Wrapper to allow ASG tools to decorste via the derivation framework
def AsgSelectionToolWrapperCfg(ConfigFlags, name, **kwargs):
    """Configure the ASG selection tool wrapper"""
    acc = ComponentAccumulator()
    AsgSelectionToolWrapper = CompFactory.DerivationFramework.AsgSelectionToolWrapper
    acc.addPublicTool(AsgSelectionToolWrapper(name, **kwargs),
                      primary = True)
    return acc   

# Generic thinning tool (via ExpressionEvaluation strings)
def GenericObjectThinningCfg(ConfigFlags, name, **kwargs):
    """Configure the generic object thinning tool"""
    acc = ComponentAccumulator()
    GenericObjectThinning = CompFactory.DerivationFramework.GenericObjectThinning
    acc.addPublicTool(GenericObjectThinning(name, **kwargs),
                      primary = True)
    return acc

