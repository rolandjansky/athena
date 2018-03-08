# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl SemileptonicDecorator
#  Powheg runcard decorator for semileptonic decays
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class SemileptonicDecorator(object) :

  ## Define decorator name string
  name = 'semileptonic'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'semileptonic', -1, default='{0}', desc='(-1:use Powheg default) only accept semileptonic decays' )
