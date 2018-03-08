# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl GenerateV1MultiCore
#  Run multiple Powheg v1 processes using a thread for each one
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..utility import FileParser, ProcessHandling

## Run multiple Powheg processes
def generate_v1_multi_core(configurator) :
  # Construct random seeds - increment by 1e6 each time
  configurator.logger.info( 'Running in multicore mode with {0} subjobs'.format(configurator.cores) )
  with open('pwgseeds.dat','wb') as random_seed_list : [ random_seed_list.write( str( configurator.random_seed + int(idx*1e6) )+'\n' ) for idx in range(configurator.cores) ]

  # Remove iseed if providing seeds from pwgseeds.dat
  FileParser('powheg.input').text_remove( '^iseed' )
  configurator.logger.debug( 'Disabling iseed variable when multiple seeds are used' )

  # Run in multicore mode
  processes = [ ProcessHandling.SingleProcessThread( configurator._powheg_executable, configurator.logger, seed_index=idx ) for idx in range(1,configurator.cores+1) ]
  manager = ProcessHandling.ProcessManager( processes )
  while manager.monitor( configurator.logger ) : pass
