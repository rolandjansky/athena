# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl WIDDecorator
#  Powheg runcard decorator for W-boson ID
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class WIDDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.idvecbos    = -24


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'idvecbos '+str(self.decorated.idvecbos)+' ! PDG code for vector boson to be produced (W:24)\n' )
