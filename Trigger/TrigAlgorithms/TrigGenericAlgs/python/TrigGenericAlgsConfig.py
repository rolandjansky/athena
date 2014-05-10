# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigGenericAlgs.TrigGenericAlgsConf import OverlapRemoval

class OverlapRemovalConfig(OverlapRemoval):
    def __init__(self, name="OverlapRemovalDefault", **kwargs):
        super(OverlapRemovalConfig , self ).__init__(name)
        self.SuccessfulCombinationsLimit = 1
        for n,v in kwargs.items():
            setattr(self, n, v)
    
