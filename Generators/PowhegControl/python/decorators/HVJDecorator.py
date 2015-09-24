# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_bb
#  Powheg runcard decorator for H+V+jet
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class HVJDecorator(object) :

  ## Define decorator name string
  name = 'Higgs+V+jet'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'bmass', ATLASCommonParameters.mass_b, desc='(default ATLAS) b quark mass' )
    self.decorated.add_parameter( 'kappa_ghb', 1.0,                      desc='(default 1.0) multiplicative kappa-factor of the Higgs-bottom coupling' )
    self.decorated.add_parameter( 'kappa_ght', 1.0,                      desc='(default 1.0) multiplicative kappa-factor of the Higgs-top coupling' )
    self.decorated.add_parameter( 'use_massive_b', 1,                    desc='(default 1, enabled) enable bottom quark loops', parameter='massivebottom' )
    self.decorated.add_parameter( 'use_massive_t', 1,                    desc='(default 1, enabled) enable charm quark loops', parameter='massivetop' )
