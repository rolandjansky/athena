# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to configure Powheg tt subprocess
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
#  tt
#
###############################################################################
class PowhegConfig_tt(PowhegConfig_base) :
  # These are process specific - put generic properties in PowhegConfig_base
  semileptonic     = -1

  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None) :
    super(PowhegConfig_tt, self).__init__(runArgs)
    self._powheg_executable += '/hvq/pwhg_main'

    # Add decorators
    PowhegDecorators.decorate( self, 'fixed scale' )
    PowhegDecorators.decorate( self, 'heavy quark' )
    PowhegDecorators.decorate( self, 'top' )

    # Set optimised integration parameters
    self.ncall1   = 10000
    self.ncall2   = 100000
    self.nubound  = 100000
    self.xupbound = 2

    # Override defaults
    self.topdecaymode = 22222

  # Implement base-class function
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( str(self.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'qmass '+str(SMParams.mass_t)+'                  ! mass of heavy quark in GeV\n' )
      f.write( 'semileptonic '+str(self.semileptonic)+'         ! only accept semileptonic decays\n' )
      f.write( 'tdec/bmass '+str(SMParams.mass_b)+'             ! b quark mass in t decay\n' )
      f.write( 'tdec/cmass '+str(SMParams.mass_c)+'             ! c quark mass\n' )
      f.write( 'tdec/dmass '+str(SMParams.mass_d)+'             ! d quark mass\n' )
      f.write( 'tdec/umass '+str(SMParams.mass_u)+'             ! u quark mass\n' )
      f.write( 'tdec/smass '+str(SMParams.mass_s)+'             ! s quark mass\n' )
      f.write( 'tdec/twidth '+str(SMParams.width_t)+'           ! top width\n' )
      f.write( 'tdec/sin2cabibbo '+str(SMParams.sin2cabibbo)+'  ! sine of Cabibbo angle squared\n' )
