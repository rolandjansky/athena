# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

""" Derive from the offline class and override InDetFlags
"""

__author__ = "J. Masik"
__version__= "$Revision$"
__doc__    = "ConfiguredVertexingTrigCuts"

from InDetRecExample.ConfiguredVertexingCuts import ConfiguredVertexingCuts

class ConfiguredVertexingTrigCuts(ConfiguredVertexingCuts):
  def __set_indetflags(self):
    from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
    self.__indetflags = InDetTrigFlags

EFIDVertexingCuts = ConfiguredVertexingTrigCuts("PileUp")
