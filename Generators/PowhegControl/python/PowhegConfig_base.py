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
import os, time, subprocess, threading
from AthenaCommon import Logging
from RepeatingTimer import RepeatingTimer


class PowhegConfig_base(object) :
  _logger = Logging.logging.getLogger('PowhegControl')
  # This needs to be set so that Generate_trf finds an appropriate file format for showering
  # It should not be changed by end-users!
  __output_events_file_name = 'PowhegOTF._1.events'
  # This must be defined by each derived class - don't change it in the jobOptions!
  _powheg_executable = os.environ['POWHEGPATH']

  # These values can be changed in the jobOptions - defaults are set here.
  # Process-specific options are in the relevant class
  # Powheg can use a different PDF for each proton; we impose the same value for each one: (CT10 by default)
  runArgs        = None
  nEvents        = 5500
  beam_energy    = 4000
  randomSeed     = 1
  mu_F, mu_R     = 1.0, 1.0
  PDF            = 10800
  # Enable PDF information by default
  # 0 - disables PDF information
  # 1 - produces nominal sample     (v2 only)
  # 2 - produces reweighted sample  (v2 only)
  PDF_info_type = 1

  # Weighted-event generation is disabled by default
  withnegweights = 0
  bornsuppfact   = -1
  bornktmin      = -1

  # Parameters to allow/disallow use of stored data
  use_old_grid, use_old_ubound = 1, 1

  # Integration parameters (reasonable defaults)
  # These are optimised in each process to ensure:
  #   Upper bound failures      < 1% : to reduce, increase nubound/xupbound, ncall2*itmx2
  #   Cross section uncertainty < 1% : to reduce, increase ncall1*itmx1 / ncall2*itmx2
  #   Negative weight events    < 1% : to reduce, increase fold parameters
  ncall1, ncall2        = 20000, 100000
  nubound, xupbound     = 100000, 2
  itmx1, itmx2          = 5, 5
  ixmax, iymax          = 1, 1
  foldx, foldy, foldphi = 1, 1, 1 # allowed values for folding parameters are 1, 2, 5, 10, 25, 50

  # Parton splitting settings
  par_diexp, par_dijexp, par_2gsupp = -1, -1, -1

  # Other universal parameters
  ptsqmin                 = 0.8
  charmthr, charmthrpdf   = 1.5, 1.5
  bottomthr, bottomthrpdf = 5.0, 5.0
  manyseeds               = -1

  # These may be needed in presence of processes where the Born cross section vanishes in some phase-space region
  withdamp                = 0
  hdamp                   = -1
  hfact                   = -1

  # LHEF filter and directory setting
  filter_name, filter_args = '', ''
  TestArea = os.environ['PWD']

  # Set up run card decorator
  run_card_decorators = []

  def __init__( self, runArgs=None ) :
    self.runArgs = runArgs
    if runArgs == None :
      self._logger.warning( 'No run arguments found! Using defaults.' )
    else :
      # Read values from runArgs
      if hasattr(self.runArgs,'ecmEnergy') :
        self.beam_energy = 0.5 * self.runArgs.ecmEnergy
      if hasattr(self.runArgs,'maxEvents') :
        if self.runArgs.maxEvents > 0 :
          self.nEvents = int( 1.1 * self.runArgs.maxEvents + 0.5 )
      if hasattr(self.runArgs,'randomSeed') :
        self.randomSeed = self.runArgs.randomSeed
      # Set inputGeneratorFile to match output events file. Otherwise Generate_trf check will fail.
      self.runArgs.inputGeneratorFile = self.__output_events_file_name

  ###############################################################################
  #
  #  Initialise runcard with generic options
  #
  ###############################################################################
  def initialiseRunCard( self ) :
    self._logger.info( 'Writing Powheg runcard to {0}/powheg.input'.format(self.TestArea) )

    # Perform some sanity checks
    if self.withnegweights == 1 and self.foldx * self.foldy * self.foldphi != 1 :
      # Performing a folded integration reduces the proportion of negative weights
      # Not needed if using (accepting) negative weights
      self.foldx, self.foldy, self.foldphi = 1, 1, 1
      self._logger.warning( 'Requesting negative weights while using folding parameters! This is suboptimal: setting folding parameters to foldx({0}) foldy({1}) foldphi({2})'.format(self.foldx,self.foldy,self.foldphi) )
    if (self.bornsuppfact > 0.0) and (self.bornktmin <= 0.0) :
      self._logger.warning( 'These settings: bornsuppfact({0}) and bornktmin({1}) cannot be used to generate events! Only a fixed-order distribution will be produced!'.format(self.bornsuppfact,self.bornktmin) )

    # Write output to runcard
    with open( str(self.TestArea)+'/powheg.input','w') as f :
      # Beam/event information
      f.write( 'numevts '+str(self.nEvents)+'                     ! number of events to be generated\n' )
      f.write( 'ih1 1                                             ! hadron 1 type\n' )
      f.write( 'ih2 1                                             ! hadron 2 type\n' )
      f.write( 'lhans1 '+str(self.PDF)+'                          ! PDF set for hadron 1 (LHAGLUE numbering)\n' )
      f.write( 'lhans2 '+str(self.PDF)+'                          ! PDF set for hadron 2 (LHAGLUE numbering)\n' )
      f.write( 'ebeam1 '+str(self.beam_energy)+'                  ! energy of beam 1 in GeV\n' )
      f.write( 'ebeam2 '+str(self.beam_energy)+'                  ! energy of beam 2 in GeV\n' )
      f.write( 'iseed '+str(self.randomSeed)+'                    ! start the random number generator with seed iseed\n' )
      f.write( 'rand1 -1                                          ! user-initiated random seed (disabled for reproducibility)\n' )
      f.write( 'rand2 -1                                          ! user-initiated random seed (disabled for reproducibility)\n' )
      f.write( 'manyseeds '+str(self.manyseeds)+'                 ! allow multiple simultaneous jobs - not allowed on the grid\n' )
      # Generation setup
      f.write( 'withnegweights '+str(self.withnegweights)+'       ! allow negative weights\n' )
      f.write( 'facscfact '+str(self.mu_F)+'                      ! factorization scale factor: mufact=muref*facscfact\n' )
      f.write( 'renscfact '+str(self.mu_R)+'                      ! renormalization scale factor: muren=muref*renscfact\n' )
      # PDF reweighting information
      if self.PDF_info_type != 0 :
        f.write( 'pdfreweight 1                                   ! store PDF information\n' )
      # Parameters to allow/disallow use of stored data
      f.write( 'use-old-grid '+str(self.use_old_grid)+'           ! if 1 use old grid if file pwggrids.dat is present (<> 1 regenerate)\n' )
      f.write( 'use-old-ubound '+str(self.use_old_ubound)+'       ! if 1 use norm of upper bounding function stored in pwgubound.dat, if present; <> 1 regenerate\n' )
      # Integration parameters: A typical call uses 1/1400 seconds (1400 calls per second)
      #   NB. the total number of calls is ncall2*itmx2*foldcsi*foldy*foldphi
      #       with the default setting of ncall2*itmx2*foldcsi*foldy*foldphi=5M, 60 minutes
      #       these folding numbers yield a negative fraction of 0.5% with bornktmin=10 GeV
      f.write( 'ncall1 '+str(self.ncall1)+'                       ! number of calls for initializing the integration grid\n' )
      f.write( 'itmx1 '+str(self.itmx1)+'                         ! number of iterations for initializing the integration grid: total 100000 calls ~ 70 seconds\n' )
      f.write( 'ncall2 '+str(self.ncall2)+'                       ! number of calls for computing the integral and finding upper bound\n' )
      f.write( 'itmx2 '+str(self.itmx2)+'                         ! number of iterations for the above\n' )
      f.write( 'foldx '+str(self.foldx)+'                         ! number of folds on x integration\n' )
      f.write( 'foldcsi '+str(self.foldx)+'                       ! (alternate naming in some processes)\n' )
      f.write( 'foldy '+str(self.foldy)+'                         ! number of folds on y integration\n' )
      f.write( 'foldphi '+str(self.foldphi)+'                     ! number of folds on phi integration\n' )
      f.write( 'nubound '+str(self.nubound)+'                     ! number of calls to setup upper bounds for radiation\n' )
      f.write( 'xupbound '+str(self.xupbound)+'                   ! increase upper bound for radiation generation by this factor\n' )
      f.write( 'ixmax '+str(self.ixmax)+'                         ! <= 10, number of intervals in x grid to compute upper bounds\n' )
      f.write( 'icsimax '+str(self.ixmax)+'                       ! (alternate naming in some processes)\n' )
      f.write( 'iymax '+str(self.iymax)+'                         ! <= 10, number of intervals in y grid to compute upper bounds\n' )
      # Born suppression
      f.write( 'bornktmin '+str(self.bornktmin)+'                 ! (default -1) Generation cut: minimum kt in underlying Born\n' )
      f.write( 'bornsuppfact  '+str(self.bornsuppfact)+'          ! (default -1) Mass parameter for Born suppression factor. If > 0 suppfact = 1\n' )
      f.write( 'ptsupp -1                                         ! deprecated, but some processes complain if it is missing\n' )
      # Parton splitting settings
      f.write( 'par_diexp '+str(self.par_diexp)+'                 !\n' )
      f.write( 'par_dijexp '+str(self.par_dijexp)+'               !\n' )
      f.write( 'par_2gsupp '+str(self.par_2gsupp)+'               !\n' )
      # Other parameters
      f.write( 'ptsqmin '+str(self.ptsqmin)+'                     ! (default 0.8 GeV) minimum pt for generation of radiation\n' )
      f.write( 'charmthr '+str(self.charmthr)+'                   ! (default 1.5 GeV) charm threshold for gluon splitting\n' )
      f.write( 'bottomthr '+str(self.bottomthr)+'                 ! (default 5.0 GeV) bottom threshold for gluon splitting\n' )
      f.write( 'charmthrpdf '+str(self.charmthrpdf)+'             ! (default 1.5 GeV) PDF charm threshold\n' )
      f.write( 'bottomthrpdf '+str(self.bottomthrpdf)+'           ! (default 5.0 GeV) PDF bottom threshold\n' )
      # These may be needed in presence of processes where the Born cross section vanishes in some phase-space region
      f.write( 'withdamp '+str(self.withdamp)+'                   ! (default 0, do not use) use Born-zero damping factor\n' )
      f.write( 'hdamp '+str(self.hdamp)+'                         ! Born-zero damping factor \n' )
      f.write( 'hfact '+str(self.hfact)+'                         ! (default no dumping factor) dump factor for high-pt radiation: > 0 dumpfac=h**2/(pt2+h**2)\n' )
      # Users are asked not to change these, since their invocation is time consuming and/or may cause some conflicts with other settings.
      f.write( 'testsuda 0                                        ! (default 0, do not test) test Sudakov form factor\n' )
      f.write( 'testplots 0                                       ! (default 0, do not) do NLO and PWHG distributions\n' )
      f.write( 'bornonly 0                                        ! (default 0) if 1 do Born only\n' )
      f.write( 'smartsig 1                                        ! (default 1) remember equal amplitudes (0 do not remember)\n' )
      f.write( 'withsubtr 1                                       ! (default 1) subtract real counterterms (0 do not subtract)\n' )
      f.write( 'radregion -1                                      ! (default all regions) only generate radiation in the selected singular region\n' )
      f.write( 'iupperisr 1                                       ! (default 1) choice of ISR upper bounding functional form\n' )
      f.write( 'iupperfsr 2                                       ! (default 2) choice of FSR upper bounding functional form\n' )
      # Parameters of unknown use
      f.write( 'flg_debug -1                                      ! (default -1) write extra information on LHEF. Breaks PYTHIA showering.\n' )
      f.write( 'btlscalereal -1                                   ! (default -1)\n' )
      f.write( 'btlscalect -1                                     ! (default -1)\n' )
      f.write( 'ubsigmadetails -1                                 ! (default -1)\n' )
      f.write( 'bornzerodamp -1                                   ! (default -1)\n' )
      f.write( 'softtest -1                                       ! (default -1)\n' )
      f.write( 'colltest -1                                       ! (default -1)\n' )

    # Add registered decorators
    for run_card_decorator in self.run_card_decorators :
      run_card_decorator.append_to_run_card()


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
    self._logger.info( 'Starting Powheg LHEF event generation at '+str(time.asctime()) )

    # Remove any existing .lhe files to avoid repeated events
    if self.PDF_info_type == 2 :
      self._logger.warning( 'Not attempting to remove existing LHE files due to reweighting settings! This could cause problems.' )
    else :
      try :
        os.remove( '*.lhe' )
      except OSError :
        pass

    # Generate the events in the external Powheg process
    time_start = time.time()
    self._logger.info( 'Running ' + str(self._powheg_executable) )

    # Setup heartbeat thread
    heartbeat = RepeatingTimer( 600., lambda: self._logger.info('Heartbeat: Powheg generation has been running for {0} in total'.format( RepeatingTimer.human_readable_time_interval(time.time() - time_start)) ) )
    heartbeat.daemon = True # Allows program to exit if only this thread is alive
    heartbeat.start()

    # Run generation until finished then kill heartbeat thread
    generate = subprocess.Popen( [self._powheg_executable,''], stdout=subprocess.PIPE, stderr=subprocess.STDOUT )
    for output_line in iter( generate.stdout.readline, b'' ) :
      self._logger.info( output_line.rstrip() )
    generate.stdout.close()
    heartbeat.cancel()

    elapsed_time = time.time() - time_start
    self._logger.info( 'Running Powheg took {0} for {1} events => {2:6.3f} Hz'.format( RepeatingTimer.human_readable_time_interval(elapsed_time), self.nEvents, self.nEvents / elapsed_time ) )

    # Move output to correctly named file
    try :
      os.rename( 'pwgevents.lhe', self.__output_events_file_name )
    except :
      print os.listdir('.')
    self._logger.info( 'Moved pwgevents.lhe to {0}'.format(self.__output_events_file_name) )

    # Finish
    self._logger.info( 'Finished at {0}'.format( time.asctime() ) )
    return


  ###############################################################################
  #
  #  Run filtered event generation
  #
  ###############################################################################
  def generateFilteredEvents(self) :
    self._logger.info( 'Starting Powheg LHEF event generation at {0}'.format( time.asctime() ) )
    filter_executable = os.environ['LHEFPATH']
    filter_executable += '/'+self.filter_name
    filter_args.insert( 0, self.__output_events_file_name )
    filter_args.insert( 0, 'pwgevents.lhe' )
    filter_args.insert( 0, str(filter_executable) )

    # Remove any existing .lhe files to avoid repeated events
    if self.PDF_info_type == 2 :
      self._logger.warning( 'Not attempting to remove existing LHE files due to reweighting settings! This could cause problems.' )
    else :
      try :
        os.remove( '*.lhe' )
      except OSError :
        pass

    # Generate the events
    self._logger.info( 'Running ' + str(self._powheg_executable) )

    # Start fifo
    setuppipe = subprocess.Popen( ['mkfifo','pwgevents.lhe'] )
    setuppipe.wait()

    # Start generation process
    generate = subprocess.Popen( [self._powheg_executable,''] )

    # Start filter and wait until quota has been filled
    # LHEF_filter moves output to correctly named file
    self._logger.info( 'Running filter with commmand: {0}'.format( filter_executable ) )
    LHEF_filter = subprocess.Popen( self.filter_args )
    LHEF_filter.wait()
    outputline = LHEF_filter.communicate()
    self._logger.info( outputline )

    # Once quota filled, kill generation process
    generate.terminate()

    # Finish
    self._logger.info( 'Finished at {0}'.format( time.asctime() ) )
    return
