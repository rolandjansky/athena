# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Run multiple Powheg processes with one pass through each one
#
#   Authors: James Robinson  <james.robinson@cern.ch>
#
#########################################################################################################################

#! /usr/bin/env python
from PowhegSingleThread import *
from OutputHandler import write_output

###############################################################################
#
#  Register multiple Powheg processes
#
###############################################################################
def runPowhegV1Multicore(configurator) :
  configurator.logger.info( 'Running in multicore mode with {0} subjobs'.format(configurator.cores) )
  # Initialise temporary files to hold the seed indices, since providing stdin while reading stdout is tricky - increment by 1e6 each time
  with open('pwgseeds.dat','wb') as random_seed_list : [ random_seed_list.write( str( configurator.random_seed + int(idx*1e6) )+'\n' ) for idx in range(configurator.cores) ]
  for idx in range(configurator.cores) :
    with open('seed_index{0}.input'.format(idx),'wb') as random_seed_index : random_seed_index.write( str(int(idx)) )
  random_seed_indices = [ open('seed_index{0}.input'.format(idx),'rb') for idx in range(configurator.cores) ]
  [ runPowhegSingleThread( configurator, stdin=random_seed_indices[idx], output_prefix='Seed #{0}: '.format(idx), display_output=False ) for idx in range(configurator.cores) ]
  write_output( configurator.running_processes, configurator.logger )
