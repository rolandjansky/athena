# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl ZMassWindowDecorator
#  Powheg runcard decorator for Z-mass window
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class ZMassWindowDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.mass_Z_low  = 1.0
    self.decorated.mass_Z_high = 14000.0


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'max_z_mass '+str(self.decorated.mass_Z_high)+' ! M_Z < mass high\n')
      f.write( 'min_z_mass '+str(self.decorated.mass_Z_low)+'  ! M_Z > mass low\n')
      f.write( 'max_Z_mass '+str(self.decorated.mass_Z_high)+' ! M_Z < mass high\n')
      f.write( 'min_Z_mass '+str(self.decorated.mass_Z_low)+'  ! M_Z > mass low\n')
