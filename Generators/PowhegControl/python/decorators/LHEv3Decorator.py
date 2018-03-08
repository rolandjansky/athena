# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl LHEv3Decorator
#  Powheg runcard decorator for LHEv3 weights
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class LHEv3Decorator(object) :

  ## Define decorator name string
  name = 'LHEv3'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.fix_parameter( 'compute_rwgt', 0,   default='{0}', desc='(0:nominal; 1:compute reweighting factor)' )
    self.decorated.fix_parameter( 'storeinfo_rwgt', 0, default='{0}', desc='(0:disabled; 1:enabled) enable new-style PDF information' )
