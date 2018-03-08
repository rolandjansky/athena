# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_WZ
#  Powheg configuration for WZ subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from ..decorators import PowhegDecorators

## Default Powheg configuration for WZ generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_WZ(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_WZ, self).__init__( runArgs, opts )
    self._powheg_executable += '/WZ/pwhg_main'

    ## Add process specific options
    self.diagCKM = 0

    ## Decorate with generic option sets
    PowhegDecorators.decorate( self, 'anomalous coupling' )
    PowhegDecorators.decorate( self, 'diboson' )
    PowhegDecorators.decorate( self, 'diboson interference' )
    PowhegDecorators.decorate( self, 'fixed scale' )
    PowhegDecorators.decorate( self, 'radiation' )
    PowhegDecorators.decorate( self, 'running width' )
    PowhegDecorators.decorate( self, 'v2' )
    PowhegDecorators.decorate( self, 'v2 radiation' )
    PowhegDecorators.decorate( self, 'zero width' )

    ## Set optimised integration parameters
    self.ncall1   = 40000
    self.ncall2   = 70000
    self.nubound  = 80000
    self.itmx1    = 4
    self.itmx2    = 8
    self.xupbound = 10

    ## Override defaults
    self.allowed_decay_modes = [ 'WZevee', 'WZmuvmumu', 'WZtauvtautau', 'WZevmumu', 'WZmuvee', 'WZevtautau', 'WZmuvtautau', 'WZtauvee', 'WZtauvmumu', 'WZlvqq', 'WZqqll', 'WZlvll', 'WZlvvv', 'WZqqvv', 'WZqqqq', 'e+ee', 'e+mumu', 'e-mumu' ]
    self.decay_mode = 'WZlvll'
    self.minlo      = -1


  ## Extend base-class runcard generation
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( self.runcard_path(), 'a' ) as f :
      f.write( 'diagCKM '+str(self.diagCKM)+' ! (default 0) 0:normal CKM; 1:diagonal CKM\n' )
