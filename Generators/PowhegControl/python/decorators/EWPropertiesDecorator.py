# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl EWPropertiesDecorator
#  Powheg runcard decorator for electroweak properties
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class EWPropertiesDecorator(object) :

  ## Define decorator name string
  name = 'electroweak properties'

  def __init__( self, decorated, boson ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'alphaem', ATLASCommonParameters.alphaem,                desc='EM coupling' )
    self.decorated.add_parameter( 'alphaem_inv', 1.0/float(ATLASCommonParameters.alphaem), desc='EM coupling' )
    if boson == 'w' :
      self.decorated.add_parameter( 'mass_W', ATLASCommonParameters.mass_W,              desc='mass of W boson in GeV', parameter='wmass' )
      self.decorated.add_parameter( 'width_W', ATLASCommonParameters.width_W,            desc='W width', parameter='wwidth' )
    elif boson == 'W' :
      self.decorated.add_parameter( 'mass_W', ATLASCommonParameters.mass_W,              desc='W mass in GeV', parameter='Wmass' )
      self.decorated.add_parameter( 'width_W', ATLASCommonParameters.width_W,            desc='W width in GeV', parameter='Wwidth' )
    self.decorated.add_parameter( 'mass_Z', ATLASCommonParameters.mass_Z,                desc='Z mass in GeV', parameter='Zmass' )
    self.decorated.add_parameter( 'width_Z', ATLASCommonParameters.width_Z,              desc='Z width in GeV', parameter='Zwidth' )
