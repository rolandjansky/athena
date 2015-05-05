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

    self.decorated.fix_parameter( 'lhfm/emass', ATLASCommonParameters.mass_e,     desc='(default ATLAS) electron mass' )
    self.decorated.fix_parameter( 'lhfm/mumass', ATLASCommonParameters.mass_mu,   desc='(default ATLAS) mu mass' )
    self.decorated.fix_parameter( 'lhfm/taumass', ATLASCommonParameters.mass_tau, desc='(default ATLAS) tau mass' )
