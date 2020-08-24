# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory


class TimeBurnerCfg(CompFactory.TimeBurner):
    def __init__(self, name="TimeBurner", **kwargs):
        super(TimeBurnerCfg, self).__init__(name, **kwargs)
        # Decorate the Configurable with a HypoTools property which is only required
        # by the menu and python configuration framework, but has no use in C++ TimeBurner
        self.HypoTools = []


def TimeBurnerHypoToolGen(chainDict):
    # Dummy HypoTool implementing only the functions used by the menu and python configuration framework
    class DummyHypo:
        def __init__(self, name):
            self.name = name
        def getName(self):
            return self.name

    return DummyHypo(chainDict['chainName'])
