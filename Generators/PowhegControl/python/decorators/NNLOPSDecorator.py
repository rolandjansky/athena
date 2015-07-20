# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl NNLOPSDecorator
#  Powheg runcard decorator for NNLOPS switch
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class NNLOPSDecorator(object) :

  ## Define decorator name string
  name = 'NNLOPS'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_phantom( 'NNLOPS_input', {},   desc='(default empty) dictionary of labels => (pre-existing) HNNLO output' )
    self.decorated.add_phantom( 'NNLOPS_weights', {}, desc='(default empty) dictionary of weight ID string => weight calculation in NNLOPS format' )


  def finalise( self ) :
    # Check NNLOPS inputs
    # self.decorated.NNLOPS_input = self.decorated.pop('NNLOPS_input')
    if not isinstance( self.decorated.NNLOPS_input, dict ) :
      self.decorated.logger.fatal( 'NNLOPS_input does not appear to be a valid label => file dictionary!' )

    # Check NNLOPS weight outputs
    # self.decorated.NNLOPS_weights = self.decorated.pop('NNLOPS_weights')
    if not isinstance( self.decorated.NNLOPS_weights, dict ) :
      self.decorated.logger.fatal( 'NNLOPS_weights does not appear to be a valid ID string => calculation string!' )
