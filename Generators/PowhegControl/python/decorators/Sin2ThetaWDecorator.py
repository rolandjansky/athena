# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl Sin2ThetaWDecorator
#  Powheg runcard decorator for sin(theta_W)**2 parameter
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class Sin2ThetaWDecorator(object) :

  ## Define decorator name string
  name = 'sin**2 theta W'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'sthw2', ATLASCommonParameters.sin2thW_eff, default='{0}', desc='sin**2 theta w' )
