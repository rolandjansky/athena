# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to configure Powheg Dijet subprocess
#
#   Authors: James Robinson  <james.robinson@cern.ch>
#            Daniel Hayden   <danhayden0@googlemail.com>
#            Stephen Bieniek <stephen.paul.bieniek@cern.ch>
#
#########################################################################################################################

#! /usr/bin/env python
from PowhegConfig_base import PowhegConfig_base
import PowhegDecorators

###############################################################################
#
#  Dijet
#
###############################################################################
class PowhegConfig_Dijet(PowhegConfig_base) :
  # These are process specific - put generic properties in PowhegConfig_base
  jacsing   = -1

  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None) :
    super(PowhegConfig_Dijet, self).__init__(runArgs)
    self._powheg_executable += '/Dijet/pwhg_main'

    # Add decorators
    PowhegDecorators.decorate( self, 'fixed scale' )
    PowhegDecorators.decorate( self, 'v2' )

    # Set optimised integration parameters
    self.ncall1   = 10000 #20000
    self.ncall2   = 10000 #15000
    self.nubound  = 15000 #20000
    self.xupbound = 2
    self.foldx    = 10
    self.foldy    = 10
    self.foldphi  = 5 #10

    # Override defaults
    self.bornktmin  = 5.0
    # Fix problem with spikes in final observables
    # Options recommended by Paolo Nason to be used with doublefst (private communication)
    # Not sure what happens if these are omitted!
    self.doublefsr, self.par_diexp, self.par_dijexp, self.par_2gsupp = 1, 4, 4, 5

  # Implement base-class function
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( str(self.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'jacsing '+str(self.jacsing)+'     ! \n' )
