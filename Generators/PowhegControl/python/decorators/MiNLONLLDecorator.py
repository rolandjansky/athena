# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl MiNLONLLDecorator
#  Powheg runcard decorator for MiNLO NNLL settings
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class MiNLONLLDecorator(object) :

  ## Define decorator name string
  name = 'MiNLO NNLL'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'factsc2min', 2.0, default='{0}', desc='value at which the factorization scale is frozen (needed with MiNLO)' )
    self.decorated.add_parameter( 'frensc2min', 2.0, default='{0}', desc='value at which the renormalisation scale is frozen (needed with MiNLO)' )
    self.decorated.add_parameter( 'minlo_nnll', 1,   default='{0}', desc='(0:disabled; 1:enabled) enable MiNLO NNLL' )
    self.decorated.add_parameter( 'sudscalevar', 1,  default='{0}', desc='(0:disabled; 1:enabled) scale variation also in Sudakov form factors in MiNLO' )
