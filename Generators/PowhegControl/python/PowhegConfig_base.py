# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_base
#  PowhegControl base class for all Powheg processes
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
import os, subprocess, time
import strategies
from AthenaCommon import Logging
from DecoratorFactory import decorate
from utility import IntegrationGridTester, RepeatingTimer

## Base class for configurable objects in the jobOptions
#
#  All subprocesses inherit from this class
class PowhegConfig_base(object) :
  ## Set up base directory and path to Powheg
  __base_directory = os.environ['PWD']
  __powheg_directory = os.environ['POWHEGPATH']

  ## Setup athena-compatible logger
  __logger = Logging.logging.getLogger('PowhegControl')

  ## Number of subjobs to run in manyseeds mode -- reset by ATHENA_PROC_NUMBER if present
  __n_cores = 1

  ## This must be defined by each derived class - don't change it in the jobOptions!
  _powheg_executable = __powheg_directory

  ## Switch to determine which version of Powheg this process belongs to
  _powheg_version_type = 1


  def __init__( self, runArgs=None, opts=None, LO_process=False ) :
    ## Allow processes to indicate that they are leading order
    self.__is_leading_order = LO_process

    # ## Allow automatic configuration of multiple weights if specified
    self.reweight_groups = {}
    self.__enable_reweighting = False

    ## This needs to be set so that Generate_trf finds an appropriate file format for showering
    self.__output_events_file_name = 'PowhegOTF._1.events'

    ## Set up lists of parameters and decorators
    self.__fixed_parameters = []
    self.__configurable_parameters = {}
    self.__phantom_parameters = {}
    self.__run_card_decorators = []

    ## Add universal functionality
    self.add_parameter_set( 'base' )

    ## Initialise values from runArgs
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
        self.output_events_file_name = runArgs.outputTXTFile.split('.tar.gz')[0]+'.events'
      ## Set inputGeneratorFile to match output events file. Otherwise Generate_trf check will fail.
      runArgs.inputGeneratorFile = self.output_events_file_name

    ## Enable parallel mode if AthenaMP mode is enabled
    if 'ATHENA_PROC_NUMBER' in os.environ :
      self.__n_cores = int( os.environ.pop('ATHENA_PROC_NUMBER') )
      if self.cores > 1 :
        self.manyseeds = 1
        self.logger.info( 'This job is running with an athena MP-like whole-node setup. Will re-configure now to make sure that the remainder of the job runs serially.' )
        # Try to modify the transform opts to suppress athenaMP mode
        if hasattr(opts,'nprocs') : opts.nprocs = 0
        else : self.logger.warning( 'No "nprocs" options provided!')
        self.logger.info( 'Scaling number of events per job from {0} down to {1}'.format( self.nEvents, int(self.nEvents / self.cores + 0.5) ) )
      else :
        self.__ncores = 1


  ## Run normal event generation
  def generate( self, filter_name='', filter_args='' ) :
    self.generateRunCard()
    if filter_name == '' :
      self.generateEvents()
    else :
      self.generateFilteredEvents( filter_name, filter_args )


  ## Initialise runcard with generic options
  def generateRunCard(self) :
    ## Check that event generation is correctly set up
    if (self.bornsuppfact > 0.0) and (self.bornktmin <= 0.0) :
      self.logger.warning( 'These settings: bornsuppfact({0}) and bornktmin({1}) cannot be used to generate events! Only fixed-order distributions can be produced with these settings!'.format(self.bornsuppfact,self.bornktmin) )

    ## Scale-down number of events produced in each run if running in multicore mode
    if self.cores > 1 :
      self.nEvents = 0.5 + self.nEvents / self.cores
      self.ncall1 = 0.5 + self.ncall1 / self.cores
      self.ncall2 = 0.5 + self.ncall2 / self.cores
      self.nubound = 0.5 + self.nubound / self.cores

    ## Finalise registered decorators
    for run_card_decorator in self.run_card_decorators :
      if hasattr( run_card_decorator, 'finalise' ) : run_card_decorator.finalise()

    ## Print list of configurable parameters for users
    self.logger.info( '** User configurable parameters for this process **' )
    self.logger.info( ': Configurable parameter :   Current value   : Description' )
    for value_tuple in sorted( self.configurable_parameters.values()+self.phantom_parameters.values(), key=lambda x: x[0].lower() ) :
      self.logger.info( ': {0:<22} : {1:>17} : {2}'.format( value_tuple[0], getattr(self, value_tuple[0]), value_tuple[1] ) )

    ## Add configurable parameters to fixed list
    [ self.fix_parameter( parameter=value_tuple[0], desc=value_tuple[1] ) for value_tuple in self.configurable_parameters.values() ]

    ## Write out final runcard
    self.logger.info( 'Writing Powheg runcard to {0}'.format( self.run_card_path ) )
    with open( self.run_card_path, 'w' ) as f :
      for parameter_tuple in sorted( self.fixed_parameters, key=lambda x: x[0].lower() ) :
        name, value, desc = parameter_tuple
        if 'lhrwgt' in name : value = '\'{0}\''.format( value ) # lhrwgt parameters need to be in quotes
        # Set starting value to first in list when multiple values are provided
        if isinstance(value,list) :
          value = value[0]
          self.__enable_reweighting = True
        output_line = '{0:<30}! {1}'.format( '{0} {1}'.format( name, value ), desc )
        f.write( '{0}\n'.format(output_line) )

    ## Print final preparation message
    self.logger.info( 'Using executable: {0}'.format( self._powheg_executable ) )


  ## Run normal event generation
  def generateEvents(self) :
    ## Initialise timer
    time_start = time.time()
    self.logger.info( 'Starting Powheg LHEF event generation at {0}'.format( time.ctime( time_start ) ) )

    ## Setup heartbeat thread
    heartbeat = RepeatingTimer( 600., lambda: self.emit_heartbeat( time.time() - time_start ) )
    heartbeat.daemon = True # Allow program to exit if this is the only live thread
    heartbeat.start()

    # Remove any existing .lhe files to avoid repeated events
    self.logger.info( 'Removing old LHE files' )
    try :
      os.remove( '*.lhe *.ev*ts' )
    except OSError :
      pass

    ## Initialise generation process tracker
    self.running_processes = []

    ## Run appropriate Powheg process
    self.runPowhegStrategy()

    ## Display generation output until finished then kill heartbeat thread
    heartbeat.cancel()

    ## Print timing information
    generation_end = time.time()
    elapsed_time = generation_end - time_start
    self.logger.info( 'Running nominal Powheg took {0} for {1} events => {2:6.3f} Hz'.format( RepeatingTimer.human_readable_time_interval(elapsed_time), self.nEvents, self.nEvents / elapsed_time ) )

    ## Concatenate output events if running in multicore mode
    if self.manyseeds >= 1 :
      self.logger.info( 'Concatenating {0} output LHE files'.format( self.cores ) )
      subprocess.call( 'cat pwgevents*.lhe > pwgevents.lhe', shell=True )

    ## Run Powheg afterburners
    self.runPowhegAfterburners()
    elapsed_time = time.time() - generation_end
    self.logger.info( 'Running Powheg afterburners took {0}'.format( RepeatingTimer.human_readable_time_interval(elapsed_time) ) )

    ## Move output to correctly named file
    try :
      os.rename( 'pwgevents.lhe', self.output_events_file_name )
      self.logger.info( 'Moved pwgevents.lhe to {0}'.format(self.output_events_file_name) )
    except OSError :
      self.logger.warning( 'No output LHEF file found! Probably because the Powheg process was killed before finishing.' )

    ## Print finalisation message
    IntegrationGridTester.output_results( self.logger )
    self.logger.info( 'Finished at {0}'.format( time.asctime() ) )
    return


  ## Run external Powheg process
  def runPowhegStrategy(self) :
    ## Initialise reweighting
    if self.__enable_reweighting :
      strategies.initialisePowhegReweighting(self)

    ## Run single core
    if self.cores == 1 :
      strategies.runPowhegSingleThread(self)

    ## Run multicore
    else :
      ## Run v1-style multiprocess (only needs one step)
      if self._powheg_version_type == 1 :
        strategies.runPowhegV1Multicore(self)
      ## Run v2-style multiprocess (needs four steps)
      else :
        strategies.runPowhegV2Multicore(self)


  ## Run external Powheg process
  def runPowhegAfterburners(self) :
    ## Run scale/PDF/arbitrary reweighting if requested
    if self.__enable_reweighting :
      strategies.runPowhegReweightingAfterburner( self )

    ## Run NNLOPS if requested
    if hasattr( self, 'NNLOPS_input' ) and self.NNLOPS_input is not None :
      strategies.runPowhegNNLOPSAfterburner( self )



  ## Register configurable parameter
  def add_parameter( self, configurable_name, value, desc='', parameter=None ) :
    setattr( self, configurable_name, value ) # add new attribute
    powheg_parameter = parameter if parameter is not None else configurable_name
    self.configurable_parameters[powheg_parameter] = ( configurable_name, desc )


  ## Register configurable parameter
  def add_phantom( self, name, value, desc='' ) :
    setattr( self, name, value ) # add new attribute
    self.phantom_parameters[name] = ( name, desc )


  ## Alias to PowhegDecorators.decorate
  def add_parameter_set( self, parameter_set, **kwargs ) :
    return decorate( self, parameter_set, **kwargs )


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


  ## Output a heartbeat message
  def emit_heartbeat(self, duration) :
    message = 'Heartbeat: Powheg generation has been running for {0} in total'.format( RepeatingTimer.human_readable_time_interval(duration) )
    self.logger.info( message )
    with open( '{0}/eventLoopHeartBeat.txt'.format( self.__base_directory ), 'w' ) as f : f.write( message )


  ## Register non-configurable parameter
  def fix_parameter( self, parameter, value=None, desc='' ) :
    # Get previously set value if not overwriting
    if value is None : value = getattr( self, parameter )
    # Remove it from the configurable list if it was there
    for powheg_parameter, configurable_name_tuple in self.configurable_parameters.items() :
      # Retrieve Powheg parameter name and description if there is a match
      if parameter == configurable_name_tuple[0] :
        parameter, desc = powheg_parameter, configurable_name_tuple[1]
        self.configurable_parameters.pop(powheg_parameter)
        break
    self.fixed_parameters.append( (parameter, value, desc) )


  ## Get base directory
  @property
  def base_directory(self) :
    return self.__base_directory


  ## Get dictionary of configurable parameters
  @property
  def configurable_parameters(self) :
    return self.__configurable_parameters

  ## Get number of cores
  @property
  def cores(self) :
    return self.__n_cores


  ## Get list of non-configurable parameters
  @property
  def fixed_parameters(self) :
    return self.__fixed_parameters


  ## Determine whether this is a leading-order process
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


  # ## Run filtered event generation
  # def generateFilteredEvents(self, filter_name, filter_args) :
  #   self.logger.info( 'Starting Powheg LHEF event generation at {0}'.format( time.asctime() ) )
  #   filter_executable = os.environ['LHEFPATH'] + '/' + filter_name
  #   filter_args.insert( 0, self.output_events_file_name )
  #   filter_args.insert( 0, 'pwgevents.lhe' )
  #   filter_args.insert( 0, str(filter_executable) )
  #
  #   ## Remove any existing .lhe files to avoid repeated events
  #   try :
  #     os.remove( '*.lhe *.ev*ts' )
  #   except OSError :
  #     pass
  #
  #   ## Generate the events
  #   self.logger.info( 'Running ' + str(self._powheg_executable) )
  #
  #   ## Start FIFO
  #   setuppipe = subprocess.Popen( ['mkfifo','pwgevents.lhe'] )
  #   setuppipe.wait()
  #
  #   ## Start generation process
  #   generate = subprocess.Popen( [self._powheg_executable,''] )
  #
  #   ## Start filter and wait until quota has been filled. LHEF_filter moves output to correctly named file
  #   self.logger.info( 'Running filter with commmand: {0}'.format( filter_executable ) )
  #   LHEF_filter = subprocess.Popen( filter_args )
  #   LHEF_filter.wait()
  #   LHEF_filter_output = LHEF_filter.communicate()
  #   self.logger.info( LHEF_filter_output )
  #
  #   ## Once quota filled, kill generation process
  #   generate.terminate()
  #
  #   ## Finish
  #   self.logger.info( 'Finished at {0}'.format( time.asctime() ) )
  #   return

  # ## Remove parameter from list
  # def pop( self, name ) :
  #   if name in self.configurable_parameters :
  #     configurable_name = self.configurable_parameters.pop( name )[0]
  #     return getattr( self, configurable_name )
  #   for idx, parameter_set in enumerate( self.fixed_parameters ) :
  #     if parameter_set[0] == name :
  #       fixed_parameter_set = self.fixed_parameters.pop( idx )
  #       return fixed_parameter_set[1]
  #       break
