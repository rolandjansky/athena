# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_bb
#  Powheg configuration for bb subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators
from .. import ATLASCommonParameters

## Default Powheg configuration for bb generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_bb(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_bb, self).__init__( runArgs, opts )
    self._powheg_executable += '/hvq/pwhg_main'

    ## Decorate with generic option sets
    PowhegDecorators.decorate( self, 'fixed scale' )
    PowhegDecorators.decorate( self, 'heavy quark' )
    PowhegDecorators.decorate( self, 'radiation' )
    PowhegDecorators.decorate( self, 'second generation quark mass' )
    PowhegDecorators.decorate( self, 'top decay mode' )
    PowhegDecorators.decorate( self, 'v2' )
    PowhegDecorators.decorate( self, 'v2 radiation' )

    ## Set optimised integration parameters
    self.ncall1   = 10000
    self.ncall2   = 20000
    self.nubound  = 20000
    self.xupbound = 2
    self.foldx    = 5
    self.foldy    = 5
    self.foldphi  = 2

    ## Override defaults
    self.bornktmin    = 5.0
    self.minlo        = -1
    self.topdecaymode = 0

  ## Extend base-class runcard generation
  def generateRunCard(self) :
    self.quark_mass = ATLASCommonParameters.mass_b
    self.initialiseRunCard()
