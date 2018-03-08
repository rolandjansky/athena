# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl HPlusJetsDecorator
#  Powheg runcard decorator for Higgs+jets properties
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class HPlusJetsDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.bwcutoff = 15


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'bwcutoff '+str(self.decorated.bwcutoff)+' ! Mass window is hmass +- bwcutoff*hwidth\n' )
