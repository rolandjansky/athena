# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl FakeVirtualDecorator
#  Powheg runcard decorator for faking virtual contributions
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class FakeVirtualDecorator(object) :

  ## Define decorator name string
  name = 'fake virtual'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'fakevirt', 0, desc='(0:disabled; 1:enabled) use Born for virtuals - DO NOT USE when generating events.' )
