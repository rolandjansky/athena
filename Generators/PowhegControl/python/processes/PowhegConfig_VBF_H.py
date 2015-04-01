# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_VBF_H
#  Powheg configuration for VBF_H subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators

## Default Powheg configuration for VBF_H generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_VBF_H(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_VBF_H, self).__init__( runArgs, opts )
    self._powheg_executable += '/VBF_H/pwhg_main'

    ## Add process specific options
    self.complexpolescheme = -1

    ## Decorate with generic option sets
    PowhegDecorators.decorate( self, 'Higgs decay mode' )
    PowhegDecorators.decorate( self, 'Higgs fixed width' )
    PowhegDecorators.decorate( self, 'Higgs properties' )
    PowhegDecorators.decorate( self, 'radiation' )

    ## Set optimised integration parameters
    self.ncall1   = 50000
    self.ncall2   = 150000
    self.nubound  = 150000
    self.xupbound = 10
    self.foldx    = 2
    self.foldy    = 2
    self.foldphi  = 2

  ## Extend base-class runcard generation
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( self.runcard_path(), 'a' ) as f :
      f.write( 'complexpolescheme '+str(self.complexpolescheme)+' ! 0 = SM\n' )
