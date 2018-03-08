# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl DibosonDecorator
#  Powheg runcard decorator for diboson settings
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class DibosonDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.allowed_decay_modes = []
    self.decorated.decay_mode          = None
    self.decorated.dronly              = 0


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'dronly '+str(self.decorated.dronly)+' ! (default 0) if 1 include only double resonant diagrams \n' )
      ## Add all decay modes
      if self.decorated.decay_mode is not None :
        if self.decorated.decay_mode in self.decorated.allowed_decay_modes :
          for decay_mode in self.decorated.allowed_decay_modes :
            f.write( decay_mode+' {0}                 ! Diboson decay mode\n'.format([-1,1][decay_mode==self.decorated.decay_mode]) )
        else :
          self.decorated.logger().warning( 'Decay mode {0} not recognised!'.format( self.decorated.decay_mode) )
