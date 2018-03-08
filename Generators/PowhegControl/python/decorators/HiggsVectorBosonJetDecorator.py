# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl HiggsVectorBosonJetDecorator
#  Powheg runcard decorator for H+V+jet
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class HiggsVectorBosonJetDecorator(object) :

  ## Define decorator name string
  name = 'Higgs + vector boson + jet'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'bornsuppfactV', -1,                   desc='(-1:Powheg default)' )
    self.decorated.add_parameter( 'kappa_ghb', 1.0,                      desc='multiplicative kappa-factor of the Higgs-bottom coupling' )
    self.decorated.add_parameter( 'kappa_ght', 1.0,                      desc='multiplicative kappa-factor of the Higgs-top coupling' )
    self.decorated.add_parameter( 'ptVhigh', -1,                         desc='(-1:Powheg default)' )
    self.decorated.add_parameter( 'ptVlow', -1,                          desc='(-1:Powheg default)' )
    self.decorated.add_parameter( 'use_massive_b', 1,                    desc='(0:disabled; 1:enabled) enable bottom quark loops', parameter='massivebottom' )
    self.decorated.add_parameter( 'use_massive_t', 1,                    desc='(0:disabled; 1:enabled) enable top quark loops', parameter='massivetop' )
    self.decorated.add_parameter( 'Vstep', -1,                           desc='(-1:Powheg default)' )
