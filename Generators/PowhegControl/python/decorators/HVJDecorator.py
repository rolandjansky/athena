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

    self.decorated.fix_parameter( 'bmass', ATLASCommonParameters.mass_b, desc='(default ATLAS) b quark mass' )
    self.decorated.add_parameter( 'kappa_ghb', 1.0,                      desc='(default 1.0) multiplicative kappa-factor of the Higgs-bottom coupling' )
    self.decorated.add_parameter( 'kappa_ght', 1.0,                      desc='(default 1.0) multiplicative kappa-factor of the Higgs-top coupling' )
    self.decorated.add_parameter( 'use_massive_b', True,                 desc='(default True) enable bottom quark mass' )
    self.decorated.add_parameter( 'use_massive_t', True,                 desc='(default True) enable charm quark mass' )


  def finalise(self) :
    ## Set bottom and charm masses if requested
    self.decorated.fix_parameter( 'massivebottom', [0,1][self.decorated.pop('use_massive_b')], desc='(default process-dependent) include bottom quark loops' )
    self.decorated.fix_parameter( 'massivetop', [0,1][self.decorated.pop('use_massive_t')],    desc='(default process-dependent) include top quark loops' )
