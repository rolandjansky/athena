# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl SecondGenerationQuarkMassDecorator
#  Powheg runcard decorator for second generation quark masses
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class SecondGenerationQuarkMassDecorator(object) :

  ## Define decorator name string
  name = 'second generation quark mass'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'bmass_lhe', ATLASCommonParameters.mass_b, default='{0}', desc='set b-quark mass for momentum reshuffling in GeV' )
    self.decorated.add_parameter( 'cmass_lhe', ATLASCommonParameters.mass_c, default='{0}', desc='set c-quark mass for momentum reshuffling in GeV' )
