# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
This module bundles the RTT factory functions.
These functions are written as classes to help write test code.
"""


from Factory_BaseClass     import Factory
from BaseDirectoryMaker   import BaseDirectoryMaker, CERNSLC4BaseDirectoryMaker    
class BaseDirectoryMakerFactory(Factory):
  def __init__(self, logger):
    Factory.__init__(self, logger, self.__class__.__name__)
    
    self.dict = {
      'CERNslc4':  CERNSLC4BaseDirectoryMaker
      }
    
  def create(self, paths): 
    key = paths.site
    if key in self.dict:
        return self.dict[key](paths,self.logger)
    else:
        return BaseDirectoryMaker(paths, self.logger)
