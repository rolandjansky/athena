# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl DarkMatterPhiDecorator
#  Powheg runcard decorator for non-vector dark matter
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class DarkMatterPhiDecorator(object) :

  ## Define decorator name string
  name = 'dark matter phi'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'decaymode', -1,     desc='[DMS] 1:S-mediated; 2:PS-mediated fermionic DM; -1,-2 for full theory (mediator mass and width needed); [DMGG] 1:GG-mediated fermionic DM; -1:full theory (mediator mass and width needed)' )
    self.decorated.add_parameter( 'phi_mass', 100,     desc='mediator mass', parameter='DMphimass' )
    self.decorated.add_parameter( 'phi_width', 42.494, desc='mediator width', parameter='DMphiwidth' )
