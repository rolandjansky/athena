# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl TopMassDecorator
#  Powheg runcard decorator for top mass parameter
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class TopMassDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'tmass '+str(ATLASCommonParameters.mass_t)+'   ! top quark mass\n' )
      f.write( 'topmass '+str(ATLASCommonParameters.mass_t)+' ! top quark mass\n' )
