# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Run multiple Powheg processes with multiple passes through each one
#
#   Authors: James Robinson  <james.robinson@cern.ch>
#
#########################################################################################################################

#! /usr/bin/env python
import subprocess, time
from PowhegSingleThread import *
from OutputHandler import write_output
from ..utility import RepeatingTimer

###############################################################################
#
#  Register multiple Powheg processes
#
###############################################################################
def runPowhegV2Multicore(configurator) :
  configurator.logger.info( 'Running in multicore mode with {0} subjobs'.format(configurator.cores) )
  # Initialise temporary files to hold the seed indices, since providing stdin while reading stdout is tricky - increment by 1e6 each time
  with open('pwgseeds.dat','wb') as random_seed_list : [ random_seed_list.write( str( configurator.random_seed + int(idx*1e6) )+'\n' ) for idx in range(configurator.cores) ]
  for idx in range(configurator.cores) :
    with open('seed_index{0}.input'.format(idx),'wb') as random_seed_index : random_seed_index.write( str(int(idx)) )

  # Set up four passes to run each parallelstage
  for idx_stage in range(1,5) :
    random_seed_indices = [ open('seed_index{0}.input'.format(idx),'rb') for idx in range(configurator.cores) ]
    configurator.logger.info( 'Now running stage {0}/4'.format(idx_stage) )

    # Run the multiprocess step and wait for output collector to terminate
    time_start = time.time()
    [ runPowhegSingleThread( configurator, stdin=random_seed_indices[idx], output_prefix='Seed #{0}: '.format(idx), display_output=False ) for idx in range(configurator.cores) ]
    write_output( configurator.running_processes, configurator.logger )

    # Increment parallelstage and repeat
    subprocess.call( 'sed -i "s/parallelstage {0}/parallelstage {1}/g" powheg.input'.format(idx_stage, idx_stage+1), shell=True ) # NB. shell=True is unsafe if combined with user input
    [ random_seed_index.close() for random_seed_index in random_seed_indices ]
    configurator.logger.info( 'Finished stage {0}/4 in {1}'.format( idx_stage, RepeatingTimer.human_readable_time_interval(time.time() - time_start)) )

