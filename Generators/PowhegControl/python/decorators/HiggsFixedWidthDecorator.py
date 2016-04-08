# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl HiggsFixedWidthDecorator
#  Powheg runcard decorator for fixed/running Higgs width parameter
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class HiggsFixedWidthDecorator(object) :

  ## Define decorator name string
  name = 'Higgs fixed width'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'higgsfixedwidth', 0, default='{0}', desc='(0:running width; 1:fixed width in Breit-Wigner)' )
