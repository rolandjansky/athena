# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl V2Decorator
#  Powheg runcard decorator for version 2.0
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class V2Decorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated
    self.decorated._powheg_version_type = 2

    self.decorated.btildeborn           = -1
    self.decorated.btildecoll           = -1
    self.decorated.btildereal           = -1
    self.decorated.btildevirt           = -1
    self.decorated.doublefsr            = -1
    self.decorated.evenmaxrat           = 1
    self.decorated.fastbtlbound         = 1
    self.decorated.fixedgrid            = -1
    self.decorated.itmx1rm              = None
    self.decorated.itmx2rm              = None
    self.decorated.lhrwgt_descr         = 'nominal'
    self.decorated.lhrwgt_group_combine = 'none'
    self.decorated.lhrwgt_group_name    = 'none'
    self.decorated.lhrwgt_id            = 0
    self.decorated.minlo                = 1 # if minlo is set for unsupported processes, Powheg will crash with an 'st_bornorder' error
    self.decorated.ncall1rm             = -1
    self.decorated.ncall2rm             = -1
    self.decorated.novirtual            = -1
    self.decorated.parallelstage        = -1
    self.decorated.stage2init           = -1
    self.decorated.storemintupb         = 1
    self.decorated.xgriditeration       = -1


  def append_to_run_card( self ) :
    ## Set remnant iterations to be equal to regular iterations if not set
    if self.decorated.itmx1rm is None : self.decorated.itmx1rm = self.decorated.itmx1
    if self.decorated.itmx2rm is None : self.decorated.itmx2rm = self.decorated.itmx2
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'btildeborn '+str(self.decorated.btildeborn)+'                         ! \n' )
      f.write( 'btildecoll '+str(self.decorated.btildecoll)+'                         ! \n' )
      f.write( 'btildereal '+str(self.decorated.btildereal)+'                         ! For fixed order distributions: Born, virtual and subtraction-term contributions distinguished from the real ones.\n' )
      f.write( 'btildevirt '+str(self.decorated.btildevirt)+'                         ! \n' )
      f.write( 'compute_rwgt 0                                                        ! (default 0) 0:nominal, 1:reweight\n' )
      f.write( 'doublefsr '+str(self.decorated.doublefsr)+'                           ! (default 0) if 1 reduces spikes on final observables. Use new mechanism to generate regions such that emissions harder than the emitter in FSR are suppressed\n' )
      f.write( 'evenmaxrat '+str(self.decorated.evenmaxrat)+'                         ! (default 0) if 1 speed up upper-bound calculation by taking maximum of all identical processes\n' )
      f.write( 'fastbtlbound '+str(self.decorated.fastbtlbound)+'                     ! (default 0) if 1 use fast btilde bound\n' )
      f.write( 'fixedgrid '+str(self.decorated.fixedgrid)+'                           ! \n' )
      f.write( 'itmx1rm '+str(self.decorated.itmx1rm)+'                               ! number of iterations for initializing the integration grid for the remnant\n' )
      f.write( 'itmx2rm '+str(self.decorated.itmx2rm)+'                               ! number of iterations for computing the integral and finding upper bound for the remnant\n' )
      f.write( 'lhrwgt_descr \''+str(self.decorated.lhrwgt_descr)+'\'                 ! weight description\n' )
      f.write( 'lhrwgt_id \''+str(self.decorated.lhrwgt_id)+'\'                       ! weight ID\n' )
      f.write( 'lhrwgt_group_combine \''+str(self.decorated.lhrwgt_group_combine)+'\' ! reweighting combination method\n' )
      f.write( 'lhrwgt_group_name \''+str(self.decorated.lhrwgt_group_name)+'\'       ! group description\n' )
      f.write( 'LOevents -1                                                           ! with LOevents==1 bornonly must be set equal to 1\n' )
      f.write( 'minlo '+str(self.decorated.minlo)+'                                   ! (default 0) set to 1 to use MiNLO\n' )
      f.write( 'ncall1rm '+str(self.decorated.ncall1rm)+'                             ! number of calls for initializing the integration grid for the remant\n' )
      f.write( 'ncall2rm '+str(self.decorated.ncall2rm)+'                             ! number of calls for computing the integral and finding upper bound for the remnant\n' )
      f.write( 'ncallfrominput -1                                                     ! Read ncall parameters from input\n' )
      f.write( 'noevents -1                                                           ! \n' )
      f.write( 'novirtual '+str(self.decorated.novirtual)+'                           ! \n' )
      f.write( 'parallelstage '+str(self.decorated.parallelstage)+'                   ! 1...4, which stage to perform in parallel\n' )
      f.write( 'stage2init '+str(self.decorated.stage2init)+'                         ! \n' )
      f.write( 'storeinfo_rwgt 0                                                      ! enable new-style PDF information\n' )
      f.write( 'storemintupb '+str(self.decorated.storemintupb)+'                     ! store results of inclusive cross section calls to allow easier building of upper bounding envelope in a later parallel stage2init\n' )
      f.write( 'xgriditeration '+str(self.decorated.xgriditeration)+'                 ! iteration level for the calculation of the importance sampling grid (only relevant for parallelstage=1\n' )
