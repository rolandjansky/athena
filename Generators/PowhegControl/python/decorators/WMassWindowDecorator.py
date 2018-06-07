# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl WMassWindowDecorator
#  Powheg runcard decorator for W-mass window
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class WMassWindowDecorator(object) :

  ## Define decorator name string
  name = 'W mass window'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'mass_W_low', 1.0,      default='{0}', desc='M_W > mass low', parameter='min_w_mass' )
    self.decorated.add_parameter( 'mass_W_high', 14000.0, default='{0}', desc='M_W < mass high', parameter='max_w_mass' )
    self.decorated.add_parameter( 'mass_W_low', 1.0,      default='{0}', desc='M_W > mass low', parameter='min_W_mass' )
    self.decorated.add_parameter( 'mass_W_high', 14000.0, default='{0}', desc='M_W < mass high', parameter='max_W_mass')
