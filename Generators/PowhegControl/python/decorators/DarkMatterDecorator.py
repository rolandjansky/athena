# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl DarkMatterDecorator
#  Powheg runcard decorator for dark matter settings
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class DarkMatterDecorator(object) :

  ## Define decorator name string
  name = 'dark matter'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'DM_mass', 10,   desc='(default, 10) DM-candidate mass', parameter='DMmass' )
    self.decorated.add_parameter( 'gDM', 1.0,      desc='(default, 1.0) X-Xbar-med coupling', parameter='DMgDM' )
    self.decorated.add_parameter( 'gSM', 1.0,      desc='(default, 1.0) q-q-med coupling', parameter='DMgSM' )
    self.decorated.fix_parameter( 'idDM', 1000022, desc='(default, 1000022) id of DM particles' )
