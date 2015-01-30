# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to configure Powheg Wt_DR subprocess
#
#   Authors: James Robinson  <james.robinson@cern.ch>
#
#########################################################################################################################

#! /usr/bin/env python
from PowhegConfig_base import PowhegConfig_base
import PowhegDecorators
import SMParams

###############################################################################
#
#  Wt_DR
#
###############################################################################
class PowhegConfig_Wt_DR(PowhegConfig_base) :
  # These are process specific - put generic properties in PowhegConfig_base
  wdecaymode   = 11111
  ttype        = 1

  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None) :
    super(PowhegConfig_Wt_DR, self).__init__(runArgs)
    self._powheg_executable += '/ST_wtch_DR/pwhg_main'

    # Add decorators
    PowhegDecorators.decorate( self, 'CKM' )
    PowhegDecorators.decorate( self, 'top' )

    # Set optimised integration parameters
    self.ncall1   = 50000
    self.ncall2   = 50000
    self.nubound  = 50000

    # Override defaults
    self.topdecaymode = 11111

  # Implement base-class function
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( str(self.TestArea)+'/powheg.input','a') as f :
      f.write( 'alphaem_inv '+str(1.0/float(SMParams.alphaem))+' ! EM coupling\n' )
      f.write( 'lhfm/bmass '+str(SMParams.mass_b)+'              ! b quark mass\n' )
      f.write( 'lhfm/cmass '+str(SMParams.mass_c)+'              ! c quark mass\n' )
      f.write( 'lhfm/emass '+str(SMParams.mass_e)+'              ! electron mass\n' )
      f.write( 'lhfm/mumass '+str(SMParams.mass_mu)+'            ! mu mass\n' )
      f.write( 'lhfm/taumass '+str(SMParams.mass_tau)+'          ! tau mass\n' )
      f.write( 'sthw2 '+str(SMParams.sthw2)+'                    ! sin**2 theta w\n' )
      f.write( 'topmass '+str(SMParams.mass_t)+'                 ! mass of top quark in GeV\n' )
      f.write( 'topwidth '+str(SMParams.width_t)+'               ! top width\n' )
      f.write( 'ttype '+str(self.ttype)+'                        ! (default 1) 1:t, -1:tbar\n' )
      f.write( 'wdecaymode '+str(self.wdecaymode)+'              ! decay mode: the 5 digits correspond to the following\n' )
      f.write( '                                                 !   primary W decay channels (e,mu,tau,u,c)\n' )
      f.write( '                                                 !   0 means close, 1 open\n' )
      f.write( 'wmass '+str(SMParams.mass_W)+'                   ! mass of W boson in GeV\n' )
      f.write( 'wwidth '+str(SMParams.width_W)+'                 ! W width\n' )
