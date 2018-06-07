# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl DibosonInterferenceDecorator
#  Powheg runcard decorator for diboson interference#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class DibosonInterferenceDecorator(object) :

  ## Define decorator name string
  name = 'diboson interference'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'mllmin', 4,           default='{0}', desc='Minimum invariant mass of lepton pairs from Z decay' )
    self.decorated.add_parameter( 'withinterference', 1, default='{0}', desc='(0:disabled; 1:enabled) include interference for like flavour charged leptons' )
