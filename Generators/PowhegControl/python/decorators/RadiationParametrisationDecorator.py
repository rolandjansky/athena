# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl RadiationParametrisationDecorator
#  Powheg runcard decorator for ISR/FSR phase space parameters
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class RadiationParametrisationDecorator(object) :

  ## Define decorator name string
  name = 'radiation parametrisation'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'fullphsp', -1,      default='{0}', desc='(-1:use Powheg default) enable ISR/FSR phase space parametrization' )
    self.decorated.add_parameter( 'raisingscales', -1, default='{0}', desc='(-1:use Powheg default)' )
