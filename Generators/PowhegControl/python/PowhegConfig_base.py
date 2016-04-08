# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_base
#  PowhegControl base class for all Powheg processes
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
import glob, os, subprocess, time
import strategies
from AthenaCommon import Logging
from DecoratorFactory import decorate
from utility import IntegrationGridTester, LHEHandler, RepeatingTimer

## Base class for configurable objects in the jobOptions
#
#  All subprocesses inherit from this class
class PowhegConfig_base(object) :
  ## Current directory
  __base_directory = os.environ['PWD']
  ## Path to Powheg installation
  __powheg_directory = os.environ['POWHEGPATH']

  ## Setup athena-compatible logger
  __logger = Logging.logging.getLogger('PowhegControl')

  ## Number of subjobs to run in manyseeds mode -- reset by ATHENA_PROC_NUMBER if present
  __n_cores = 1

  ## This must be defined by each derived class - don't change it in the jobOptions!
  _powheg_executable = __powheg_directory

  ## Switch to determine which version of Powheg this process belongs to
  _powheg_version_type = 1


  ## Constructor
  #  @param runArgs Generate_tf run arguments
  #  @param opts athena run options
  #  @param indicator that this is a leading-order process
  def __init__( self, runArgs=None, opts=None, LO_process=False ) :
    # Allow processes to indicate that they are leading order
    self.__is_leading_order = LO_process

    ## Dictionary of named groups of event weights
    self.reweight_groups = {}
    self.__enable_reweighting = False

    # This needs to be set so that Generate_trf finds an appropriate file format for showering
    self.__output_events_file_name = 'PowhegOTF._1.events'
    self.__output_tarball_name = None

    # Set up lists of parameters and decorators
    self.__fixed_parameters = []
    self.__configurable_parameters = {}
    self.__phantom_parameters = {}
    self.__run_card_decorators = []

    # Add universal functionality
    self.add_parameter_set( 'base' )

    # Initialise values from runArgs
    if runArgs == None :
      self.logger.warning( 'No run arguments found! Using defaults.' )
    else :
      # Read values from runArgs
      if hasattr(runArgs,'ecmEnergy') :
        self.beam_energy = 0.5 * runArgs.ecmEnergy
      if hasattr(runArgs,'maxEvents') and runArgs.maxEvents > 0 :
        self.nEvents = int( 1.1 * runArgs.maxEvents + 0.5 )
      if hasattr(runArgs,'randomSeed') :
        self.random_seed = runArgs.randomSeed
      if hasattr(runArgs,'outputTXTFile') :
        for tarball_suffix in [ x for x in ['.tar.gz', '.tgz'] if x in runArgs.outputTXTFile ] :
          self.__output_tarball_name = runArgs.outputTXTFile
          self.__output_events_file_name = self.output_tarball_name.split(tarball_suffix)[0]+'.events'
      # Set inputGeneratorFile to match output events file; otherwise Generate_tf check will fail
      runArgs.inputGeneratorFile = self.output_events_file_name

    # Enable parallel mode if AthenaMP mode is enabled
    self.__n_cores = int( os.environ.pop('ATHENA_PROC_NUMBER',1) )
    if self.cores > 1 :
      self.logger.info( 'This job is running with an athenaMP-like whole-node setup.' )
      self.manyseeds = 1
      # Try to modify the transform opts to suppress athenaMP mode
      if hasattr(opts,'nprocs') :
        self.logger.info( 'Re-configuring to keep athena running serially while parallelising Powheg generation.' )
        opts.nprocs = 0
      else :
        self.logger.warning( 'No "nprocs" option provided!')


  ## Run normal event generation
  #  @param external_run_card Use a user-provided run card
  #  @param run_card_only Only generate the run card
  def generate( self, external_run_card=False, run_card_only=False ) :
    if not external_run_card :
      self.__generate_run_card()
    if not run_card_only :
      self.__generate_events()
    return


  ## Initialise runcard with generic options
  def __generate_run_card(self) :
    # Check that event generation is correctly set up
    if (self.bornsuppfact > 0.0) and (self.bornktmin <= 0.0) :
      self.logger.warning( 'These settings: bornsuppfact = {0} and bornktmin = {1} cannot be used to generate events!'.format(self.bornsuppfact,self.bornktmin) )
      self.logger.warning( 'Only fixed-order distributions can be produced with these settings!' )

    # Scale-down number of events produced in each run if running in multicore mode
    if self.cores > 1 :
      self.nEvents_unscaled = self.nEvents
      self.nEvents = int( 0.5 + self.nEvents / self.cores )
      self.ncall1 = int( 0.5 + self.ncall1 / self.cores )
      self.ncall2 = int( 0.5 + self.ncall2 / self.cores )
      self.nubound = int( 0.5 + self.nubound / self.cores )
      self.logger.info( 'Scaling number of events per job from {0} down to {1}'.format(self.nEvents_unscaled, self.nEvents) )

    # Finalise registered decorators
    for run_card_decorator in self.run_card_decorators :
      if hasattr( run_card_decorator, 'finalise' ) : run_card_decorator.finalise()

    # Print list of configurable parameters for users
    self.logger.info( '** User configurable parameters for this process **' )
    self.logger.info( ':   Option name   : ATLAS default :  Current  : Description' )
    for configurable_list in sorted( self.configurable_parameters.values()+self.phantom_parameters.values(), key=lambda x: x[0].lower() ) :
      self.logger.info( ': {0:<15} : {1:>13} : {2:>9} : {3}'.format( configurable_list[0], configurable_list[1], getattr(self, configurable_list[0]), configurable_list[2] ) )

    # Add configurable parameters to fixed list
    [ self.fix_parameter( non_configurable_name=configurable_list[0], default=configurable_list[1], desc=configurable_list[2] ) for configurable_list in self.configurable_parameters.values() ]

    # Write out final runcard
    self.logger.info( 'Writing Powheg runcard to {0}'.format( self.run_card_path ) )
    with open( self.run_card_path, 'w' ) as f :
      for non_configurable_list in sorted( self.fixed_parameters, key=lambda x: x[0].lower() ) :
        name, value, default, desc = non_configurable_list
        if 'lhrwgt' in name : value = '\'{0}\''.format( value ) # lhrwgt parameters need to be in quotes
        # Set starting value to first in list when multiple values are provided
        if isinstance(value,list) :
          value = value[0]
          self.__enable_reweighting = True
        output_line = '{0:<30}! [ATLAS default: {1}] {2}'.format( '{0} {1}'.format( name, value ), default, desc )
        f.write( '{0}\n'.format(output_line) )
        # Print warnings for specific parameters
        if name == 'bornsuppfact' and value > 0 :
          self.logger.warning( 'Born-level suppression is enabled: using this in conjunction with J-slicing may give problems.' )

    # Print final preparation message
    self.logger.info( 'Using executable: {0}'.format( self._powheg_executable ) )
    return


  ## Run normal event generation
  def __generate_events(self) :
    # Initialise timer
    time_start = time.time()
    self.logger.info( 'Starting Powheg LHEF event generation at {0}'.format( time.ctime( time_start ) ) )

    # Setup heartbeat thread
    heartbeat = RepeatingTimer( 600., lambda: self.__emit_heartbeat( time.time() - time_start ) )
    heartbeat.setName( 'heartbeat thread' )
    heartbeat.daemon = True # Allow program to exit if this is the only live thread
    heartbeat.start()

    # Remove any existing .lhe files to avoid repeated events
    for LHE_file in glob.glob('*.lhe')+glob.glob('*.ev*ts') :
      self.logger.warning( 'Removing existing LHE file: {0}'.format(LHE_file) )
      os.remove( LHE_file )

    # Run appropriate Powheg process and display generation output until finished
    self.__run_generation_strategy()

    # Print timing information
    generation_end = time.time()
    elapsed_time = generation_end - time_start
    self.logger.info( 'Running nominal Powheg took {0} for {1} events => {2:6.3f} Hz'.format( RepeatingTimer.human_readable_time_interval(elapsed_time), self.nEvents, self.nEvents / elapsed_time ) )

    # Concatenate output events if running in multicore mode
    if self.cores > 1 :
      self.logger.info( 'Concatenating {0} output LHE files'.format( self.cores ) )
      LHEHandler(self.logger).merge( 'pwgevents.lhe', sorted( glob.glob('pwgevents*.lhe') ) )
      subprocess.call( 'rm pwgevents-*.lhe 2> /dev/null', shell=True )
      # Unscale nEvents in case this is needed by afterburners
      subprocess.call( 'sed -i "s/numevts.*/numevts {0}/g" powheg*.input'.format(self.nEvents_unscaled), shell=True )

    # Run Powheg afterburners
    self.__run_afterburners()
    elapsed_time = time.time() - generation_end
    self.logger.info( 'Running Powheg afterburners took {0}'.format( RepeatingTimer.human_readable_time_interval(elapsed_time) ) )

    # Move output to correctly named file
    try :
      os.rename( 'pwgevents.lhe', self.output_events_file_name )
      self.logger.info( 'Moved pwgevents.lhe to {0}'.format(self.output_events_file_name) )
    except OSError :
      self.logger.warning( 'No output LHEF file found! Probably because the Powheg process was killed before finishing.' )

    # Tar events if LHE output is requested
    if self.output_tarball_name is not None :
      self.logger.info( 'Tar-ing output events into {0}'.format(self.output_tarball_name) )
      [ self.logger.info(line) for line in subprocess.check_output( [ 'tar', 'cvzf', self.output_tarball_name, self.output_events_file_name ], stderr=subprocess.STDOUT ).splitlines() ]

    # Print finalisation message
    IntegrationGridTester.output_results( self.logger )
    self.logger.info( 'Finished at {0}'.format( time.asctime() ) )

    # Kill heartbeat thread
    heartbeat.cancel()
    return


  ## Run external Powheg process
  def __run_generation_strategy(self) :
    # Initialise reweighting
    if self.__enable_reweighting :
      strategies.initialise_reweighting( self )

    # Run single core
    if self.cores == 1 :
      strategies.generate_single_core( self )

    # Run multicore
    else :
      # Run v1-style multicore (only needs one step)
      if self._powheg_version_type == 1 :
        strategies.generate_v1_multi_core( self )
      # Run v2-style multicore (needs four steps)
      else :
        strategies.generate_v2_multi_core( self )
    return


  ## Run external Powheg process
  def __run_afterburners(self) :
    # Run scale/PDF/arbitrary reweighting if requested
    if self.__enable_reweighting :
      strategies.afterburner_reweighting( self )
    # Run NNLOPS if requested
    if hasattr( self, 'NNLO_reweighting_inputs' ) and len(self.NNLO_reweighting_inputs) > 0 :
      strategies.afterburner_NNLO_reweighting( self )
    return


  ## Register configurable parameter: exposed to the user and written to the run card
  #  @param configurable_name Configurable parameter name exposed to the user
  #  @param value Value of the parameter
  #  @param desc Description for the run card
  #  @param parameter Name used in the run card if different
  def add_parameter( self, configurable_name, value, default='', desc='', parameter=None ) :
    setattr( self, configurable_name, value ) # add new attribute
    powheg_parameter = parameter if parameter is not None else configurable_name
    self.configurable_parameters[powheg_parameter] = [configurable_name, default, desc]
    return


  ## Register configurable parameter: exposed to the user but not written to the run card
  #  @param configurable_name Configurable parameter name exposed to the user
  #  @param value Value of the parameter
  #  @param desc Description for the run card
  def add_phantom( self, configurable_name, value, default='', desc='' ) :
    setattr( self, configurable_name, value ) # add new attribute
    self.phantom_parameters[configurable_name] = [configurable_name, default, desc]
    return


  ## Register a non-configurable parameter: not exposed to the user but written to the run card
  #  @param non_configurable_name Parameter name
  #  @param value Value of the parameter
  #  @param desc Description for the run card
  def fix_parameter( self, non_configurable_name, value=None, default='', desc='' ) :
    # Get previously set value if not overwriting
    if value is None : value = getattr( self, non_configurable_name )
    # Remove it from the configurable list if it was there
    for powheg_parameter, configurable_list in self.configurable_parameters.items() :
      # Retrieve Powheg parameter name and description if there is a match
      if non_configurable_name == configurable_list[0] :
        non_configurable_name, desc = powheg_parameter, configurable_list[1]
        self.configurable_parameters.pop(powheg_parameter)
        break
    self.fixed_parameters.append( [non_configurable_name, value, default, desc] )
    return


  ## Alias to PowhegDecorators.decorate
  #  @param parameter_set Name of a set of parameter to add to this configurable
  def add_parameter_set( self, parameter_set, **kwargs ) :
    return decorate( self, parameter_set, **kwargs )


  ## Add a new named group of event weights
  #  @param group_name Name of the group of weights
  #  @param parameter_set Names of the individual weights
  def define_weight_group( self, group_name, weight_names ) :
    self.logger.info( 'Defining new weight group {0} with {1} variations'.format( group_name, len(weight_names) ) )
    self.reweight_groups[group_name] = {}
    parameter_names, parameter_values = [], []
    for parameter, ( name, desc ) in self.configurable_parameters.items() :
      if isinstance( getattr(self,name), list ) :
        if not name in [ 'PDF', 'mu_R', 'mu_F' ] : # these are treated separately
          assert( len(weight_names) == len(getattr(self,name)) ), 'Number of parameter variations must be the same as the number of names provided'
          parameter_names.append( parameter )
          parameter_values.append( getattr(self,name) )
    for idx, weight_name in enumerate( weight_names ) :
      self.reweight_groups[group_name][weight_name] = [ [n,v[idx]] for n,v in zip(parameter_names,parameter_values) ]
    return


  ## Output a heartbeat message
  #  @param duration Time interval in seconds between output messages
  def __emit_heartbeat(self, duration) :
    message = 'Heartbeat: Powheg generation has been running for {0} in total'.format( RepeatingTimer.human_readable_time_interval(duration) )
    self.logger.info( message )
    with open( '{0}/eventLoopHeartBeat.txt'.format( self.__base_directory ), 'w' ) as f : f.write( message )
    return


  ## Write default values to string before allowing users to change them
  def populate_default_strings( self ) :
    # Configurable parameters
    for configurable_list in self.configurable_parameters.values() : # [configurable_name, default, desc]
      value = getattr( self, configurable_list[0] )
      if '{0}' in configurable_list[1] : configurable_list[1] = configurable_list[1].format( value )
    # Phantom parameters
    for configurable_list in self.phantom_parameters.values() : # [configurable_name, default, desc]
      value = getattr( self, configurable_list[0] )
      if '{0}' in configurable_list[1] : configurable_list[1] = configurable_list[1].format( value )
    # Fixed parameters
    for non_configurable_list in self.fixed_parameters : #[non_configurable_name, value, default, desc]
      if '{0}' in non_configurable_list[2] : non_configurable_list[2] = non_configurable_list[2].format( non_configurable_list[1] )
    return


  ## Get base directory
  @property
  def base_directory(self) :
    return self.__base_directory


  ## Get dictionary of configurable parameters: visible to user and written to runcard
  @property
  def configurable_parameters(self) :
    return self.__configurable_parameters


  ## Get number of cores
  @property
  def cores(self) :
    return self.__n_cores


  ## Get dictionary of non-configurable parameters: not visible to user but written to runcard
  @property
  def fixed_parameters(self) :
    return self.__fixed_parameters


  ## Get whether this is a leading-order process
  @property
  def is_LO(self) :
    return self.__is_leading_order


  ## Get handle to logger
  @property
  def logger(self) :
    return self.__logger


  ## Get output file name
  @property
  def output_events_file_name(self) :
    return self.__output_events_file_name


  ## Get output tarball name
  @property
  def output_tarball_name(self) :
    return self.__output_tarball_name


  ## Get dictionary of phantom parameters: visible to user but not written to runcard
  @property
  def phantom_parameters(self) :
    return self.__phantom_parameters


  ## Get Powheg directory
  @property
  def powheg_directory(self) :
    return self.__powheg_directory


  ## Get list of enabled run card decorators
  @property
  def run_card_decorators(self) :
    return self.__run_card_decorators


  ## Get full path to runcard
  @property
  def run_card_path(self) :
    return '{0}/powheg.input'.format( self.base_directory )


  ## Deprecated function - use generate() instead
  def generateRunCard(self) :
    self.logger.warning( 'The function generateRunCard() is deprecated as of 2015-02-19' )
    self.logger.warning( 'Please use generate() instead of generateRunCard() and generateEvents()' )
    return self.__generate_run_card()


  ## Deprecated function - use generate() instead
  def generateEvents(self) :
    self.logger.warning( 'The function generateEvents() is deprecated as of 2015-02-19' )
    self.logger.warning( 'Please use generate() instead of generateRunCard() and generateEvents()' )
    return self.__generate_events()
