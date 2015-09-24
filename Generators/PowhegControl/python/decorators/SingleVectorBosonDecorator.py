# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl SingleVectorBosonDecorator
#  Powheg runcard decorator for single vector boson production
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class SingleVectorBosonDecorator(object) :

  ## Define decorator name string
  name = 'single vector boson'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'alphaem', ATLASCommonParameters.alphaem, desc='(default ATLAS) EM coupling' )
    self.decorated.add_parameter( 'running_width', -1,                      desc='(default -1, use Powheg default)' )
    self.decorated.add_parameter( 'Wmass', ATLASCommonParameters.mass_W,    desc='(default ATLAS) W mass in GeV' )
    self.decorated.add_parameter( 'Wwidth', ATLASCommonParameters.width_W,  desc='(default ATLAS) W width in GeV' )
    self.decorated.add_parameter( 'Zmass', ATLASCommonParameters.mass_Z,    desc='(default ATLAS) Z mass in GeV' )
    self.decorated.add_parameter( 'Zwidth', ATLASCommonParameters.width_Z,  desc='(default ATLAS) Z width in GeV' )
