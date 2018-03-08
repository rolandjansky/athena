# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl ZeroWidthDecorator
#  Powheg runcard decorator for zero width parameter
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class ZeroWidthDecorator(object) :

  ## Define decorator name string
  name = 'zero width'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'zerowidth', 0, desc='(0:disabled; 1:enabled) use on-shell quarks/bosons only' )
