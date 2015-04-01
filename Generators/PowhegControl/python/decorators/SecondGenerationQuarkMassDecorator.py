# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl SecondGenerationQuarkMassDecorator
#  Powheg runcard decorator for second generation quark masses
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class SecondGenerationQuarkMassDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'bmass_lhe '+str(ATLASCommonParameters.mass_b)+' ! Set b-quark mass for momentum reshuffling\n' )
      f.write( 'cmass_lhe '+str(ATLASCommonParameters.mass_c)+' ! Set c-quark mass for momentum reshuffling\n' )
