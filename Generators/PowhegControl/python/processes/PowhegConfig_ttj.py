# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_ttj
#  Powheg configuration for ttj subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from .. import ATLASCommonParameters

## Default Powheg configuration for ttj generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_ttj(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_ttj, self).__init__( runArgs, opts )
    self._powheg_executable += '/ttJ/pwhg_main'

    ## Logger warnings for unvalidated process
    self.logger.warning( 'Integration parameters have not been validated - see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/PowhegForATLAS#Changing_POWHEG_BOX_integration' )

    ## Add process specific options
    self.add_parameter( 'bcut', -1 )
    self.add_parameter( 'collremnsamp', -1, parameter='collremnsamp"' ) # probably a typo in Powheg
    self.add_parameter( 'ffltest', -1 )
    self.add_parameter( 'ggproc', -1 )
    self.add_parameter( 'gqbproc', -1 )
    self.add_parameter( 'largecorrfact', -1 )
    self.add_parameter( 'psgen', -1 )
    self.add_parameter( 'ptmin_jet', 25, desc='jet min pt in GeV for use in analysis code (not relevant here)' )
    self.add_parameter( 'qgproc', -1 )
    self.add_parameter( 'qqbproc', -1 )
    self.add_parameter( 'qqpproc', -1 )
    self.add_parameter( 'qqproc', -1 )
    self.add_parameter( 'R_jet', 0.4, desc='jet radius for use in internal Powheg analysis code (not relevant here)' )
    self.add_parameter( 'use_OLP_interface', -1, parameter='use-OLP-interface' )

    ## Decorate with generic option sets
    self.add_parameter_set( 'CKM' )
    self.add_parameter_set( 'jacsing' )
    self.add_parameter_set( 'lepton mass' )
    self.add_parameter_set( 'LHEv3' )
    self.add_parameter_set( 'mass window' )
    self.add_parameter_set( 'running scale' )
    self.add_parameter_set( 'semileptonic' )
    self.add_parameter_set( 'sin**2 theta W' )
    self.add_parameter_set( 'single vector boson' )
    self.add_parameter_set( 'top decay branching', tdec_prefix=False )
    self.add_parameter_set( 'top decay mode' )
    self.add_parameter_set( 'top decay second generation quark' )
    self.add_parameter_set( 'top mass' )
    self.add_parameter_set( 'V+jets' )
    self.add_parameter_set( 'zero width' )

    ## Set optimised integration parameters
    self.itmx1   = 5
    self.itmx2   = 10
    self.ncall1  = 2000000
    self.ncall2  = 5000000
    self.nubound = 5000000
    self.foldx   = 5
    self.foldy   = 5
    self.foldphi = 2

    ## Override defaults
    self.bornktmin    = 5.0
    self.bornsuppfact = 100.0 # was disabled
    self.minlo        = -1
    self.quark_mass   = ATLASCommonParameters.mass_t
    self.topdecaymode = 22222

    self.populate_default_strings()
