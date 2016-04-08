# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl HiggsMassWindowDecorator
#  Powheg runcard decorator for Higgs mass window settings
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class HiggsMassWindowDecorator(object) :

  ## Define decorator name string
  name = 'Higgs mass window'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'mass_H_low', 10.,    default='{0}', desc='M_H > mass low', parameter='min_h_mass' )
    self.decorated.add_parameter( 'mass_H_high', 1000., default='{0}', desc='M_H < mass high', parameter='max_h_mass' )
