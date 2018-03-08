# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_ggHZ
#  Powheg configuration for ggHZ subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators

## Default Powheg configuration for ggHZ generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_ggHZ(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_ggHZ, self).__init__( runArgs, opts )
    self._powheg_executable += '/ggHZ/pwhg_main'

    ## Decorate with generic option sets
    PowhegDecorators.decorate( self, 'Higgs fixed width' )
    PowhegDecorators.decorate( self, 'Higgs mass window' )
    PowhegDecorators.decorate( self, 'Higgs properties' )
    PowhegDecorators.decorate( self, 'running scales' )
    PowhegDecorators.decorate( self, 'vector boson decay' )
    PowhegDecorators.decorate( self, 'v2' )
    PowhegDecorators.decorate( self, 'Z mass window' )

    ## Set optimised integration parameters
    self.ncall1   = 25000
    self.ncall2   = 60000
    self.nubound  = 60000
    self.xupbound = 6
    self.itmx1    = 1

    ## Override defaults
    self.minlo = -1


  ## Extend base-class runcard generation
  def generateRunCard(self) :
    self.initialiseRunCard()
