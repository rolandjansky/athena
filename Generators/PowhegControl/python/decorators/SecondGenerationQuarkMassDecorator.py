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

  def __init__( self, decorated, prefix ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    if prefix == 'lhfm' :
      self.decorated.add_parameter( 'mass_b', ATLASCommonParameters.mass_b, desc='b-quark mass in GeV', parameter='lhfm/bmass' )
      self.decorated.add_parameter( 'mass_c', ATLASCommonParameters.mass_c, desc='c-quark mass in GeV', parameter='lhfm/cmass' )
    elif prefix == 'lhe' :
      self.decorated.add_parameter( 'mass_b', ATLASCommonParameters.mass_b, desc='set b-quark mass in GeV for momentum reshuffling', parameter='bmass_lhe' )
      self.decorated.add_parameter( 'mass_c', ATLASCommonParameters.mass_c, desc='set c-quark mass in GeV for momentum reshuffling', parameter='cmass_lhe' )
