# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl CKKWDecorator
#  Powheg runcard decorator for CKKW scalup
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class CKKWDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.ckkwscalup = 1


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'ckkwscalup '+str(self.decorated.ckkwscalup)+' ! 0: do not use the CKKW scalup, use the normal Powheg one (default)\n')
      f.write( '                                              ! 1: compute the scalup scale for subsequent shower using the smallest kt in the final state\n' )
