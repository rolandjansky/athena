# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Factory_BaseClass   import Factory
from HardCoded           import HardCodedProjectRelease
from HardCoded           import HardCodedProjectKit
from HardCoded           import HardCodedBase

class HardCodedFactory(Factory):
  def __init__(self, logger):
    Factory.__init__(self, logger, self.__class__.__name__)
    
    self.dict = {
      ('project', 'build'):         HardCodedProjectRelease,
      ('project', 'kit'):           HardCodedProjectKit,
      ('base', None):               HardCodedBase
      }
  def create(self, releaseType, runType):

    mode = (releaseType, runType)    
    return self.dict[mode]()
 


