# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl TopDecayModeDecorator
#  Powheg runcard decorator for top decay parameter
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class TopDecayModeDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.topdecaymode = 0


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'topdecaymode '+str(self.decorated.topdecaymode)+' ! an integer of 5 digits that are either 0, 1 or 2, representing\n' )
      f.write( '                                                  !  the maximum number of the following particles(antiparticles)\n' )
      f.write( '                                                  !  in the final state: e mu tau up charm\n' )
