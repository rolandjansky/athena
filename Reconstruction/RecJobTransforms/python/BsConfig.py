# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
__all__ = []

from PyJobTransformsCore.TransformConfig import *

class BsConfig(TransformConfig):
    # prevent any mistypings by not allowing dynamic members
    __slots__ = ()

    # add properties

    def __init__(self,name='bsConfig',metaData=None):
        TransformConfig.__init__(self,name,metaData)
        self.maxeventsstrategy = 'INPUTEVENTS'


# make configuration object
bsConfig = BsConfig()
