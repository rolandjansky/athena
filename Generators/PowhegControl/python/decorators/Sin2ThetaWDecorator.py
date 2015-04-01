# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl Sin2ThetaWDecorator
#  Powheg runcard decorator for sin(theta_W)**2 parameter
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class Sin2ThetaWDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'sthw2 '+str(ATLASCommonParameters.sin2thW_eff)+' ! sin**2 theta w\n' )
