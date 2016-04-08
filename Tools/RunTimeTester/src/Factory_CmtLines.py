# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Factory_BaseClass    import Factory
from CmtLines import CmtLinesBuild, CmtLinesLocal, CmtLinesBuildHLT

import sys

class CmtLinesFactory(Factory):
  def __init__(self, logger):
    Factory.__init__(self, logger, self.__class__.__name__)

    self.dict = {
      'build':         CmtLinesBuild,
      'hlt':           CmtLinesBuildHLT,
      'local':         CmtLinesLocal
      }
    
  def create(self, paths, argDict):
    """
    it is overridden by local RTT runs
    """


    mode = paths.runType # now only build
    if paths.localRTTRun: mode = 'local'

    if mode in self.dict.keys():
      if paths.topProject == 'AtlasHLT': mode='hlt'
      cmtl =  self.dict[mode](self.logger, argDict, useRunTime=True)
      self.logger.debug('Creating a %s' % cmtl.__class__.__name__)
      return cmtl
    else:
      msg = 'Unknown mode, could not instantiate a CmtLines: '+str(mode)
      self.logger.info(msg)
      sys.exit(0)
 


