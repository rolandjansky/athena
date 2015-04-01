# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl MassWindowDecorator
#  Powheg runcard decorator for mass window restriction
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class MassWindowDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.mass_low  = -1
    self.decorated.mass_high = -1


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'mass_low '+str(self.decorated.mass_low)+'   ! M_V > mass low\n' )
      f.write( 'mass_high '+str(self.decorated.mass_high)+' ! M_V < mass high\n' )
      f.write( 'masswindow_low -1                           ! disabled: use mass low instead\n' )
      f.write( 'masswindow_high -1                          ! disabled: use mass high instead\n' )
