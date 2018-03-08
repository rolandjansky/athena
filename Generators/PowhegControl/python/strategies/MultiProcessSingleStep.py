# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to run multiple Powheg processes with one pass through each one
#
#   Authors: James Robinson  <james.robinson@cern.ch>
#
#########################################################################################################################

#! /usr/bin/env python
from SingleProcessSingleStep import *

###############################################################################
#
#  Register multiple Powheg processes
#
###############################################################################
def runMultiProcessSingleStep(configurator) :
  configurator._logger.info( 'Running in multicore mode with {0} subjobs'.format(configurator._n_cores) )
  # Initialise temporary files to hold the seed indices, since providing stdin while reading stdout is tricky - increment by 1e6 each time
  with open('pwgseeds.dat','wb') as random_seed_list : [ random_seed_list.write( str( configurator.randomSeed + int(idx*1e6) )+'\n' ) for idx in range(configurator._n_cores) ]
  for idx in range(configurator._n_cores) :
    with open('seed_index{0}.input'.format(idx),'wb') as random_seed_index : random_seed_index.write( str(int(idx)) )
  random_seed_indices = [ open('seed_index{0}.input'.format(idx),'rb') for idx in range(configurator._n_cores) ]
  [ runSingleProcessSingleStep( configurator, stdin=random_seed_indices[idx], output_prefix='Seed #{0}: '.format(idx), display_output=False ) for idx in range(configurator._n_cores) ]
  displayProcessOutput(configurator)
