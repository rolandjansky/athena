# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl TopMassDecorator
#  Powheg runcard decorator for top mass parameter
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class TopMassDecorator(object) :

  ## Define decorator name string
  name = 'top mass'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'tmass', ATLASCommonParameters.mass_t,   default='{0}', desc='top quark mass' )
    self.decorated.add_parameter( 'topmass', ATLASCommonParameters.mass_t, default='{0}', desc='top quark mass' )
