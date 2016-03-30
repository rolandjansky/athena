# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl HiggsVectorBosonDecorator
#  Powheg runcard decorator for Higgs+vector boson processes
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class HiggsVectorBosonDecorator(object) :

  ## Define decorator name string
  name = 'Higgs + vector boson'

  def __init__( self, decorated, boson='' ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'kappa_ghb', 1.0,                       desc='multiplicative kappa-factor of the Higgs-bottom coupling. ONLY USED FOR REWEIGHTING.' )
    self.decorated.add_parameter( 'kappa_ght', 1.0,                       desc='multiplicative kappa-factor of the Higgs-top coupling. ONLY USED FOR REWEIGHTING.' )
    if boson == 'W' :
      self.decorated.add_parameter( 'kappa_ghw', 1.0,                     desc='multiplicative kappa-factor of the Higgs-W coupling. ONLY USED FOR REWEIGHTING.' )
    elif boson == 'Z' :
      self.decorated.add_parameter( 'kappa_ghz', 1.0,                     desc='multiplicative kappa-factor of the Higgs-Z coupling. ONLY USED FOR REWEIGHTING.' )
    self.decorated.add_parameter( 'mass_b', ATLASCommonParameters.mass_b, desc='b-quark mass', parameter='bmass' )
    self.decorated.add_parameter( 'mass_H_low', 10.,                      desc='M_H > mass low', parameter='min_h_mass' )
    self.decorated.add_parameter( 'mass_H_high', 1000.,                   desc='M_H < mass high', parameter='max_h_mass' )
