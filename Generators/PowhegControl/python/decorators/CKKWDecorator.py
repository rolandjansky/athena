# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl CKKWDecorator
#  Powheg runcard decorator for CKKW scalup
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class CKKWDecorator(object) :

  ## Define decorator name string
  name = 'CKKW'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'ckkwscalup', 1, desc='(0:use Powheg scalup; 1:CKKW scalup [compute scalup scale for subsequent shower using the smallest kt in the final state])' )
