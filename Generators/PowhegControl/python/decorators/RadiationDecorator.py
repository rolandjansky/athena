# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl RadiationDecorator
#  Powheg runcard decorator for radiation parameters
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class RadiationDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.ixmax          = 1
    self.decorated.iymax          = 1
    self.decorated.nubound        = 100000
    self.decorated.use_old_ubound = 1
    self.decorated.xupbound       = 2


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'btlscalect -1                                         ! (default -1) Use the scales of the underlying-Born configuration for the subtraction terms\n' )
      f.write( 'btlscalereal -1                                       ! (default -1)\n' )
      f.write( 'icsimax '+str(self.decorated.ixmax)+'                 ! <= 10, number of intervals in x (csi) grid to compute upper bounds\n' )
      f.write( 'iymax '+str(self.decorated.iymax)+'                   ! <= 10, number of intervals in y grid to compute upper bounds\n' )
      f.write( 'nubound '+str(self.decorated.nubound)+'               ! number of calls to setup upper bounds for radiation\n' )
      f.write( 'radregion -1                                          ! (default all regions) only generate radiation in the selected singular region\n' )
      f.write( 'use-old-ubound '+str(self.decorated.use_old_ubound)+' ! if 1 use norm of upper bounding function stored in pwgubound.dat, if present; <> 1 regenerate\n' )
      f.write( 'xupbound '+str(self.decorated.xupbound)+'             ! increase upper bound for radiation generation by this factor\n' )
