# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl VectorBosonDecayDecorator
#  Powheg runcard decorator for vector boson decays
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class VectorBosonDecayDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.vdecaymode = 1


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'vdecaymode '+str(self.decorated.vdecaymode)+' ! Normal      -> 1:leptonic decay, 2:muonic decay, 3: tauonic decay,...\n' )
      f.write( '                                              ! Dark matter -> 1:V-mediated fermionic DM; 2:A-mediated fermionic DM;-1, -2 for full theory (mediator mass and width needed)\n' )
