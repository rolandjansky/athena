# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to configure Powheg subprocesses
#
#   Authors: James Robinson  <james.robinson@cern.ch>
#            Daniel Hayden   <danhayden0@googlemail.com>
#            Stephen Bieniek <stephen.paul.bieniek@cern.ch>
#
#########################################################################################################################

#! /usr/bin/env python
import os, select, subprocess, threading, time
from AthenaCommon import Logging
from strategies import *
from utility import RepeatingTimer

## Base class for configurable objects in the jobOptions
#
#  All subprocesses inherit from this class
class PowhegConfig_base(object) :
  ## Setup athena-compatible logger
  __logger = Logging.logging.getLogger('PowhegControl')

  ## This needs to be set so that Generate_trf finds an appropriate file format for showering
  # It should not be changed by end-users!
  __output_events_file_name = 'PowhegOTF._1.events'

  ## This must be defined by each derived class - don't change it in the jobOptions!
  _powheg_executable = os.environ['POWHEGPATH']

  ## Switch to determine which version of Powheg this process belongs to
  _powheg_version_type = 1

  ## These values can be changed in the jobOptions - defaults are set here.
  # Process-specific options are in the relevant decorator/class

  ## Beam/event information
  nEvents        = 5500
  beam_energy    = 4000
  PDF            = 10800
  mu_F           = 1.0
  mu_R           = 1.0

  ### Generation setup
  manyseeds    = -1
  randomSeed   = 1
  use_old_grid = 1
  __n_cores    = 1 # Number of subjobs to run in manyseeds mode -- reset by ATHENA_PROC_NUMBER

  ## Integration parameters (reasonable defaults)
  # These are optimised in each process to ensure:
  #   Upper bound failures      < 1% : to reduce, increase nubound, xupbound or ncall2*itmx2
  #   Cross section uncertainty < 1% : to reduce, increase ncall1*itmx1 or ncall2*itmx2
  #   Negative weight events    < 1% : to reduce, increase fold parameters
  ncall1  = 20000
  itmx1   = 5
  ncall2  = 100000
  itmx2   = 5
  # allowed values for folding parameters are 1, 2, 5, 10, 25, 50
  foldx   = 1
  foldy   = 1
  foldphi = 1

  # Born suppression : may be needed in presence of processes where the Born cross section vanishes in some phase-space region
  bornktmin      = -1
  bornsuppfact   = -1
  hdamp          = -1
  hfact          = -1
  withdamp       = 0
  withnegweights = 0

  # Parton splitting settings
  bottomthr    = 5.0
  bottomthrpdf = 5.0
  charmthr     = 1.5
  charmthrpdf  = 1.5
  par_2gsupp   = 1
  par_diexp    = 1
  par_dijexp   = 1
  ptsqmin      = 0.8

  # Set up run card decorator and current directory
  run_card_decorators = []
  base_directory = os.environ['PWD']


  def __init__( self, runArgs=None, opts=None ) :
    # Initialise values from runArgs
    if runArgs == None :
      self.logger().warning( 'No run arguments found! Using defaults.' )
    else :
      # Read values from runArgs
      if hasattr(runArgs,'ecmEnergy') :
        self.beam_energy = 0.5 * runArgs.ecmEnergy
      if hasattr(runArgs,'maxEvents') and runArgs.maxEvents > 0 :
        self.nEvents = int( 1.1 * runArgs.maxEvents + 0.5 )
      if hasattr(runArgs,'randomSeed') :
        self.randomSeed = runArgs.randomSeed
      # Set inputGeneratorFile to match output events file. Otherwise Generate_trf check will fail.
      runArgs.inputGeneratorFile = self.__output_events_file_name

    # Enable parallel mode if AthenaMP mode is enabled
    if 'ATHENA_PROC_NUMBER' in os.environ :
      self.logger().info( 'This job is running with an athena MP-like whole-node setup. Will re-configure now to make sure that the remainder of the job runs serially.' )
      self.__n_cores = int( os.environ.pop('ATHENA_PROC_NUMBER') )
      self.manyseeds = 1
      self.logger().info( 'Scaling number of events per job from {0} down to {1}'.format( self.nEvents, int(self.nEvents / self.cores() + 0.5) ) )
      # Try to modify the transform opts to suppress athenaMP mode
      if hasattr(opts,'nprocs') : opts.nprocs = 0
      else : self.logger().warning( 'No "nprocs" options provided!')

  ###############################################################################
  #
  #  Get number of cores
  #
  ###############################################################################
  def cores( self ) :
    return self.__n_cores

  ###############################################################################
  #
  #  Get handle to logger
  #
  ###############################################################################
  def logger( self ) :
    return self.__logger

  ###############################################################################
  #
  #  Get full path to runcard
  #
  ###############################################################################
  def runcard_path( self ) :
    return str(self.base_directory)+'/powheg.input'

  ###############################################################################
  #
  #  Initialise runcard with generic options
  #
  ###############################################################################
  def initialiseRunCard( self ) :
    self.logger().info( 'Writing Powheg runcard to {0}'.format( self.runcard_path() ) )

    # Perform some sanity checks
    if self.withnegweights == 1 and self.foldx * self.foldy * self.foldphi != 1 :
      # Performing a folded integration reduces the proportion of negative weights. Not necessarily needed if using (accepting) negative weights
      self.logger().warning( 'Accepting negative weights while using folding to reduce negative weight fraction! This may not be necessary.' )
    if (self.bornsuppfact > 0.0) and (self.bornktmin <= 0.0) :
      self.logger().warning( 'These settings: bornsuppfact({0}) and bornktmin({1}) cannot be used to generate events! Only fixed-order distributions should be produced with these settings!'.format(self.bornsuppfact,self.bornktmin) )

    # Scale-down number of events produced in each run if running in multicore mode
    if self.cores() > 1 :
      self.parallelstage = 1
      self.xgriditeration = 1
      self.nEvents = int( self.nEvents / self.cores() + 0.5 )
      self.ncall1 = int(  self.ncall1 / self.cores() + 0.5 )
      self.ncall2 = int( self.ncall2 / self.cores() + 0.5 )
      self.nubound = int( self.nubound / self.cores() + 0.5  )
      # Set ncall1rm if available for this process
      try : self.ncall1rm = int( self.ncall1rm / self.cores() + 0.5 )
      except AttributeError : pass

    # Set starting value to first in list when multiple values are provided
    PDF = self.PDF[0] if isinstance(self.PDF,list) else self.PDF
    if isinstance(self.mu_F,list) : # Re-order list if 1.0 is not first
      self.mu_F.insert(0, self.mu_F.pop(self.mu_F.index(1.0)))
      mu_F = self.mu_F[0]
    else :
      mu_F = self.mu_F
    if isinstance(self.mu_R,list) : # Re-order list if 1.0 is not first
      self.mu_R.insert(0, self.mu_R.pop(self.mu_R.index(1.0)))
      mu_R = self.mu_R[0]
    else :
      mu_R = self.mu_R

    # Write output to runcard
    with open( self.runcard_path(), 'w' ) as f :
      # Beam/event information
      f.write( 'numevts '+str(self.nEvents)+'                     ! number of events to be generated\n' )
      f.write( 'ih1 1                                             ! hadron 1 type\n' )
      f.write( 'ih2 1                                             ! hadron 2 type\n' )
      f.write( 'ebeam1 '+str(self.beam_energy)+'                  ! energy of beam 1 in GeV\n' )
      f.write( 'ebeam2 '+str(self.beam_energy)+'                  ! energy of beam 2 in GeV\n' )
      f.write( 'lhans1 '+str(PDF)+'                               ! PDF set for hadron 1 (LHAGLUE numbering)\n' )
      f.write( 'lhans2 '+str(PDF)+'                               ! PDF set for hadron 2 (LHAGLUE numbering)\n' )
      f.write( 'facscfact '+str(mu_F)+'                           ! factorization scale factor: mufact=muref*facscfact\n' )
      f.write( 'renscfact '+str(mu_R)+'                           ! renormalization scale factor: muren=muref*renscfact\n' )
      # Generation setup
      f.write( 'iseed '+str(self.randomSeed)+'                    ! seed for the random number generator\n' )
      f.write( 'manyseeds '+str(self.manyseeds)+'                 ! read multiple seeds for the random number generator from pwgseeds.dat\n' )
      f.write( 'pdfreweight 1                                     ! store PDF information\n' )
      f.write( 'rand1 -1                                          ! user-initiated random seed (disabled for reproducibility)\n' )
      f.write( 'rand2 -1                                          ! user-initiated random seed (disabled for reproducibility)\n' )
      f.write( 'use-old-grid '+str(self.use_old_grid)+'           ! if 1 use old grid if file pwggrids.dat is present (<> 1 regenerate)\n' )
      # Integration parameters:
      #   A typical call uses 1/1400 seconds (1400 calls per second)
      #   The total number of calls is ncall2*itmx2*foldcsi*foldy*foldphi
      f.write( 'itmx1 '+str(self.itmx1)+'                         ! number of iterations for initializing the integration grid\n' )
      f.write( 'itmx2 '+str(self.itmx2)+'                         ! number of iterations for computing the integral and finding upper bound\n' )
      f.write( 'ncall1 '+str(self.ncall1)+'                       ! number of calls for initializing the integration grid\n' )
      f.write( 'ncall2 '+str(self.ncall2)+'                       ! number of calls for computing the integral and finding upper bound\n' )
      f.write( 'foldcsi '+str(self.foldx)+'                       ! number of folds on x (csi) integration\n' )
      f.write( 'foldy '+str(self.foldy)+'                         ! number of folds on y integration\n' )
      f.write( 'foldphi '+str(self.foldphi)+'                     ! number of folds on phi integration\n' )
      # Born suppression : may be needed in presence of processes where the Born cross section vanishes in some phase-space region
      f.write( 'bornktmin '+str(self.bornktmin)+'                 ! (default -1) Generation cut: minimum kt in underlying Born\n' )
      f.write( 'bornsuppfact '+str(self.bornsuppfact)+'           ! (default -1) Mass parameter for Born suppression factor. If > 0 suppfact = 1\n' )
      f.write( 'hdamp '+str(self.hdamp)+'                         ! Born-zero damping factor \n' )
      f.write( 'hfact '+str(self.hfact)+'                         ! (default no dumping factor) dump factor for high-pt radiation: > 0 dumpfac=h**2/(pt2+h**2)\n' )
      f.write( 'ptsupp -1                                         ! deprecated, but some processes complain if it is missing\n' )
      f.write( 'withdamp '+str(self.withdamp)+'                   ! (default 0, do not use) use Born-zero damping factor\n' )
      f.write( 'withnegweights '+str(self.withnegweights)+'       ! allow negative weights\n' )
      # Parton splitting settings
      f.write( 'bottomthr '+str(self.bottomthr)+'                 ! (default 5.0 GeV) bottom threshold for gluon splitting\n' )
      f.write( 'bottomthrpdf '+str(self.bottomthrpdf)+'           ! (default 5.0 GeV) PDF bottom threshold\n' )
      f.write( 'charmthr '+str(self.charmthr)+'                   ! (default 1.5 GeV) charm threshold for gluon splitting\n' )
      f.write( 'charmthrpdf '+str(self.charmthrpdf)+'             ! (default 1.5 GeV) PDF charm threshold\n' )
      f.write( 'par_2gsupp '+str(self.par_2gsupp)+'               ! \n' )
      f.write( 'par_diexp '+str(self.par_diexp)+'                 ! (default 1) ISR singularity exponent (p1)\n' )
      f.write( 'par_dijexp '+str(self.par_dijexp)+'               ! (default 1) FSR singularity exponent (p2)\n' )
      f.write( 'ptsqmin '+str(self.ptsqmin)+'                     ! (default 0.8 GeV) minimum pt for generation of radiation\n' )
      # Users are asked not to change these, since their invocation is time consuming and/or may cause some conflicts with other settings.
      f.write( 'bornonly 0                                        ! (default 0) if 1 do Born only\n' )
      f.write( 'iupperisr 1                                       ! (default 1) choice of ISR upper bounding functional form\n' )
      f.write( 'iupperfsr 2                                       ! (default 2) choice of FSR upper bounding functional form\n' )
      f.write( 'smartsig 1                                        ! (default 1) remember equal amplitudes (0 do not remember)\n' )
      f.write( 'testplots 0                                       ! (default 0, do not) do NLO and PWHG distributions\n' )
      f.write( 'testsuda 0                                        ! (default 0, do not test) test Sudakov form factor\n' )
      f.write( 'withsubtr 1                                       ! (default 1) subtract real counterterms (0 do not subtract)\n' )
      # Parameters of unknown use
      f.write( 'bornzerodamp -1                                   ! (default -1)\n' )
      f.write( 'flg_debug -1                                      ! (default -1) write extra information on LHEF. Breaks PYTHIA showering.\n' )
      f.write( 'colltest -1                                       ! (default -1)\n' )
      f.write( 'softtest -1                                       ! (default -1)\n' )
      f.write( 'ubsigmadetails -1                                 ! (default -1)\n' )

    # Add registered decorators
    for run_card_decorator in self.run_card_decorators :
      run_card_decorator.append_to_run_card()


  ###############################################################################
  #
  #  Run normal event generation
  #
  ###############################################################################
  def generate(self, filter_name='', filter_args='') :
    self.generateRunCard()
    if filter_name == '' :
      self.generateEvents()
    else :
      self.generateFilteredEvents( filter_name, filter_args )

  ###############################################################################
  #
  #  Must be implemented by children
  #
  ###############################################################################
  def generateRunCard(self) :
    raise NotImplementedError('Please implement this method!')


  ###############################################################################
  #
  #  Run normal event generation
  #
  ###############################################################################
  def generateEvents(self) :
    self.logger().info( 'Starting Powheg LHEF event generation at '+str(time.asctime()) )

    # Remove any existing .lhe files to avoid repeated events
    try :
      os.remove( '*.lhe *.ev*ts' )
    except OSError :
      pass

    # Generate the events in the external Powheg process
    time_start = time.time()
    self.logger().info( 'Running ' + str(self._powheg_executable) )

    # Setup heartbeat thread
    heartbeat = RepeatingTimer( 600., lambda: self.logger().info('Heartbeat: Powheg generation has been running for {0} in total'.format( RepeatingTimer.human_readable_time_interval(time.time() - time_start)) ) )
    heartbeat.daemon = True # Allow program to exit if this is the only live thread
    heartbeat.start()

    # Initialise generation processes
    self.running_processes = []

    # Check whether multiple PDFs have been specified
    try : multiple_PDFs_or_scales = isinstance(self.PDF,list) or (isinstance(self.mu_F,list) and isinstance(self.mu_R,list))
    except TypeError : multiple_PDFs_or_scales = False

    if self.manyseeds < 1  :
      # Normal run
      if not multiple_PDFs_or_scales :
        runPowhegSingleThread(self)
      # Multiple PDFs/scales
      else :
        runPowhegV2Multiweight(self)
    else :
      if multiple_PDFs_or_scales : self.logger().warning( 'Cannot combine multicore running with multiple weight rerunning!' )
      # Run v1-style multiprocess (only needs one step)
      if self._powheg_version_type == 1 :
        runPowhegV1Multicore(self)
      # Run v2-style multiprocess (needs four steps)
      else :
        runPowhegV2Multicore(self)

    # Display generation output until finished then kill heartbeat thread
    heartbeat.cancel()

    # Print timing information
    elapsed_time = time.time() - time_start
    self.logger().info( 'Running Powheg took {0} for {1} events => {2:6.3f} Hz'.format( RepeatingTimer.human_readable_time_interval(elapsed_time), self.nEvents, self.nEvents / elapsed_time ) )

    # Concatenate output events if running in multicore mode
    if self.manyseeds >= 1 :
      self.logger().info( 'Concatenating {0} output LHE files'.format( self.cores() ) )
      subprocess.call( 'cat pwgevents*.lhe > pwgevents.lhe', shell=True ) # NB. shell=True is unsafe if combined with user input

    # Move output to correctly named file
    try :
      os.rename( 'pwgevents.lhe', self.__output_events_file_name )
      self.logger().info( 'Moved pwgevents.lhe to {0}'.format(self.__output_events_file_name) )
    except OSError :
      print os.listdir('.')
      self.logger().warning( 'No pwgevents.lhe found!' )

    # Print finalisation message
    self.logger().info( 'Finished at {0}'.format( time.asctime() ) )
    return


  ###############################################################################
  #
  #  Run filtered event generation
  #
  ###############################################################################
  def generateFilteredEvents(self, filter_name, filter_args) :
    self.logger().info( 'Starting Powheg LHEF event generation at {0}'.format( time.asctime() ) )
    filter_executable = os.environ['LHEFPATH'] + '/' + filter_name
    filter_args.insert( 0, self.__output_events_file_name )
    filter_args.insert( 0, 'pwgevents.lhe' )
    filter_args.insert( 0, str(filter_executable) )

    # Remove any existing .lhe files to avoid repeated events
    try :
      os.remove( '*.lhe *.ev*ts' )
    except OSError :
      pass

    # Generate the events
    self.logger().info( 'Running ' + str(self._powheg_executable) )

    # Start FIFO
    setuppipe = subprocess.Popen( ['mkfifo','pwgevents.lhe'] )
    setuppipe.wait()

    # Start generation process
    generate = subprocess.Popen( [self._powheg_executable,''] )

    # Start filter and wait until quota has been filled. LHEF_filter moves output to correctly named file
    self.logger().info( 'Running filter with commmand: {0}'.format( filter_executable ) )
    LHEF_filter = subprocess.Popen( filter_args )
    LHEF_filter.wait()
    LHEF_filter_output = LHEF_filter.communicate()
    self.logger().info( LHEF_filter_output )

    # Once quota filled, kill generation process
    generate.terminate()

    # Finish
    self.logger().info( 'Finished at {0}'.format( time.asctime() ) )
    return
