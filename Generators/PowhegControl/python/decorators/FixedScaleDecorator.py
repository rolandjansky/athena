# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl FixedScaleDecorator
#  Powheg runcard decorator for fixed scale
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class FixedScaleDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.fixedscale = -1


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'fixedscale '+str(self.decorated.fixedscale)+' ! use reference renormalisation and factorisation scales \n' )
