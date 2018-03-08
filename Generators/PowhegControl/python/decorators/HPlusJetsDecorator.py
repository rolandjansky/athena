# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl HPlusJetsDecorator
#  Powheg runcard decorator for Higgs+jets properties
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class HPlusJetsDecorator(object) :

  ## Define decorator name string
  name = 'H+jets'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'bwcutoff', 15, default='{0}', desc='Mass window is hmass +/- bwcutoff * hwidth' )
