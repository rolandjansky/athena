# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl HiggsDecayPropertiesDecorator
#  Powheg runcard decorator for Higgs decay properties
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class HiggsDecayPropertiesDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.hdecaywidth   = 0
    self.decorated.masswindow    = 10.0
    self.decorated.nnlo          = -1
    self.decorated.use_massive_b = True
    self.decorated.use_massive_c = True


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'bottommass '+str( [-1,ATLASCommonParameters.mass_b][self.decorated.use_massive_b] )+' ! bottom quark mass (enabled if defined)\n' )
      f.write( 'charmmass '+str( [-1,ATLASCommonParameters.mass_c][self.decorated.use_massive_c] )+'  ! charm quark mass (enabled if defined)\n' )
      f.write( 'hdecaywidth '+str(self.decorated.hdecaywidth)+'                                       ! 0: the hwidth value is used\n' )
      f.write( '                                                                                      ! 1: read total decay width from HDECAY sm.br2 file\n' )
      f.write( 'masswindow '+str(self.decorated.masswindow)+'                                         ! (default 10d0) number of widths around hmass in the BW for an off-shell Higgs boson\n' )
      f.write( 'nnlo '+str(self.decorated.nnlo)+'                                                     ! \n' )
