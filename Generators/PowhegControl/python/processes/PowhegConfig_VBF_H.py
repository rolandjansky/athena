# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to configure Powheg VBF_H subprocess
#
#   Authors: James Robinson  <james.robinson@cern.ch>
#            Daniel Hayden   <danhayden0@googlemail.com>
#            Stephen Bieniek <stephen.paul.bieniek@cern.ch>
#
#########################################################################################################################

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators
from .. import ATLASCommonParameters

###############################################################################
#
#  VBF_H
#
###############################################################################
class PowhegConfig_VBF_H(PowhegConfig_base) :
  # These are process specific - put generic properties in PowhegConfig_base
  complexpolescheme = -1
  use_massive_b     = False
  use_massive_c     = False

  # Set process-dependent paths in the constructor
  def __init__( self, runArgs=None, opts=None ) :
    super(PowhegConfig_VBF_H, self).__init__( runArgs, opts )
    self._powheg_executable += '/VBF_H/pwhg_main'

    # Add decorators
    PowhegDecorators.decorate( self, 'Higgs' )

    # Set optimised integration parameters
    self.ncall1   = 50000
    self.ncall2   = 150000
    self.nubound  = 150000
    self.xupbound = 10
    self.foldx    = 2
    self.foldy    = 2
    self.foldphi  = 2

  # Implement base-class function
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( str(self.TestArea)+'/powheg.input', 'a' ) as f :
      bottommass = ATLASCommonParameters.mass_b if self.use_massive_b else -1
      charmmass = ATLASCommonParameters.mass_c if self.use_massive_c else -1
      f.write( 'complexpolescheme '+str(self.complexpolescheme)+' ! 0 = SM\n' )
      f.write( 'bottomass '+str(bottommass)+'                     ! bottom quark mass in GeV (enabled if defined)\n' )
      f.write( 'charmass '+str(charmmass)+'                       ! charm quark mass in GeV (enabled if defined)\n' )
