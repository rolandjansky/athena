# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl HiggsDecayPropertiesDecorator
#  Powheg runcard decorator for Higgs decay properties
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class HiggsDecayPropertiesDecorator(object) :

  ## Define decorator name string
  name = 'Higgs decay properties'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'hdecaywidth', 0,                       default='{0}', desc='(0:use hwidth; 1:read total decay width from HDECAY sm.br2 file)' )
    self.decorated.add_parameter( 'mass_b', ATLASCommonParameters.mass_b, default='{0}', desc='bottom quark mass (loops disabled if <= 0)', parameter='bottommass' )
    self.decorated.add_parameter( 'mass_c', ATLASCommonParameters.mass_c, default='{0}', desc='charm quark mass (loops enabled if <= 0)', parameter='charmmass' )
    self.decorated.add_parameter( 'masswindow', 10.0,                     default='{0}', desc='number of widths around hmass in the BW for an off-shell Higgs boson' )
    self.decorated.add_parameter( 'nnlo', -1,                             default='{0}', desc='(-1:use Powheg default) enable NNLO rescaling' )
