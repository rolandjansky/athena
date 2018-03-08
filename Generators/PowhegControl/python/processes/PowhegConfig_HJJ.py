# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_HJJ
#  Powheg configuration for HJJ subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators

## Default Powheg configuration for HJJ generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_HJJ(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_HJJ, self).__init__( runArgs, opts )
    self._powheg_executable += '/HJJ/pwhg_main'

    ## Add process specific options
    self.polecheck = -1

    ## Decorate with generic option sets
    PowhegDecorators.decorate( self, 'CKKW' )
    PowhegDecorators.decorate( self, 'H+jets' )
    PowhegDecorators.decorate( self, 'Higgs fixed width' )
    PowhegDecorators.decorate( self, 'Higgs properties' )
    PowhegDecorators.decorate( self, 'radiation' )
    PowhegDecorators.decorate( self, 'radiation parametrisation' )
    PowhegDecorators.decorate( self, 'running scales' )
    PowhegDecorators.decorate( self, 'v2' )
    PowhegDecorators.decorate( self, 'v2 radiation' )

    ## Set optimised integration parameters
    self.ncall1   = 15000000
    self.ncall1rm = 15000000
    self.ncall2   = 800000
    self.ncall2rm = 800000
    self.nubound  = 400000
    self.xupbound = 2
    self.foldx    = 10
    self.foldy    = 10
    self.foldphi  = 5

# Testing itmx1 = default, itmx2 = default, ncall1 = 10000000, ncall2 = 500000, ncall1rm = 10000000, ncall2rm = 500000, foldx = 5, foldy = 5, foldphi = 2, nubound = 20000, xupbound = default:
#   Cross-section test: 0.012086 FAILED
#   Neg. weight test  : 0.046428 FAILED
#   Upper bound test  : 0.003458 PASSED
#   Output EVNT files : 1        PASSED
#   Generation of 157 events took 43.61 hours: 0.001Hz
# Testing itmx1 = default, itmx2 = default, ncall1 = 5000000, ncall2 = 400000, ncall1rm = 5000000, ncall2rm = 400000, foldx = 2, foldy = 2, foldphi = 2, nubound = 30000, xupbound = default:
#   Cross-section test: 0.022879 FAILED
#   Neg. weight test  : 0.128643 FAILED
#   Upper bound test  : 0.007461 PASSED
#   Output EVNT files : 1        PASSED
#   Generation of 157 events took 8.72 hours: 0.005Hz
# Testing itmx1 = default, itmx2 = default, ncall1 = 500000, ncall2 = 500000, ncall1rm = 500000, ncall2rm = default, foldx = 2, foldy = 2, foldphi = 2, nubound = 600000, xupbound = default:
#   Cross-section test: 0.021104 FAILED
#   Neg. weight test  : 0.134426 FAILED
#   Upper bound test  : 0.006551 PASSED
#   Output EVNT files : 1        PASSED
#   Generation of 173 events took 8.01 hours: 0.006Hz
# Testing itmx1 = default, itmx2 = default, ncall1 = 500000, ncall2 = 500000, ncall1rm = 500000, ncall2rm = 500000, foldx = 1, foldy = 1, foldphi = 1, nubound = 1000, xupbound = default:
#   Cross-section test: 0.032254 FAILED
#   Neg. weight test  : 0.287858 FAILED
#   Upper bound test  : 0.007097 PASSED
#   Output EVNT files : 1        PASSED
#   Generation of 157 events took 2.57 hours: 0.017Hz

    ## Override defaults
    self.bornktmin    = 0.26
    self.bornzerodamp = 1
    self.par_diexp    = 2
    self.par_dijexp   = 2

  ## Extend base-class runcard generation
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( self.runcard_path(), 'a' ) as f :
      f.write( 'polecheck '+str(self.polecheck)+' ! \n' )
