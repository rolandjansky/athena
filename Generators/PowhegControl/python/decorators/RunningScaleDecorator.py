# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl RunningScaleDecorator
#  Powheg runcard decorator for running scale
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class RunningScaleDecorator(object) :

  ## Define decorator name string
  name = 'running scale'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'runningscale', 1, default='{0}', desc='Choice for mu_R and mu_F in Bbar integration. For DM 0, 1, 2, 3 correspond to 2mX (fixed), ptj, XXbar invmass, Ht/2' )
