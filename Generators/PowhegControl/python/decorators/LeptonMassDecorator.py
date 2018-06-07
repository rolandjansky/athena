# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl LeptonMassDecorator
#  Powheg runcard decorator for lepton masses
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class LeptonMassDecorator(object) :

  ## Define decorator name string
  name = 'lepton mass'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'lhfm/emass', ATLASCommonParameters.mass_e,     default='{0}', desc='electron mass' )
    self.decorated.add_parameter( 'lhfm/mumass', ATLASCommonParameters.mass_mu,   default='{0}', desc='mu mass' )
    self.decorated.add_parameter( 'lhfm/taumass', ATLASCommonParameters.mass_tau, default='{0}', desc='tau mass' )
