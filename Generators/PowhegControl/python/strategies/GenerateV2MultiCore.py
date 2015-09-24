# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl GenerateV2MultiCore
#  Run multiple Powheg v2 processes using a thread for each one
#  - needs multiple stages
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
import subprocess, time
from ..utility import ProcessHandling, RepeatingTimer

##  Run multiple Powheg processes
def generate_v2_multi_core(configurator) :
  # Construct random seeds - increment by 1e6 each time
  configurator.logger.info( 'Running in multicore mode with {0} subjobs'.format(configurator.cores) )
  with open('pwgseeds.dat','wb') as random_seed_list : [ random_seed_list.write( str( configurator.random_seed + int(idx*1e6) )+'\n' ) for idx in range(configurator.cores) ]

  # Remove iseed if providing seeds from pwgseeds.dat
  subprocess.call( 'sed -i "/^iseed/d" powheg*.input', shell=True )
  configurator.logger.debug( 'Disabling iseed variable when multiple seeds are used' )

  # Set up four passes to run each parallelstage
  for idx_stage in range(1,5) :
    configurator.logger.info( 'Now running stage {0}/4'.format(idx_stage) )

    # Set number of xgriditerations to itmx1
    n_xgriditerations = configurator.itmx1
    current_xgriditeration = 0

    # Inner loop to allow multiple xgrid iterations
    while True :
      # For stage 1, we need itmx1 iterations
      if idx_stage == 1 :
        current_xgriditeration += 1
        subprocess.call( 'sed -i "s/xgriditeration.*/xgriditeration {0}/g" powheg.input'.format(current_xgriditeration), shell=True )
        configurator.logger.info( 'Now running xgriditeration {0}/{1}'.format(current_xgriditeration,n_xgriditerations) )

      # Run the multiprocess step and wait for output collector to terminate
      time_start = time.time()
      processes = [ ProcessHandling.SingleProcessThread( configurator._powheg_executable, configurator.logger, seed_index=idx ) for idx in range(1,configurator.cores+1) ]
      manager = ProcessHandling.ProcessManager( processes )
      while manager.monitor( configurator.logger ) : pass

      # Exit inner loop
      if idx_stage != 1 or current_xgriditeration >= n_xgriditerations : break

    # Increment parallelstage and repeat
    subprocess.call( 'sed -i "s/parallelstage {0}/parallelstage {1}/g" powheg.input'.format(idx_stage, idx_stage+1), shell=True )
    configurator.logger.info( 'Finished stage {0}/4 in {1}'.format( idx_stage, RepeatingTimer.human_readable_time_interval(time.time() - time_start)) )
