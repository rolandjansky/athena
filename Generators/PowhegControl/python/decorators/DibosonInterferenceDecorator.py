# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl DibosonInterferenceDecorator
#  Powheg runcard decorator for diboson interference#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class DibosonInterferenceDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.mllmin            = 4
    self.decorated.withinterference  = 1


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'mllmin '+str(self.decorated.mllmin)+'                     ! Minimum invariant mass of lepton pairs from Z decay\n' )
      f.write( 'withinterference '+str(self.decorated.withinterference)+' ! if 1 (true) include interference for like flavour charged leptons\n' )
