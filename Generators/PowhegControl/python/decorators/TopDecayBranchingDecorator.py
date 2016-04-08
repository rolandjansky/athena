# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl TopDecayLeptonDecorator
#  Powheg runcard decorator for top decay parameters
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class TopDecayBranchingDecorator(object) :

  ## Define decorator name string
  name = 'top decay branching'

  def __init__( self, decorated, tdec_prefix=True ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    if tdec_prefix :
      self.decorated.add_parameter( 'tdec/elbranching', ATLASCommonParameters.W_lepton_BR, default='{0}', desc='W electronic branching fraction' )
      self.decorated.add_parameter( 'tdec/emass', ATLASCommonParameters.mass_e,            default='{0}', desc='electron mass' )
      self.decorated.add_parameter( 'tdec/mumass', ATLASCommonParameters.mass_mu,          default='{0}', desc='mu mass' )
      self.decorated.add_parameter( 'tdec/taumass', ATLASCommonParameters.mass_tau,        default='{0}', desc='tau mass' )
    else :
      self.decorated.add_parameter( 'elbranching', ATLASCommonParameters.W_lepton_BR,      default='{0}', desc='W electronic branching fraction' )
    self.decorated.add_parameter( 'topwidth', ATLASCommonParameters.width_t,               default='{0}', desc='top width' )
