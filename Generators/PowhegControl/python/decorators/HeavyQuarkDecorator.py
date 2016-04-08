# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl HeavyQuarkDecorator
#  Powheg runcard decorator for heavy quarks
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class HeavyQuarkDecorator(object) :

  ## Define decorator name string
  name = 'heavy quark'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'quark_mass', -1, default='{0}', desc='mass of heavy quark in GeV', parameter='qmass' )
