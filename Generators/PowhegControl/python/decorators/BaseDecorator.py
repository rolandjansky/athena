# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl BaseDecorator
#  Powheg runcard decorator for universal functionality
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class BaseDecorator(object) :

  ## Define decorator name string
  name = 'base'

  def __init__( self, decorated ) :
    # Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    # Beam/event information
    self.decorated.add_parameter( 'nEvents', 5500,     default='{0}', desc='number of events to be generated', parameter='numevts' )
    self.decorated.fix_parameter( 'ih1', 1,            default='{0}', desc='(1:proton) type of hadron 1' )
    self.decorated.fix_parameter( 'ih2', 1,            default='{0}', desc='(1:proton) type of hadron 2' )
    self.decorated.add_parameter( 'beam_energy', 6500, default='{0}', desc='energy of beam 1 in GeV', parameter='ebeam1' )
    self.decorated.add_parameter( 'beam_energy', 6500, default='{0}', desc='energy of beam 2 in GeV', parameter='ebeam2' )
    # PDF information
    self.decorated.add_parameter( 'PDF', 10800,     default='{0}', desc='PDF set for hadron 1 (LHAGLUE numbering)', parameter='lhans1' )
    self.decorated.add_parameter( 'PDF', 10800,     default='{0}', desc='PDF set for hadron 2 (LHAGLUE numbering)', parameter='lhans2' )
    self.decorated.fix_parameter( 'pdfreweight', 1, default='{0}', desc='(0:disabled; 1:enabled) store PDF information' )
    # Random seeds
    self.decorated.add_parameter( 'manyseeds', 0,   default='{0}', desc='(0:disabled; 1:enabled) read multiple seeds for the random number generator from pwgseeds.dat' )
    self.decorated.fix_parameter( 'rand1', 0,       default='{0}', desc='(0:disabled; 1:enabled) user-initiated random seed (disabled for reproducibility)' )
    self.decorated.fix_parameter( 'rand2', 0,       default='{0}', desc='(0:disabled; 1:enabled) user-initiated random seed (disabled for reproducibility)' )
    self.decorated.add_parameter( 'random_seed', 1, default='{0}', desc='seed for the random number generator', parameter='iseed' )
    # Scale evaluation
    self.decorated.add_parameter( 'mu_F', 1.0, default='{0}', desc='factorization scale factor: mu_fact = mu_ref * facscfact', parameter='facscfact' )
    self.decorated.add_parameter( 'mu_R', 1.0, default='{0}', desc='renormalization scale factor: mu_ren = mu_ref * renscfact', parameter='renscfact' )
    # Integration parameters:
    #   The total number of calls is ncall2*itmx2*foldcsi*foldy*foldphi, with a typical call using 1/1400 seconds
    # These are optimised in each process to ensure:
    #   Cross section uncertainty < 1% : to reduce, increase ncall1*itmx1 or ncall2*itmx2
    #   Negative weight events    < 1% : to reduce, increase fold parameters
    #   Upper bound failures      < 1% : to reduce, increase nubound, xupbound or ncall2*itmx2
    self.decorated.add_parameter( 'itmx1', 5,      default='{0}', desc='number of iterations for initializing the integration grid' )
    self.decorated.add_parameter( 'itmx2', 5,      default='{0}', desc='number of iterations for computing the integral and finding upper bound' )
    self.decorated.add_parameter( 'ncall1', 10000, default='{0}', desc='number of calls for initializing the integration grid' )
    self.decorated.add_parameter( 'ncall2', 10000, default='{0}', desc='number of calls for computing the integral and finding upper bound' )
    # Allowed values for folding parameters are 1, 2, 5, 10, 25, 50
    self.decorated.add_parameter( 'foldx', 1,        default='{0}', desc='number of folds on x (csi) integration', parameter='foldcsi' )
    self.decorated.add_parameter( 'foldy', 1,        default='{0}', desc='number of folds on y integration' )
    self.decorated.add_parameter( 'foldphi', 1,      default='{0}', desc='number of folds on phi integration' )
    self.decorated.fix_parameter( 'use-old-grid', 1, default='{0}', desc='(0:disabled; 1:enabled) use old integration grid if available' )
    # Born suppression : may be needed in presence of processes where the Born cross section vanishes in some phase-space region
    self.decorated.add_parameter( 'bornktmin', -1,                         default='{0}', desc='(-1:use Powheg default) generation cut: minimum kt in underlying Born' )
    self.decorated.fix_parameter( 'bornonly', [0,1][self.decorated.is_LO], default='{0}', desc='(0:disabled; 1:enabled) calculate only Born-level process' )
    self.decorated.add_parameter( 'bornsuppfact', -1,                      default='{0}', desc='(-1:use Powheg default) mass parameter for Born suppression factor. If > 0 suppfact = 1' )
    self.decorated.add_parameter( 'bornzerodamp', 1,                       default='{0}', desc='(0:disabled; 1:enabled) use damping where the Born is strongly suppressed (or 0).' )
    self.decorated.add_parameter( 'hdamp', -1,                             default='{0}', desc='(-1:use Powheg default) DEPRECATED, apply damping factor for high-pt radiation: h**2/(pt2+h**2)' )
    self.decorated.add_parameter( 'hfact', -1,                             default='{0}', desc='(-1:use Powheg default) apply dumping factor for high-pt radiation: h**2/(pt2+h**2)' )
    self.decorated.add_parameter( 'ptsupp', -1,                            default='{0}', desc='(-1:use Powheg default) DEPRECATED, but some processes complain if it is missing' )
    self.decorated.add_parameter( 'withdamp', -1,                          default='{0}', desc='(-1:use Powheg default) use Born-zero damping factor.' )
    self.decorated.add_parameter( 'withnegweights', 1,                     default='{0}', desc='(0:disabled; 1:enabled) allow negative weights' )
    # Parton splitting settings
    self.decorated.add_parameter( 'bottomthr', -1,     default='{0}', desc='(-1:use Powheg default) minimum pT in GeV for generating emission off b-quarks' )
    self.decorated.add_parameter( 'bottomthrpdf', -1,  default='{0}', desc='(-1:use Powheg default) threshold in GeV at which b-quark PDF becomes non-zero' )
    self.decorated.add_parameter( 'charmthr', -1,      default='{0}', desc='(-1:use Powheg default) minimum pT in GeV for generating emission off c-quarks' )
    self.decorated.add_parameter( 'charmthrpdf', -1,   default='{0}', desc='(-1:use Powheg default) threshold in GeV at which c-quark PDF becomes non-zero' )
    self.decorated.add_parameter( 'par_2gsupp', -1,    default='{0}', desc='(-1:use Powheg default)' )
    self.decorated.add_parameter( 'par_diexp', -1,     default='{0}', desc='(-1:use Powheg default) ISR singularity exponent (p1)' )
    self.decorated.add_parameter( 'par_dijexp', -1,    default='{0}', desc='(-1:use Powheg default) FSR singularity exponent (p2)' )
    self.decorated.add_parameter( 'ptsqmin', -1,       default='{0}', desc='(-1:use Powheg default) minimum pT in GeV for generating emission off light quarks' )
    # Users are asked not to change these, since their invocation is time consuming and/or may cause some conflicts with other settings.
    self.decorated.fix_parameter( 'iupperisr', 1,  default='{0}', desc='choice of ISR upper bounding functional form' )
    self.decorated.fix_parameter( 'iupperfsr', 2,  default='{0}', desc='choice of FSR upper bounding functional form' )
    self.decorated.fix_parameter( 'smartsig', 1,   default='{0}', desc='(0:disabled; 1:enabled) remember equal amplitudes' )
    self.decorated.fix_parameter( 'testplots', 0,  default='{0}', desc='(0:disabled; 1:enabled) do NLO and PWHG distributions' )
    self.decorated.fix_parameter( 'testsuda', 0,   default='{0}', desc='(0:disabled; 1:enabled) test Sudakov form factor' )
    self.decorated.fix_parameter( 'withsubtr', 1,  default='{0}', desc='(0:disabled; 1:enabled) subtract real counterterms' )
    # Parameters of unknown use
    self.decorated.add_parameter( 'flg_debug', 0,       default='{0}', desc='(0:disabled; 1:enabled) write extra information to LHEF. Breaks PYTHIA showering.' )
    self.decorated.add_parameter( 'colltest', 1,        default='{0}', desc='(0:disabled; 1:enabled) check collinear limits' )
    self.decorated.add_parameter( 'softtest', 1,        default='{0}', desc='(0:disabled; 1:enabled) check soft limits' )
    self.decorated.add_parameter( 'ubsigmadetails', 1,  default='{0}', desc='(0:disabled; 1:enabled) output calculated cross-sections' )
    # Radiation settings for NLO processes
    if not self.decorated.is_LO :
      self.decorated.add_parameter( 'btlscalect', 0,     default='{0}', desc='(0:disabled; 1:enabled) use the scales of the underlying-Born configuration for the subtraction terms' )
      self.decorated.add_parameter( 'btlscalereal', 0,   default='{0}', desc='(0:disabled; 1:enabled) compute scales that depend on the real kinematics' )
      self.decorated.add_parameter( 'ixmax', 1,          default='{0}', desc='number of intervals (<= 10) in x (csi) grid to compute upper bounds', parameter='icsimax' )
      self.decorated.add_parameter( 'iymax', 1,          default='{0}', desc='number of intervals (<= 10) in y grid to compute upper bounds' )
      self.decorated.add_parameter( 'nubound', 10000,    default='{0}', desc='number of calls to setup upper bounds for radiation' )
      self.decorated.add_parameter( 'radregion', -1,     default='{0}', desc='(-1:all regions) only generate radiation in the selected singular region' )
      self.decorated.fix_parameter( 'use-old-ubound', 1, default='{0}', desc='(0:disabled; 1:enabled) read norm of upper bounding function from pwgubound.dat, if present' )
      self.decorated.add_parameter( 'xupbound', 2,       default='{0}', desc='increase upper bound for radiation generation by this factor' )

  def finalise(self) :
    # Force parameters to integer values
    [ setattr( self.decorated, parameter, int( getattr(self.decorated, parameter) ) ) for parameter in ('nEvents','itmx1','itmx2','ncall1','ncall2','foldx','foldy','foldphi','random_seed') ]

    # Fix parameters read from Generate_tf before printing list for user
    [ self.decorated.fix_parameter( parameter ) for parameter in ('beam_energy', 'manyseeds', 'random_seed' ) ]
