# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def SkimmingToolHIGG1Cfg(ConfigFlags, **kwargs):

    """ Skimming Tool for HIGG1 derivations"""

    kwargs.setdefault("RequireGRL", True)
    kwargs.setdefault("ReqireLArError", True)
    kwargs.setdefault("RequireTrigger", False)
    kwargs.setdefault("RequirePreselection",False)
    kwargs.setdefault("RequireKinematic", False)
    kwargs.setdefault("RequireQuality", False)
    kwargs.setdefault("RequireIsolation", False)
    kwargs.setdefault("RequireInvariantMass", False)
    kwargs.setdefault("Triggers" , [])
    kwargs.setdefault("IncludeSingleElectronPreselection", False)
    kwargs.setdefault("IncludeDoubleElectronPreselection", False)
    kwargs.setdefault("IncludeSingleMuonPreselection", False)

    acc = ComponentAccumulator() 

    acc.setPrivateTools( CompFactory.DerivationFramework.SkimmingToolHIGG1(**kwargs) ) 

    return acc