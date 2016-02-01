# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl UpperBoundDecorator
#  Powheg runcard decorator for upper bound parameters
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class UpperBoundDecorator(object) :

  ## Define decorator name string
  name = 'upper bound'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'ixmax', 1,          desc='number of intervals (<= 10) in x (csi) grid to compute upper bounds', parameter='icsimax' )
    self.decorated.add_parameter( 'iymax', 1,          desc='number of intervals (<= 10) in y grid to compute upper bounds' )
    self.decorated.add_parameter( 'nubound', 10000,    desc='number of calls to setup upper bounds for radiation' )
    self.decorated.add_parameter( 'radregion', -1,     desc='(-1:all regions) only generate radiation in the selected singular region' )
    self.decorated.fix_parameter( 'use-old-ubound', 1, desc='(0:disabled; 1:enabled) read norm of upper bounding function from pwgubound.dat, if present' )
    self.decorated.add_parameter( 'xupbound', 2,       desc='increase upper bound for radiation generation by this factor' )

  def finalise(self) :
    # Force parameters to integer values
    [ setattr( self.decorated, parameter, int( getattr(self.decorated, parameter) ) ) for parameter in ('ixmax','iymax','nubound','xupbound') ]
