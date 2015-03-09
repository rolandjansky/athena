# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to configure Powheg Zj subprocess
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
#  Zj
#
###############################################################################
class PowhegConfig_Zj(PowhegConfig_base) :
  # Set process-dependent paths in the constructor
  def __init__( self, runArgs=None, opts=None ) :
    super(PowhegConfig_Zj, self).__init__( runArgs, opts )
    self._powheg_executable += '/Zj/pwhg_main'

    #PowhegDecorators.decorate( self, 'vector boson decay' )
    PowhegDecorators.decorate( self, 'single boson' )

    # Set optimised integration parameters
    self.ncall1   = 120000
    self.ncall2   = 80000
    self.nubound  = 20000
    self.xupbound = 3
    self.foldx    = 5
    self.foldy    = 5
    self.foldphi  = 2

    # Override defaults
    self.bornktmin  = 10.0

  # Implement base-class function
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( str(self.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'lhfm/emass '+str(ATLASCommonParameters.mass_e)+'     ! electron mass\n' )
      f.write( 'lhfm/mumass '+str(ATLASCommonParameters.mass_mu)+'   ! muon mass\n' )
      f.write( 'lhfm/taumass '+str(ATLASCommonParameters.mass_tau)+' ! tau mass\n' )
