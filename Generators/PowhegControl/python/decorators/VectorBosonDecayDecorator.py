# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl VectorBosonDecayDecorator
#  Powheg runcard decorator for vector boson decays
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class VectorBosonDecayDecorator(object) :

  ## Define decorator name string
  name = 'vector boson decay'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'vdecaymode', 1, default='{0}', desc='(1:e, 2:mu, 3: tau). (DM 1:V-mediated fermionic DM; 2:A-mediated fermionic DM;-1, -2 for full theory)' )
