# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl MassWindowDecorator
#  Powheg runcard decorator for mass window restriction
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class MassWindowDecorator(object) :

  ## Define decorator name string
  name = 'mass window'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'mass_low', -1,        desc='(-1:use Powheg default) If set then require M_object > mass_low; otherwise allow internal Powheg computation.' )
    self.decorated.add_parameter( 'mass_high', -1,       desc='(-1:use Powheg default) If set then require M_object < mass_high; otherwise allow internal Powheg computation.' )
    self.decorated.add_parameter( 'masswindow_low', -1,  desc='DEPRECATED use mass_low instead' )
    self.decorated.add_parameter( 'masswindow_high', -1, desc='DEPRECATED use mass_high instead' )
