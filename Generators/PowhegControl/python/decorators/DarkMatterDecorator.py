# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl DarkMatterDecorator
#  Powheg runcard decorator for dark matter settings
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class DarkMatterDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.DM_mass = 10
    self.decorated.gDM     = 1.0
    self.decorated.gSM     = 1.0


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'DMgDM '+str(self.decorated.gDM)+'      ! X-Xbar-med coupling\n' )
      f.write( 'DMgSM '+str(self.decorated.gSM)+'      ! q-q-med coupling\n' )
      f.write( 'DMmass '+str(self.decorated.DM_mass)+' ! DM-candidate mass \n' )
      f.write( 'idDM 1000022                           ! id of DM particles; (+- 19 is the default, but might not always work; Using the id of neutralinos is recommended)\n' )
