# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl HiggsPropertiesDecorator
#  Powheg runcard decorator for Higgs mass/width
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class HiggsPropertiesDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'hmass '+str(ATLASCommonParameters.mass_H)+'   ! mass of Higgs boson in GeV\n' )
      f.write( 'hwidth '+str(ATLASCommonParameters.width_H)+' ! width of Higgs boson in GeV\n' )
