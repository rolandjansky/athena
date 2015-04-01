# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl RunningScalesDecorator
#  Powheg runcard decorator for running scales
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class RunningScalesDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.runningscales = -1


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'runningscales '+str(self.decorated.runningscales)+' ! (default 0) 0:use hmass as central factorization and renormalisation scale; 1:use the Ht/2\n' )
