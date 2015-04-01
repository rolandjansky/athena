# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl WMassWindowDecorator
#  Powheg runcard decorator for W-mass window
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class WMassWindowDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.mass_W_low  = 1.0
    self.decorated.mass_W_high = 14000.0


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'max_w_mass '+str(self.decorated.mass_W_high)+' ! M_W < mass high\n')
      f.write( 'min_w_mass '+str(self.decorated.mass_W_low)+'  ! M_W > mass low\n')
      f.write( 'max_W_mass '+str(self.decorated.mass_W_high)+' ! M_W < mass high\n')
      f.write( 'min_W_mass '+str(self.decorated.mass_W_low)+'  ! M_W > mass low\n')
