# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Factory_BaseClass          import Factory

from UserStuffRetriever  import UserStuffRetrieverLocal
from UserStuffRetriever  import UserStuffRetrieverNonLocal

import sys

class UserStuffRetrieverFactory(Factory):
  def __init__(self, logger):
    Factory.__init__(self, logger, self.__class__.__name__)
    
    #'monolithic':    UserStuffRetriever,
    self.dict = {
                 'release' :      UserStuffRetrieverNonLocal,
                 'dev' :          UserStuffRetrieverNonLocal,
                 'bugfix' :       UserStuffRetrieverNonLocal,
                 'val':           UserStuffRetrieverNonLocal,
                 'localPackages': UserStuffRetrieverLocal
                 }

  def makeKey(self, paths):
    
    if paths.localRTTRun:                     return 'localPackages'
    if paths.release in ['11.3.0', '11.4.0']: return '11301140'
    if paths.branch=='dev':                   return 'dev'
    if paths.branch=='bugfix':                return 'bugfix'
    if paths.branch=='val':                   return 'val'

    return 'release'

  def create(self, paths, argDict): 

    key = self.makeKey(paths)
        
    if key not in self.dict.keys():
      msg  = 'Unknown key, could not instantiate a UserStuffRetriever: '
      msg += str(key)
      self.logger.error(msg)
      sys.exit(0)


    usr =  self.dict[key](argDict)

    self.logger.debug('Instantiated a USR of type %s' % usr.__class__.__name__)

    return usr
  


