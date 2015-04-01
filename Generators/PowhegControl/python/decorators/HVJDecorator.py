# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_bb
#  Powheg runcard decorator for H+V+jet
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class HVJDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.kappa_ghb     = 1.0
    self.decorated.kappa_ght     = 1.0
    self.decorated.use_massive_b = True
    self.decorated.use_massive_t = True


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'bmass '+str(ATLASCommonParameters.mass_b)+'                   ! b quark mass\n' )
      f.write( 'kappa_ghb '+str(self.decorated.kappa_ghb)+'                   ! multiplicative kappa-factor of the Higgs-bottom coupling\n' )
      f.write( 'kappa_ght '+str(self.decorated.kappa_ght)+'                   ! multiplicative kappa-factor of the Higgs-top coupling\n' )
      f.write( 'massivebottom '+str( [0,1][self.decorated.use_massive_b] )+'  ! if 1 include bottom quark loops\n' )
      f.write( 'massivetop '+str( [0,1][self.decorated.use_massive_t] )+'     ! if 1 include top quark loops\n' )
