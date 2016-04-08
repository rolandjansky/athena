# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Factory_BaseClass   import Factory
from Legals              import LegalsProject
from Legals              import LegalsBase
from Tools2              import releaseNumberComponents

import sys

class LegalsFactory(Factory):
  def __init__(self, logger):
    Factory.__init__(self, logger, self.__class__.__name__)
    
  def create(self, releaseType, runType, hardCoded):
    return LegalsProject(hardCoded)
  
