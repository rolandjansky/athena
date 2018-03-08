# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl V2Decorator
#  Powheg runcard decorator for version 2.0
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class V2Decorator(object) :

  ## Define decorator name string
  name = 'v2'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated
    self.decorated._powheg_version_type = 2

    self.decorated.add_parameter( 'btildeborn', -1,                             desc='(default -1, use Powheg default)' )
    self.decorated.add_parameter( 'btildecoll', -1,                             desc='(default -1, use Powheg default)' )
    self.decorated.add_parameter( 'btildereal', -1,                             desc='(default -1, use Powheg default) for fixed order distributions: distinguish Born, virtual and subtraction-term contributions from real ones.' )
    self.decorated.add_parameter( 'btildevirt', -1,                             desc='(default -1, use Powheg default)' )
    self.decorated.add_parameter( 'doublefsr', -1,                              desc='(default -1, disabled) reduce observable spikes by suppressing FSR emissions harder than the emitter.' )
    self.decorated.add_parameter( 'evenmaxrat', 1,                              desc='(default 1, enabled) speed up upper-bound calculation by taking maximum of all identical processes.' )
    self.decorated.add_parameter( 'fastbtlbound', 1,                            desc='(default 1, enabled) use fast btilde bound.' )
    self.decorated.add_parameter( 'fixedgrid', -1,                              desc='(default -1, use Powheg default)' )
    self.decorated.add_parameter( 'itmx1rm', None,                              desc='(default None) number of iterations for initializing the integration grid for the remnant.' )
    self.decorated.add_parameter( 'itmx2rm', None,                              desc='(default None) number of iterations for computing the integral and finding upper bound for the remnant.' )
    self.decorated.fix_parameter( 'lhrwgt_descr', 'nominal',                    desc='weight description.' )
    self.decorated.fix_parameter( 'lhrwgt_id', 0,                               desc='weight ID.' )
    self.decorated.fix_parameter( 'lhrwgt_group_combine', 'none',               desc='reweighting combination method.' )
    self.decorated.fix_parameter( 'lhrwgt_group_name', 'none',                  desc='group description.' )
    self.decorated.fix_parameter( 'LOevents', ['-1','1'][self.decorated.is_LO], desc='(default process-dependent) produce LOPS events (scalup=ptj); in this case bornonly should also be enabled.' )
    self.decorated.add_parameter( 'minlo', 1,                                   desc='(default enabled where available) use MiNLO.' ) # if minlo is set for unsupported processes, Powheg will crash with an 'st_bornorder' error
    self.decorated.add_parameter( 'ncall1rm', None,                             desc='(default None) number of calls for initializing the integration grid for the remant.' )
    self.decorated.add_parameter( 'ncall2rm', None,                             desc='(default None) number of calls for computing the integral and finding upper bound for the remnant.' )
    self.decorated.add_parameter( 'noevents', -1,                               desc='(default -1, use Powheg default)' )
    self.decorated.add_parameter( 'novirtual', -1,                              desc='(default -1, use Powheg default)' )
    self.decorated.add_parameter( 'parallelstage', -1,                          desc='(default -1, disabled) 1...4, which stage to perform in parallel.' )
    self.decorated.add_parameter( 'stage2init', -1,                             desc='(default -1, use Powheg default)' )
    self.decorated.add_parameter( 'storemintupb', 1,                            desc='(default 1, enabled) cache cross section calls to ease building of upper bounding envelope in later parallel stage.' )
    ## Add radiation for NLO processes
    if not self.decorated.is_LO :
      self.decorated.add_parameter( 'olddij', -1,                               desc='(default -1, use Powheg default)' )


  def finalise( self ) :
    ## Set up parallelisation parameters if in multicore mode
    if self.decorated.cores > 1 :
      if self.decorated.ncall1rm is not None : self.decorated.ncall1rm = int( self.decorated.ncall1rm / self.decorated.cores + 0.5 )
      self.decorated.parallelstage = 1
      self.decorated.fix_parameter( 'ncallfrominput', -1, desc='(default -1, disabled) read ncall parameters from input.' )
      self.decorated.fix_parameter( 'xgriditeration', 1,  desc='(default 1) iteration level for the calculation of the importance sampling grid (only relevant for parallelstage=1).' )

    ## Set remnant properties to be equal to regular properties if not set
    if self.decorated.itmx1rm is None : self.decorated.itmx1rm = -1 #self.decorated.itmx1
    if self.decorated.itmx2rm is None : self.decorated.itmx2rm = -1 #self.decorated.itmx2
    if self.decorated.ncall1rm is None : self.decorated.ncall1rm = -1 #self.decorated.ncall1
    if self.decorated.ncall2rm is None : self.decorated.ncall2rm = -1 #self.decorated.ncall2

    ## Fix integration parameters before printing list for user
    [ self.decorated.fix_parameter( parameter ) for parameter in ('itmx1rm', 'itmx2rm', 'ncall1rm', 'ncall2rm', 'parallelstage') ]
