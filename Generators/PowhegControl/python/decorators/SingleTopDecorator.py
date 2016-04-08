# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl SingleTopDecorator
#  Powheg runcard decorator for single top production
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class SingleTopDecorator(object) :

  ## Define decorator name string
  name = 'single top'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'alphaem_inv', 1.0/float(ATLASCommonParameters.alphaem), default='{0}', desc='EM coupling' )
    self.decorated.add_parameter( 'ttype', 1,                                              default='{0}', desc='(1:t; -1:tbar)' )
    self.decorated.add_parameter( 'wmass', ATLASCommonParameters.mass_W,                   default='{0}', desc='mass of W boson in GeV' )
    self.decorated.add_parameter( 'wwidth', ATLASCommonParameters.width_W,                 default='{0}', desc='W width' )
