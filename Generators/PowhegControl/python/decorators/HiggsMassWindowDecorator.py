# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl HiggsMassWindowDecorator
#  Powheg runcard decorator for Higgs mass window settings
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class HiggsMassWindowDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.mass_H_low    = 10.
    self.decorated.mass_H_high   = 1000.


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'max_h_mass '+str(self.decorated.mass_H_high)+' ! M_H < mass high\n' )
      f.write( 'min_h_mass '+str(self.decorated.mass_H_low)+'  ! M_H > mass low\n' )
