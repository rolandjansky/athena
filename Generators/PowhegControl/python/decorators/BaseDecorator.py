# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl BaseDecorator
#  Powheg runcard decorator for universal functionality
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class BaseDecorator(object) :

  ## Define decorator name string
  name = 'base'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    ## Beam/event information
    self.decorated.add_parameter( 'nEvents', 5500,     desc='(default 1.1*maxEvents OR 5500) number of events to be generated', parameter='numevts' )
    self.decorated.fix_parameter( 'ih1', 1,            desc='(default 1, proton) hadron 1 type' )
    self.decorated.fix_parameter( 'ih2', 1,            desc='(default 1, proton) hadron 2 type' )
    self.decorated.add_parameter( 'beam_energy', 6500, desc='(default 6500) energy of beam 1 in GeV', parameter='ebeam1' )
    self.decorated.add_parameter( 'beam_energy', 6500, desc='(default 6500) energy of beam 2 in GeV', parameter='ebeam2' )
    # PDF information
    self.decorated.add_parameter( 'PDF', 10800,     desc='(default 10800, CT10) PDF set for hadron 1 (LHAGLUE numbering)', parameter='lhans1' )
    self.decorated.add_parameter( 'PDF', 10800,     desc='(default 10800, CT10) PDF set for hadron 2 (LHAGLUE numbering)', parameter='lhans2' )
    self.decorated.fix_parameter( 'pdfreweight', 1, desc='(default 1, enabled) store PDF information' )
    # Random seeds
    self.decorated.add_parameter( 'manyseeds', -1,  desc='(default -1, disabled) read multiple seeds for the random number generator from pwgseeds.dat' )
    self.decorated.fix_parameter( 'rand1', -1,      desc='(default -1, disabled) user-initiated random seed (disabled for reproducibility)' )
    self.decorated.fix_parameter( 'rand2', -1,      desc='(default -1, disabled) user-initiated random seed (disabled for reproducibility)' )
    self.decorated.add_parameter( 'random_seed', 1, desc='(default 1) seed for the random number generator', parameter='iseed' )
    # Scale evaluation
    self.decorated.add_parameter( 'mu_F', 1.0, desc='(default 1.0) factorization scale factor: mu_fact = mu_ref * facscfact', parameter='facscfact' )
    self.decorated.add_parameter( 'mu_R', 1.0, desc='(default 1.0) renormalization scale factor: mu_ren = mu_ref * renscfact', parameter='renscfact' )
    ## Integration parameters:
    #   The total number of calls is ncall2*itmx2*foldcsi*foldy*foldphi, with a typical call using 1/1400 seconds
    ## These are optimised in each process to ensure:
    #   Upper bound failures      < 1% : to reduce, increase nubound, xupbound or ncall2*itmx2
    #   Cross section uncertainty < 1% : to reduce, increase ncall1*itmx1 or ncall2*itmx2
    #   Negative weight events    < 1% : to reduce, increase fold parameters
    self.decorated.add_parameter( 'itmx1', 5,      desc='(default process-dependent) number of iterations for initializing the integration grid' )
    self.decorated.add_parameter( 'itmx2', 5,      desc='(default process-dependent) number of iterations for computing the integral and finding upper bound' )
    self.decorated.add_parameter( 'ncall1', 10000, desc='(default process-dependent) number of calls for initializing the integration grid' )
    self.decorated.add_parameter( 'ncall2', 10000, desc='(default process-dependent) number of calls for computing the integral and finding upper bound' )
    ## Allowed values for folding parameters are 1, 2, 5, 10, 25, 50
    self.decorated.add_parameter( 'foldx', 1,        desc='(default process-dependent) number of folds on x (csi) integration', parameter='foldcsi' )
    self.decorated.add_parameter( 'foldy', 1,        desc='(default process-dependent) number of folds on y integration' )
    self.decorated.add_parameter( 'foldphi', 1,      desc='(default process-dependent) number of folds on phi integration' )
    self.decorated.add_parameter( 'use_old_grid', 1, desc='(default 1, enabled) use old grid if file pwggrids.dat is present; otherwise regenerate', parameter='use-old-grid' )
    # Born suppression : may be needed in presence of processes where the Born cross section vanishes in some phase-space region
    self.decorated.add_parameter( 'bornktmin', -1,                         desc='(default -1, disabled) Generation cut: minimum kt in underlying Born' )
    self.decorated.fix_parameter( 'bornonly', [0,1][self.decorated.is_LO], desc='(default process-dependent) calculate Born only' )
    self.decorated.add_parameter( 'bornsuppfact', -1,                      desc='(default -1, disabled) Mass parameter for Born suppression factor. If > 0 suppfact = 1' )
    self.decorated.fix_parameter( 'bornzerodamp', 1,                       desc='(default 1, enabled) Use damping in regions where the Born is strongly suppressed (or 0), such as W production.' )
    self.decorated.add_parameter( 'hdamp', -1,                             desc='(default -1, disabled) Born-zero damping factor' )
    self.decorated.add_parameter( 'hfact', -1,                             desc='(default -1, disabled) apply dump factor for high-pt radiation: dumpfac=h**2/(pt2+h**2)' )
    self.decorated.fix_parameter( 'ptsupp', -1,                            desc='(default -1, disabled) deprecated, but some processes complain if it is missing' )
    self.decorated.fix_parameter( 'withdamp', 1,                           desc='(default 1, enabled) use Born-zero damping factor' )
    self.decorated.add_parameter( 'withnegweights', 1,                     desc='(default 1, enabled) allow negative weights' )
    # Parton splitting settings
    self.decorated.add_parameter( 'bottomthr', 5.0,    desc='(default 5.0) bottom threshold for gluon splitting in GeV' )
    self.decorated.add_parameter( 'bottomthrpdf', 5.0, desc='(default 5.0) PDF bottom threshold in GeV' )
    self.decorated.add_parameter( 'charmthr', 1.5,     desc='(default 1.5) charm threshold for gluon splitting in GeV' )
    self.decorated.add_parameter( 'charmthrpdf', 1.5,  desc='(default 1.5) PDF charm threshold in GeV' )
    self.decorated.add_parameter( 'par_2gsupp', 1 )
    self.decorated.add_parameter( 'par_diexp', 1,      desc='(default 1) ISR singularity exponent (p1)' )
    self.decorated.add_parameter( 'par_dijexp', 1,     desc='(default 1) FSR singularity exponent (p2)' )
    self.decorated.add_parameter( 'ptsqmin', 0.8,      desc='(default 0.8) minimum pT in GeV for generation of radiation' )
    # Users are asked not to change these, since their invocation is time consuming and/or may cause some conflicts with other settings.
    self.decorated.fix_parameter( 'iupperisr', 1, desc='(default 1) choice of ISR upper bounding functional form' )
    self.decorated.fix_parameter( 'iupperfsr', 2, desc='(default 2) choice of FSR upper bounding functional form' )
    self.decorated.fix_parameter( 'smartsig', 1,  desc='(default 1, enabled) remember equal amplitudes' )
    self.decorated.fix_parameter( 'testplots', 0, desc='(default 0, disabled) do NLO and PWHG distributions' )
    self.decorated.fix_parameter( 'testsuda', 0,  desc='(default 0, disabled) test Sudakov form factor' )
    self.decorated.fix_parameter( 'withsubtr', 1, desc='(default 1, enabled) subtract real counterterms' )
    # Parameters of unknown use
    self.decorated.fix_parameter( 'flg_debug', -1,      desc='(default -1, disabled) write extra information on LHEF. Breaks PYTHIA showering.' )
    self.decorated.fix_parameter( 'colltest', -1,       desc='(default -1)' )
    self.decorated.fix_parameter( 'softtest', -1,       desc='(default -1)' )
    self.decorated.fix_parameter( 'ubsigmadetails', -1, desc='(default -1)' )
    ## Radiation settings for NLO processes
    if not self.decorated.is_LO :
      self.decorated.fix_parameter( 'btlscalect', -1,    desc='(default -1, disabled) use the scales of the underlying-Born configuration for the subtraction terms' )
      self.decorated.fix_parameter( 'btlscalereal', -1,  desc='(default -1, disabled)' )
      self.decorated.add_parameter( 'ixmax', 1,          desc='(default process-dependent) number of intervals (<= 10) in x (csi) grid to compute upper bounds', parameter='icsimax' )
      self.decorated.add_parameter( 'iymax', 1,          desc='(default process-dependent) number of intervals (<= 10) in y grid to compute upper bounds' )
      self.decorated.add_parameter( 'nubound', 10000,    desc='(default process-dependent) number of calls to setup upper bounds for radiation' )
      self.decorated.fix_parameter( 'radregion', -1,     desc='(default -1, all regions) only generate radiation in the selected singular region' )
      self.decorated.add_parameter( 'use_old_ubound', 1, desc='(default 1, enabled) read norm of upper bounding function from pwgubound.dat, if present', parameter='use-old-ubound' )
      self.decorated.add_parameter( 'xupbound', 2,       desc='(default process-dependent) increase upper bound for radiation generation by this factor' )

  def finalise(self) :
    ## Fix integration parameters before printing list for user
    [ self.decorated.fix_parameter( parameter ) for parameter in ('beam_energy', 'manyseeds', 'random_seed', 'itmx1', 'itmx2', 'ncall1', 'ncall2', 'foldx', 'foldy', 'foldphi' ) ]
    if not self.decorated.is_LO : [ self.decorated.fix_parameter( parameter ) for parameter in ('ixmax', 'iymax', 'nubound', 'xupbound') ]
