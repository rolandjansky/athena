# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl HiggsFixedWidthDecorator
#  Powheg runcard decorator for fixed/running Higgs width parameter
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class HiggsFixedWidthDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.higgsfixedwidth = 0


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'higgsfixedwidth '+str(self.decorated.higgsfixedwidth)+' (default 0) 0:running width; 1:standard, fixed width in Breit-Wigner\n' )
