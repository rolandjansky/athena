# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl ISRFSRDecorator
#  Powheg runcard decorator for ISR/FSR parameters
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class ISRFSRDecorator(object) :

  ## Define decorator name string
  name = 'ISR/FSR'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'par_fsrtinycsi', -1, desc='(-1:use Powheg default)' )
    self.decorated.add_parameter( 'par_fsrtinyy', -1,   desc='(-1:use Powheg default)' )
    self.decorated.add_parameter( 'par_isrtinycsi', -1, desc='(-1:use Powheg default)' )
    self.decorated.add_parameter( 'par_isrtinyy', -1,   desc='(-1:use Powheg default)' )
