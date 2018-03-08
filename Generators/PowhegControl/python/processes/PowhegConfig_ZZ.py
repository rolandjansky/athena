# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_ZZ
#  Powheg configuration for ZZ subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators

## Default Powheg configuration for ZZ generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_ZZ(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_ZZ, self).__init__( runArgs, opts )
    self._powheg_executable += '/ZZ/pwhg_main'

    ## Add process specific options
    self.cutallpairs = -1

    ## Decorate with generic option sets
    PowhegDecorators.decorate( self, 'diboson' )
    PowhegDecorators.decorate( self, 'diboson interference' )
    PowhegDecorators.decorate( self, 'fixed scale' )
    PowhegDecorators.decorate( self, 'radiation' )
    PowhegDecorators.decorate( self, 'running width' )
    PowhegDecorators.decorate( self, 'v2' )
    PowhegDecorators.decorate( self, 'v2 radiation' )
    PowhegDecorators.decorate( self, 'zero width' )

    ## Set optimised integration parameters
    self.ncall1   = 50000
    self.ncall2   = 50000
    self.nubound  = 100000
    self.xupbound = 10
    self.itmx1    = 5
    self.itmx2    = 14
    self.foldx    = 2
    self.foldy    = 2

    ## Override defaults
    self.allowed_decay_modes = [ 'ZZllll', 'ZZqqll', 'ZZqqqq', 'ZZvvvv', 'ZZvvll', 'ZZvvqq', 'ZZeeee', 'ZZmumumumu', 'ZZtautautautau', 'ZZmumuee', 'ZZeetautau', 'ZZtautaumumu' ]
    self.decay_mode = 'ZZllll'
    self.minlo      = -1


  ## Extend base-class runcard generation
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( self.runcard_path(), 'a' ) as f :
      f.write( 'cutallpairs '+str(self.cutallpairs)+' ! \n' )
