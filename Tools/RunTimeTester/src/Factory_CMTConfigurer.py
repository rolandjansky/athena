# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys

from Factory_BaseClass import Factory
from CMTConfigurer     import CMTConfigurerBuild
from CMTConfigurer     import CMTConfigurerLocal

class CMTConfigurerFactory(Factory):
  def __init__(self, logger):
    Factory.__init__(self, logger, self.__class__.__name__)

    self.dict = {
      'afs_build'     : CMTConfigurerBuild,
      'localPackages' : CMTConfigurerLocal
      }

  def makeKey(self, paths):    
    if paths.localRTTRun:     return 'localPackages'
    return 'afs_build'
    
  def create(self, paths, configArgDict): 
    key = self.makeKey(paths)

    configurer =  self.dict[key](configArgDict, self.logger)

    self.logger.debug('Instantiated a CMTConfigurer of type %s' % (configurer.__class__.__name__))

    return configurer
  


