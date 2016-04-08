# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
This module bundles the RTT factory functions.
These functions are written as classes to help write test code.
"""
# -------------------------------------------------------------------------
# Base Class
# -------------------------------------------------------------------------
class Factory:
  def __init__(self, logger, name='Factory'):
    # logger.debug('Init for class %s' % name)
    self.logger = logger
    self.name = name
