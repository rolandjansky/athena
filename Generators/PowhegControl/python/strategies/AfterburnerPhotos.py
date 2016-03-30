# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl AfterburnerPhotos
#  Apply PHOTOS afterburner to pre-generated Powheg events
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..utility import ProcessHandling


## Run PHOTOS as an afterburner to existing events
def afterburner_PHOTOS(configurator) :
  configurator.logger.info( 'Running PHOTOS afterburner' )
  processes = [ ProcessHandling.SingleProcessThread( configurator._PHOTOS_executable, configurator.logger ) ]
  manager = ProcessHandling.ProcessManager( processes )
  while manager.monitor( configurator.logger ) : pass
