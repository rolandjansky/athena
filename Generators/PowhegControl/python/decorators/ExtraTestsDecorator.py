# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl ExtraTestsDecorator
#  Powheg runcard decorator for skipextratests parameter
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class ExtraTestsDecorator(object) :

  ## Define decorator name string
  name = 'extra tests'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'skipextratests', -1, default='{0}', desc='(-1:use Powheg default)' )
