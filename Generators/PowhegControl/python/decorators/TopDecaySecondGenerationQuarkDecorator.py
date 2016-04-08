# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl TopDecaySecondGenerationQuarkDecorator
#  Powheg runcard decorator for second generation quark masses from top decays
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class TopDecaySecondGenerationQuarkDecorator(object) :

  ## Define decorator name string
  name = 'top decay second generation quark'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'lhfm/bmass', ATLASCommonParameters.mass_b, default='{0}', desc='b-quark mass' )
    self.decorated.add_parameter( 'lhfm/cmass', ATLASCommonParameters.mass_c, default='{0}', desc='c-quark mass' )
