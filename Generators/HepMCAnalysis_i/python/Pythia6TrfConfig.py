# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__all__ = []

from PyJobTransformsCore.TransformConfig import TransformConfig


class Pythia6TrfConfig(TransformConfig):
    
    # prevent any mistypings by not allowing dynamic members
  
    __slots__ = ()
  

    # add properties
  
    def __init__(self,name='Pythia6TrfConfig',metaData=None):
        
        TransformConfig.__init__(self,name,metaData)
        self.maxeventsstrategy = 'INPUTEVENTS'
  


# make configuration object

pythia6config = Pythia6TrfConfig()
