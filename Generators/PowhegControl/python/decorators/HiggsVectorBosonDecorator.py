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

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'mass_b', ATLASCommonParameters.mass_b, desc='b-quark mass', parameter='bmass' )
    self.decorated.add_parameter( 'mass_H_low', 10.,                      desc='M_H > mass low', parameter='min_h_mass' )
    self.decorated.add_parameter( 'mass_H_high', 1000.,                   desc='M_H < mass high', parameter='max_h_mass' )
