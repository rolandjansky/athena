# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#####################
## OverlayConfig.py
#####################

import os
__all__ = []

from PyJobTransformsCore.TransformConfig import *

import PyJobTransformsCore.basic_trfarg as trfarg

# define configuration properties
class OverlayConfig(TransformConfig):
   # prevent any mistypings by not allowing dynamic members
   __slots__ = ()

   def __init__(self,name='overlayConfig',metaData=None):
     if metaData is None: metaData = []
     TransformConfig.__init__(self,name,metaData)
     self.maxeventsstrategy = 'INPUTEVENTS'

# make configuration object
overlayConfig = OverlayConfig()

# EOF
##########################
                                    
