# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl ZeroWidthDecorator
#  Powheg runcard decorator for zero width parameter
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class ZeroWidthDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.zerowidth = 0


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'zerowidth '+str(self.decorated.zerowidth)+' ! (default 0) if 1 use on-shell bosons only \n' )
