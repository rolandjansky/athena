# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_Dijet
#  Powheg configuration for Dijet subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators

## Default Powheg configuration for Dijet generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_Dijet(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_Dijet, self).__init__( runArgs, opts )
    self._powheg_executable += '/Dijet/pwhg_main'

    ## Add process specific options
    self.jacsing = -1

    ## Decorate with generic option sets
    PowhegDecorators.decorate( self, 'fixed scale' )
    PowhegDecorators.decorate( self, 'radiation' )
    PowhegDecorators.decorate( self, 'v2' )
    PowhegDecorators.decorate( self, 'v2 radiation' )

    ## Set optimised integration parameters
    self.ncall1   = 10000
    self.ncall2   = 10000
    self.nubound  = 15000
    self.xupbound = 2
    self.foldx    = 10
    self.foldy    = 10
    self.foldphi  = 5

    ## Override defaults
    self.bornktmin = 5.0
    self.minlo     = -1
    ## Fix problem with spikes in final observables
    # Options recommended by Paolo Nason to be used with doublefsr (private communication)
    self.doublefsr  = 1
    self.par_diexp  = 4
    self.par_dijexp = 4
    self.par_2gsupp = 5


  ## Extend base-class runcard generation
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( self.runcard_path(), 'a' ) as f :
      f.write( 'jacsing '+str(self.jacsing)+' ! \n' )
