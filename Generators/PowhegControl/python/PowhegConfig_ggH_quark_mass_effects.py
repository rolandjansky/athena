# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to configure Powheg ggH_quark_mass_effects subprocess
#
#   Authors: James Robinson  <james.robinson@cern.ch>
#            Daniel Hayden   <danhayden0@googlemail.com>
#            Stephen Bieniek <stephen.paul.bieniek@cern.ch>
#
#########################################################################################################################

#! /usr/bin/env python
from PowhegConfig_base import PowhegConfig_base
import PowhegDecorators
import SMParams

###############################################################################
#
#  ggH_quark_mass_effects
#
###############################################################################
class PowhegConfig_ggH_quark_mass_effects(PowhegConfig_base) :
  # These are process specific - put generic properties in PowhegConfig_base
  bwshape       = 1
  ew            = 1
  hdecaywidth   = -1
  masswindow    = -1
  model         = 0
  massren       = 0
  nnlo          = -1
  use_massive_b = False
  use_massive_c = False

  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None) :
    super(PowhegConfig_ggH_quark_mass_effects, self).__init__(runArgs)
    self._powheg_executable += '/gg_H_quark-mass-effects/pwhg_main'

    # Add decorators
    PowhegDecorators.decorate( self, 'Higgs' )
    PowhegDecorators.decorate( self, 'on-shell' )
    PowhegDecorators.decorate( self, 'running scale' )

    # Set optimised integration parameters
    self.ncall1   = 50000 #120000
    self.ncall2   = 100000 #250000
    self.nubound  = 50000

    # Override defaults
    self._use_scale_damp_parameters = True

  # Implement base-class function
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( str(self.TestArea)+'/powheg.input', 'a' ) as f :
      bottommass = SMParams.mass_b if self.use_massive_b else -1
      charmmass = SMParams.mass_c if self.use_massive_c else -1
      f.write( 'bwshape '+str(self.bwshape)+'         ! Controls the functional form of the Breit-Wigner along which Higgs boson virtuality is distributed.\n' )
      f.write( '                                      !   1 (default) : running width that depends on Higgs boson virtuality\n' )
      f.write( '                                      !   2 : fix the BW width to the value in hwidth.\n' )
      f.write( 'ew '+str(self.ew)+'                   ! If 1 enable EW corrections\n' )
      f.write( 'gfermi '+str(SMParams.G_F)+'          ! Fermi constant\n' )
      f.write( 'hdecaywidth '+str(self.hdecaywidth)+' ! If 1 read the total decay width from br.sm2 produced by HDECAY\n' )
      f.write( 'model '+str(self.model)+'             ! 0 = SM\n' )
      f.write( 'massren '+str(self.massren)+'         ! 0 = OS, 1 = MSBAR, 2 = DRBAR\n' )
      f.write( 'masswindow '+str(self.masswindow)+'   ! \n' )
      f.write( 'nnlo '+str(self.nnlo)+'               ! \n' )
      f.write( 'topmass '+str(SMParams.mass_t)+'      ! top quark mass in GeV\n' )
      f.write( 'bottommass '+str(bottommass)+'  ! bottom quark mass in GeV (enabled if defined)\n' )
      f.write( 'charmmass '+str(charmmass)+'   ! charm quark mass in GeV (enabled if defined)\n' )
