# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl DarkMatterPhiDecorator
#  Powheg runcard decorator for non-vector dark matter
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class DarkMatterPhiDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.decaymode = -1
    self.decorated.phi_mass  = 100
    self.decorated.phi_width = 42.494


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'decaymode '+str(self.decorated.decaymode)+'  ! DMS  -> (default 1) 1:S-mediated; 2:PS-mediated fermionic DM;-1, -2 for full theory (mediator mass and width needed)\n' )
      f.write( '                                             ! DMGG -> (default 1) 1:GG-mediated fermionic DM;-1:full theory (mediator mass and width needed)\n' )
      f.write( 'DMphimass '+str(self.decorated.phi_mass)+'   ! mediator mass\n' )
      f.write( 'DMphiwidth '+str(self.decorated.phi_width)+' ! mediator width\n' )

