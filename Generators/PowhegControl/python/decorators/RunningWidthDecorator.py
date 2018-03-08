# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl RunningWidthDecorator
#  Powheg runcard decorator for running width
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
class RunningWidthDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.runningwidth = 1


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'runningwidth '+str(self.decorated.runningwidth)+' ! (default 0) if 1 using running width\n' )
