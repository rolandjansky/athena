# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl AnomalousCouplingDecorator
#  Powheg runcard decorator for anomalous vector boson couplings
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class AnomalousCouplingDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.delg1_g           = 0
    self.decorated.delg1_z           = 0
    self.decorated.delk_g            = 0
    self.decorated.delk_z            = 0
    self.decorated.lambda_g          = 0
    self.decorated.lambda_z          = 0
    self.decorated.tevscale          = -1


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'delg1_g '+str(self.decorated.delg1_g)+'   ! Delta_g1(Gamma)\n' )
      f.write( 'delg1_z '+str(self.decorated.delg1_z)+'   ! Delta_g1(Z)\n' )
      f.write( 'delk_g '+str(self.decorated.delk_g)+'     ! Delta_K(Gamma)\n' )
      f.write( 'delk_z '+str(self.decorated.delk_z)+'     ! Delta_K(Z)\n' )
      f.write( 'lambda_g '+str(self.decorated.lambda_g)+' ! Lambda(gamma)\n' )
      f.write( 'lambda_z '+str(self.decorated.lambda_z)+' ! Lambda(Z)\n' )
      f.write( 'tevscale '+str(self.decorated.tevscale)+' ! W form-factor scale, in TeV\n' )
