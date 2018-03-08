# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl NNLOReweightingDecorator
#  Powheg runcard decorator for NNLO reweighting
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
class NNLOReweightingDecorator(object) :

  ## Define decorator name string
  name = 'NNLO reweighting'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_phantom( 'NNLO_reweighting_inputs', {}, desc='dictionary of labels => (pre-existing) HNNLO output' )
    self.decorated.add_phantom( 'NNLO_output_weights', {},     desc='dictionary of weight ID string => weight calculation in NNLOPS format' )


  def finalise( self ) :
    # Check NNLO reweighting inputs
    if not isinstance( self.decorated.NNLO_reweighting_inputs, dict ) :
      self.decorated.logger.fatal( 'NNLO_reweighting_inputs does not appear to be a valid label => file dictionary!' )

    # Check NNLO weight outputs
    if not isinstance( self.decorated.NNLO_output_weights, dict ) :
      self.decorated.logger.fatal( 'NNLO_output_weights does not appear to be a valid ID string => calculation string!' )
