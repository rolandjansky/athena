# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl RunningScaleDecorator
#  Powheg runcard decorator for running scale
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class RunningScaleDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.runningscale = 1


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'runningscale '+str(self.decorated.runningscale)+' ! Normal -> choice for renormalisation and factorisation scales in Bbar integration\n' )
      f.write( '                                                  ! Dark matter -> (default 3) 0, 1, 2, 3 correspond to 2mX (fixed), ptj, XXbar invmass, Ht/2\n' )
