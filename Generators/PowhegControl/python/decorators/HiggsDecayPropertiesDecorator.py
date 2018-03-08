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

    self.decorated.use_massive_b = True
    self.decorated.use_massive_c = True
    self.decorated.add_parameter( 'use_massive_b', True, desc='(default True) enable bottom quark mass' )
    self.decorated.add_parameter( 'use_massive_c', True, desc='(default True) enable charm quark mass' )
    self.decorated.add_parameter( 'hdecaywidth', 0,      desc='(default 0) 0:use hwidth; 1:read total decay width from HDECAY sm.br2 file' )
    self.decorated.add_parameter( 'masswindow', 10.0,    desc='(default 10) number of widths around hmass in the BW for an off-shell Higgs boson' )
    self.decorated.add_parameter( 'nnlo', -1 )


  def finalise(self) :
    ## Set bottom and charm masses if requested
    self.decorated.fix_parameter( 'bottommass', [-1,ATLASCommonParameters.mass_b][self.decorated.pop('use_massive_b')], desc='(default process-dependent) bottom quark mass (enabled if defined)' )
    self.decorated.fix_parameter( 'charmmass', [-1,ATLASCommonParameters.mass_c][self.decorated.pop('use_massive_c')],  desc='(default process-dependent) charm quark mass (enabled if defined)' )
