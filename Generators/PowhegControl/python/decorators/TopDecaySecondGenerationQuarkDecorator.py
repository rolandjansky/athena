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

    self.decorated.fix_parameter( 'lhfm/bmass', ATLASCommonParameters.mass_b, desc='(default ATLAS) b quark mass' )
    self.decorated.fix_parameter( 'lhfm/cmass', ATLASCommonParameters.mass_c, desc='(default ATLAS) c quark mass' )
