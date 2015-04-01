# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl SingleVectorBosonDecorator
#  Powheg runcard decorator for single vector boson production
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class SingleVectorBosonDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.running_width = -1


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'alphaem '+str(ATLASCommonParameters.alphaem)+'      ! EM coupling\n' )
      f.write( 'running_width '+str(self.decorated.running_width)+' ! \n' )
      f.write( 'Wmass '+str(ATLASCommonParameters.mass_W)+'         ! W mass in GeV\n' )
      f.write( 'Wwidth '+str(ATLASCommonParameters.width_W)+'       ! W width in GeV\n')
      f.write( 'Zmass '+str(ATLASCommonParameters.mass_Z)+'         ! Z mass in GeV\n' )
      f.write( 'Zwidth '+str(ATLASCommonParameters.width_Z)+'       ! Z width in GeV\n' )
