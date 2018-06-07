# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl ZMassWindowDecorator
#  Powheg runcard decorator for Z-mass window
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class ZMassWindowDecorator(object) :

  ## Define decorator name string
  name = 'Z mass window'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'mass_Z_low', 1.0,      default='{0}', desc='M_Z > mass low', parameter='min_z_mass' )
    self.decorated.add_parameter( 'mass_Z_high', 14000.0, default='{0}', desc='M_Z < mass high', parameter='max_z_mass' )
    self.decorated.add_parameter( 'mass_Z_low', 1.0,      default='{0}', desc='M_Z > mass low', parameter='min_Z_mass' )
    self.decorated.add_parameter( 'mass_Z_high', 14000.0, default='{0}', desc='M_Z < mass high', parameter='max_Z_mass')
