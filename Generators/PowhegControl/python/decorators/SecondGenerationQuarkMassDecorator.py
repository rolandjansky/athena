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

    self.decorated.fix_parameter( 'bmass_lhe', ATLASCommonParameters.mass_b, desc='(default ATLAS) Set b-quark mass for momentum reshuffling' )
    self.decorated.fix_parameter( 'cmass_lhe', ATLASCommonParameters.mass_c, desc='(default ATLAS) Set c-quark mass for momentum reshuffling' )
