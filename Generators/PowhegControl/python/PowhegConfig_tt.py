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
  topdecaymode     = 22222
  semileptonic     = -1
  tdec_elbranching = 0.108

  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None) :
    super(PowhegConfig_tt, self).__init__(runArgs)
    self._powheg_executable += '/hvq/pwhg_main'

    # Add decorators
    PowhegDecorators.decorate( self, 'fixed scale' )
    PowhegDecorators.decorate( self, 'heavy quark' )

    # Set optimised integration parameters
    self.ncall1   = 10000
    self.ncall2   = 100000
    self.nubound  = 100000
    self.xupbound = 2

  # Implement base-class function
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( str(self.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'qmass '+str(SMParams.mass_t)+'                  ! mass of heavy quark in GeV\n' )
      f.write( 'topdecaymode '+str(self.topdecaymode)+'         ! an integer of 5 digits that are either 0, or 2, representing in\n' )
      f.write( '                                                !   the order the maximum number of the following particles(antiparticles)\n' )
      f.write( '                                                !   in the final state: e mu tau up charm\n' )
      f.write( '                                                !   22222    All decays (up to 2 units of everything)\n' )
      f.write( '                                                !   20000    both top go into b l nu (with the appropriate signs)\n' )
      f.write( '                                                !   10011    one top goes into electron (or positron), the other into (any) hadron\n' )
      f.write( '                                                !            or one top goes into charm, the other into up\n' )
      f.write( '                                                !   00022    Fully hadronic\n' )
      f.write( '                                                !   00002    Fully hadronic with two charms\n' )
      f.write( '                                                !   00011    Fully hadronic with a single charm\n' )
      f.write( '                                                !   00012    Fully hadronic with at least one charm\n' )
      f.write( 'semileptonic '+str(self.semileptonic)+'         ! only accept semileptonic decays\n' )
      f.write( 'tdec/elbranching '+str(self.tdec_elbranching)+' ! W electronic branching fraction\n' )
      f.write( 'tdec/wmass '+str(SMParams.mass_W)+'             ! W mass for top decay\n' )
      f.write( 'tdec/wwidth '+str(SMParams.width_W)+'           ! W width\n' )
      f.write( 'tdec/bmass '+str(SMParams.mass_b)+'             ! b quark mass in t decay\n' )
      f.write( 'tdec/twidth '+str(SMParams.width_t)+'           ! top width\n' )
      f.write( 'tdec/emass '+str(SMParams.mass_e)+'             ! electron mass\n' )
      f.write( 'tdec/mumass '+str(SMParams.mass_mu)+'           ! mu mass\n' )
      f.write( 'tdec/taumass '+str(SMParams.mass_tau)+'         ! tau mass\n' )
      f.write( 'tdec/dmass '+str(SMParams.mass_d)+'             ! d mass\n' )
      f.write( 'tdec/umass '+str(SMParams.mass_u)+'             ! u mass\n' )
      f.write( 'tdec/smass '+str(SMParams.mass_s)+'             ! s mass\n' )
      f.write( 'tdec/cmass '+str(SMParams.mass_c)+'             ! c mass\n' )
      f.write( 'tdec/sin2cabibbo '+str(SMParams.sin2cabibbo)+'  ! sine of Cabibbo angle squared\n' )
