# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl AnomalousCouplingDecorator
#  Powheg runcard decorator for anomalous vector boson couplings
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class AnomalousCouplingDecorator(object) :

  ## Define decorator name string
  name = 'anomalous coupling'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'delg1_g', 0,   desc='Delta_g1(Gamma)' )
    self.decorated.add_parameter( 'delg1_z', 0,   desc='Delta_g1(Z)' )
    self.decorated.add_parameter( 'delk_g', 0,    desc='Delta_K(Gamma)' )
    self.decorated.add_parameter( 'delk_z', 0,    desc='Delta_K(Z)' )
    self.decorated.add_parameter( 'lambda_g', 0,  desc='Lambda(gamma)' )
    self.decorated.add_parameter( 'lambda_z', 0,  desc='Lambda(Z)' )
    self.decorated.add_parameter( 'tevscale', -1, desc='(-1:use Powheg default) W form-factor scale, in TeV' )
