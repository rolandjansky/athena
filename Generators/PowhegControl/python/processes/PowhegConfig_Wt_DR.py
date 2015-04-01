# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_Wt_DR
#  Powheg configuration for Wt_DR subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators
from .. import ATLASCommonParameters

## Default Powheg configuration for Wt_DR generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_Wt_DR(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_Wt_DR, self).__init__( runArgs, opts )
    self._powheg_executable += '/ST_wtch_DR/pwhg_main'

    ## Add process specific options
    self.wdecaymode = 11111
    self.ttype      = 1

    ## Decorate with generic option sets
    PowhegDecorators.decorate( self, 'CKM' )
    PowhegDecorators.decorate( self, 'lepton mass' )
    PowhegDecorators.decorate( self, 'radiation' )
    PowhegDecorators.decorate( self, 'sin**2 theta W' )
    PowhegDecorators.decorate( self, 'top decay lepton' )
    PowhegDecorators.decorate( self, 'top decay mode' )
    PowhegDecorators.decorate( self, 'top mass' )

    ## Set optimised integration parameters
    self.ncall1  = 50000
    self.ncall2  = 50000
    self.nubound = 50000

    ## Override defaults
    self.topdecaymode = 11111

  ## Extend base-class runcard generation
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( self.runcard_path(),'a') as f :
      f.write( 'alphaem_inv '+str(1.0/float(ATLASCommonParameters.alphaem))+' ! EM coupling\n' )
      f.write( 'lhfm/bmass '+str(ATLASCommonParameters.mass_b)+'              ! b quark mass\n' )
      f.write( 'lhfm/cmass '+str(ATLASCommonParameters.mass_c)+'              ! c quark mass\n' )
      f.write( 'topwidth '+str(ATLASCommonParameters.width_t)+'               ! top width\n' )
      f.write( 'ttype '+str(self.ttype)+'                                     ! (default 1) 1:t, -1:tbar\n' )
      f.write( 'wdecaymode '+str(self.wdecaymode)+'                           ! decay mode: the 5 digits correspond to the following primary W decay channels\n' )
      f.write( '                                                              !   (e,mu,tau,u,c) : 0 means close, 1 open\n' )
      f.write( 'wmass '+str(ATLASCommonParameters.mass_W)+'                   ! mass of W boson in GeV\n' )
      f.write( 'wwidth '+str(ATLASCommonParameters.width_W)+'                 ! W width\n' )
