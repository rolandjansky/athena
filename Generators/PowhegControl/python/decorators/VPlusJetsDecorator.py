# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl VPlusJetsDecorator
#  Powheg runcard decorator for ISR/FSR V+jets parameters
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class VPlusJetsDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.par_fsrtinycsi = 5e-6
    self.decorated.par_fsrtinyy   = 5e-6
    self.decorated.par_isrtinycsi = 5e-6
    self.decorated.par_isrtinyy   = 5e-6


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'par_fsrtinycsi '+str(self.decorated.par_fsrtinycsi)+' ! \n' )
      f.write( 'par_fsrtinyy '+str(self.decorated.par_fsrtinyy)+'     ! \n' )
      f.write( 'par_isrtinycsi '+str(self.decorated.par_isrtinycsi)+' ! \n' )
      f.write( 'par_isrtinyy '+str(self.decorated.par_isrtinyy)+'     ! \n' )

