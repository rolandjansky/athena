# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl MiNLONLLDecorator
#  Powheg runcard decorator for MiNLO NNLL settings
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class MiNLONLLDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.factsc2min       = 2.0
    self.decorated.frensc2min       = 2.0
    self.decorated.minlo_nnll       = 1
    self.decorated.sudscalevar      = 1


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'factsc2min '+str(self.decorated.factsc2min)+'   ! at this value the factorization scale is frozen (needed with MiNLO)\n' )
      f.write( 'frensc2min '+str(self.decorated.frensc2min)+'   ! at this value the renormalisation scale is frozen (needed with MiNLO)\n' )
      f.write( 'minlo_nnll '+str(self.decorated.minlo_nnll)+'   ! enable MiNLO NNLL\n' )
      f.write( 'sudscalevar '+str(self.decorated.sudscalevar)+' ! (default 1) scale variation also in Sudakov form factors in MiNLO\n' )
