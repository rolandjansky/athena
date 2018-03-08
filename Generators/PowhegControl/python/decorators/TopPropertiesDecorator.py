# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl TopPropertiesDecorator
#  Powheg runcard decorator for top properties
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class TopPropertiesDecorator(object) :

  ## Define decorator name string
  name = 'top properties'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'tmass', ATLASCommonParameters.mass_t,     desc='mass of top quark in GeV' )
    self.decorated.add_parameter( 'topmass', ATLASCommonParameters.mass_t,   desc='mass of top quark in GeV' )
    self.decorated.add_parameter( 'topwidth', ATLASCommonParameters.width_t, desc='top width' )
    self.decorated.add_parameter( 'ttype', 1,                                desc='(1:t; -1:tbar)' )
