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
from utility import RepeatingTimer

## Base class for configurable objects in the jobOptions
#
#  All subprocesses inherit from this class
class PowhegConfig_base(object) :
  ## Set up base directory
  __base_directory = os.environ['PWD']

  ## Setup athena-compatible logger
  __logger = Logging.logging.getLogger('PowhegControl')

  ## Number of subjobs to run in manyseeds mode -- reset by ATHENA_PROC_NUMBER if present
  __n_cores = 1

  ## This must be defined by each derived class - don't change it in the jobOptions!
  _powheg_executable = os.environ['POWHEGPATH']
  print 'executable:',_powheg_executable

  ## Switch to determine which version of Powheg this process belongs to
  _powheg_version_type = 1


  def __init__( self, runArgs=None, opts=None, LO_process=False ) :
    ## Allow processes to indicate that they are leading order
    self.__is_leading_order = LO_process

    ## This needs to be set so that Generate_trf finds an appropriate file format for showering
    self.__output_events_file_name = 'PowhegOTF._1.events'

    ## Set up lists of parameters and decorators
    self.__fixed_parameters = []
    self.__configurable_parameters = {}
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
      if hasattr(runArgs,'random_seed') :
        self.random_seed = runArgs.random_seed
      ## Set inputGeneratorFile to match output events file. Otherwise Generate_trf check will fail.
      runArgs.inputGeneratorFile = self.output_events_file_name

    ## Enable parallel mode if AthenaMP mode is enabled
    if 'ATHENA_PROC_NUMBER' in os.environ :
      self.manyseeds = 1
      self.logger.info( 'This job is running with an athena MP-like whole-node setup. Will re-configure now to make sure that the remainder of the job runs serially.' )
      # Try to modify the transform opts to suppress athenaMP mode
      self.__n_cores = int( os.environ.pop('ATHENA_PROC_NUMBER') )
      if hasattr(opts,'nprocs') : opts.nprocs = 0
      else : self.logger.warning( 'No "nprocs" options provided!')
      self.logger.info( 'Scaling number of events per job from {0} down to {1}'.format( self.nEvents, int(self.nEvents / self.cores + 0.5) ) )


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
      self.nEvents = int( self.nEvents / self.cores + 0.5 )
      self.ncall1 = int(  self.ncall1 / self.cores + 0.5 )
      self.ncall2 = int( self.ncall2 / self.cores + 0.5 )
      self.nubound = int( self.nubound / self.cores + 0.5  )

    ## Add registered decorators
    for run_card_decorator in self.run_card_decorators :
      if hasattr( run_card_decorator, 'finalise' ) : run_card_decorator.finalise()

    ## Print list of configurable parameters for users
    self.logger.info( '** User configurable parameters for this process **' )
    for parameter, value_tuple in sorted( self.configurable_parameters.items(), key=lambda x: x[1][0].lower() ) :
      self.logger.info( ' : {0:<20} : {1}'.format( *value_tuple ) )

    ## Add configurable parameters to fixed list
    parameters_to_add = { k : self.configurable_parameters.pop(k) for k in self.configurable_parameters.keys() }
    [ self.fix_parameter( parameter, getattr(self, value_tuple[0]), value_tuple[1] ) for parameter, value_tuple in parameters_to_add.items() ]

    ## Write out final runcard
    self.logger.info( 'Writing Powheg runcard to {0}'.format( self.run_card_path ) )
    with open( self.run_card_path, 'w' ) as f :
      for parameter_tuple in sorted( self.fixed_parameters, key=lambda x: x[0].lower() ) :
        name, value, desc = parameter_tuple
        if 'lhrwgt' in name : value = '\'{0}\''.format( value ) # lhrwgt parameters need to be in quotes
        if isinstance(value,list) : value = value[0]            # Set starting value to first in list when multiple values are provided
        output_line = '{0:<30}! {1}'.format( '{0} {1}'.format( name, value ), desc )
        f.write( output_line+'\n' )


  ## Run normal event generation
  def generateEvents(self) :
    self.logger.info( 'Starting Powheg LHEF event generation at {0}'.format( time.asctime() ) )

    # Remove any existing .lhe files to avoid repeated events
    try :
      os.remove( '*.lhe *.ev*ts' )
    except OSError :
      pass

    ## Generate the events in the external Powheg process
    time_start = time.time()
    self.logger.info( 'Running ' + str(self._powheg_executable) )

    ## Setup heartbeat thread
    heartbeat = RepeatingTimer( 600., lambda: self.emit_heartbeat( time.time() - time_start ) )
    heartbeat.daemon = True # Allow program to exit if this is the only live thread
    heartbeat.start()

    ## Initialise generation processes
    self.running_processes = []

    ## Run appropriate Powheg process
    self.runPowhegStrategy()

    ## Display generation output until finished then kill heartbeat thread
    heartbeat.cancel()

    ## Print timing information
    elapsed_time = time.time() - time_start
    self.logger.info( 'Running Powheg took {0} for {1} events => {2:6.3f} Hz'.format( RepeatingTimer.human_readable_time_interval(elapsed_time), self.nEvents, self.nEvents / elapsed_time ) )

    ## Concatenate output events if running in multicore mode
    if self.manyseeds >= 1 :
      self.logger.info( 'Concatenating {0} output LHE files'.format( self.cores ) )
      subprocess.call( 'cat pwgevents*.lhe > pwgevents.lhe', shell=True ) # NB. shell=True is unsafe if combined with user input

    ## Move output to correctly named file
    try :
      os.rename( 'pwgevents.lhe', self.output_events_file_name )
      self.logger.info( 'Moved pwgevents.lhe to {0}'.format(self.output_events_file_name) )
    except OSError :
      print os.listdir('.')
      self.logger.warning( 'No pwgevents.lhe found!' )

    ## Print finalisation message
    self.logger.info( 'Finished at {0}'.format( time.asctime() ) )
    return


  ## Run filtered event generation
  def generateFilteredEvents(self, filter_name, filter_args) :
    self.logger.info( 'Starting Powheg LHEF event generation at {0}'.format( time.asctime() ) )
    filter_executable = os.environ['LHEFPATH'] + '/' + filter_name
    filter_args.insert( 0, self.output_events_file_name )
    filter_args.insert( 0, 'pwgevents.lhe' )
    filter_args.insert( 0, str(filter_executable) )

    ## Remove any existing .lhe files to avoid repeated events
    try :
      os.remove( '*.lhe *.ev*ts' )
    except OSError :
      pass

    ## Generate the events
    self.logger.info( 'Running ' + str(self._powheg_executable) )

    ## Start FIFO
    setuppipe = subprocess.Popen( ['mkfifo','pwgevents.lhe'] )
    setuppipe.wait()

    ## Start generation process
    generate = subprocess.Popen( [self._powheg_executable,''] )

    ## Start filter and wait until quota has been filled. LHEF_filter moves output to correctly named file
    self.logger.info( 'Running filter with commmand: {0}'.format( filter_executable ) )
    LHEF_filter = subprocess.Popen( filter_args )
    LHEF_filter.wait()
    LHEF_filter_output = LHEF_filter.communicate()
    self.logger.info( LHEF_filter_output )

    ## Once quota filled, kill generation process
    generate.terminate()

    ## Finish
    self.logger.info( 'Finished at {0}'.format( time.asctime() ) )
    return


  ## Run external Powheg process
  def runPowhegStrategy(self) :
    # Check whether multiple PDFs have been specified
    try : __multiple_PDFs_or_scales = isinstance(self.PDF,list) or (isinstance(self.mu_F,list) and isinstance(self.mu_R,list))
    except TypeError : __multiple_PDFs_or_scales = False

    if self.manyseeds < 1  :
      ## Normal run
      if not __multiple_PDFs_or_scales :
        strategies.runPowhegSingleThread(self)
      ## Multiple PDFs/scales
      else :
        strategies.runPowhegV2Multiweight(self)
    else :
      if __multiple_PDFs_or_scales : self.logger.warning( 'Cannot combine multicore running with multiple weight rerunning!' )
      ## Run v1-style multiprocess (only needs one step)
      if self._powheg_version_type == 1 :
        strategies.runPowhegV1Multicore(self)
      ## Run v2-style multiprocess (needs four steps)
      else :
        strategies.runPowhegV2Multicore(self)


  ## Register configurable parameter
  def add_parameter( self, name, value, desc='', parameter=None ) :
    setattr( self, name, value ) # add new attribute
    powheg_parameter = parameter if parameter is not None else name
    self.configurable_parameters[powheg_parameter] = ( name, desc )


  ## Alias to PowhegDecorators.decorate
  def add_parameter_set( self, parameter_set, **kwargs ) :
    return decorate( self, parameter_set, **kwargs )


  ## Output a heartbeat message
  def emit_heartbeat(self, duration) :
    message = 'Heartbeat: Powheg generation has been running for {0} in total'.format( RepeatingTimer.human_readable_time_interval(duration) )
    self.logger.info( message )
    with open( '{0}/eventLoopHeartBeat.txt'.format( self.__base_directory ), 'w' ) as f : f.write( message )


  ## Register non-configurable parameter
  def fix_parameter( self, name, value=None, desc='' ) :
    # Get previously set value if not overwriting
    if value is None : value = getattr( self, name )
    # Remove it from the configurable list if it was there
    for powheg_parameter, name_tuple in self.configurable_parameters.items() :
      if name == name_tuple[0] :
        # Retrieve Powheg parameter name and description
        name, desc = powheg_parameter, name_tuple[1]
        self.configurable_parameters.pop(name)
    self.fixed_parameters.append( (name, value, desc) )


  ## Remove parameter from list
  def pop( self, name ) :
    if name in self.configurable_parameters :
      self.configurable_parameters.pop( name )[0]
      return getattr( self, name )
    for idx, parameter_set in enumerate( self.fixed_parameters ) :
      if parameter_set[0] == name :
        fixed_parameter_set = self.fixed_parameters.pop( idx )
        return fixed_parameter_set[1]
        break


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


  ## Get list of enabled run card decorators
  @property
  def run_card_decorators(self) :
    return self.__run_card_decorators


  ## Get full path to runcard
  @property
  def run_card_path(self) :
    return '{0}/powheg.input'.format( self.__base_directory )
