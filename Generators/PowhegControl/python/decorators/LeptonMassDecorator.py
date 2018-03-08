# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl LeptonMassDecorator
#  Powheg runcard decorator for lepton masses
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class LeptonMassDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'lhfm/emass '+str(ATLASCommonParameters.mass_e)+'     ! electron mass\n' )
      f.write( 'lhfm/mumass '+str(ATLASCommonParameters.mass_mu)+'   ! mu mass\n' )
      f.write( 'lhfm/taumass '+str(ATLASCommonParameters.mass_tau)+' ! tau mass\n' )
