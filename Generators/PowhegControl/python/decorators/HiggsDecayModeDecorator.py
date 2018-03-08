# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl HiggsDecayModeDecorator
#  Powheg runcard decorator for Higgs decay mode
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class HiggsDecayModeDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.hdecaymode = 0


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'hdecaymode '+str(self.decorated.hdecaymode)+' ! Higgs boson decay (-1:no decay; 0:all; 1-6:dd, uu etc.; 7-9:e+e-, etc.; 10:WW; 11:ZZ; 12:gammagamma)\n' )
