# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import os, subprocess, time
from DecoratorFactory import decorate
from AthenaCommon import Logging
from PowhegControl.utility import HeartbeatTimer


## Base class for configurable objects in the jobOptions
#
#  All subprocesses inherit from this class
class ProphecyConfig(object) :
## Set up run directory and path to Prophecy
  __run_directory = os.environ['PWD']

  ## Setup athena-compatible logger
  __logger = Logging.logging.getLogger('ProphecyControl')

  ## This must be defined by each derived class - don't change it in the jobOptions!
# For rel 21.6, no longer need to set explicit path because prophecy4f is in $PATH
  _prophecy_executable = 'Prophecy4f'

  def __init__( self, runArgs=None, opts=None ) :

    ## This needs to be set so that Generate_trf finds an appropriate file format for showering
    self.__output_events_file_name = 'ProphecyOTF._1.events'

    ## Set up lists of parameters and decorators
    self.__fixed_parameters = []
    self.__configurable_parameters = {}
    self.__run_card_decorators = []

    ## Add universal functionality
    self.add_parameter_set( 'fromDefault' )

    ## Initialise values from runArgs
    if runArgs is None :
      self.logger.warning( 'No run arguments found! Using defaults.' )
    else :
      # Read values from runArgs
      if hasattr(runArgs,'maxEvents') and runArgs.maxEvents > 0 :
        self.nEvents = int( 1.1 * runArgs.maxEvents + 0.5 )
      if hasattr(runArgs,'randomSeed') :
        self.random_seed = runArgs.randomSeed
      # RDS: commenting out the following two lines to avoid renaming from Generate_tf argument -
      # will clash with running Prophecy4f twice      
      # if hasattr(runArgs,'outputTXTFile') :
      #   self.output_events_file_name = 'prophecy_'+runArgs.outputTXTFile.split('.tar.gz')[0]+'.events'

      if 100 * self.nEvents > self.nEvents_weighted :
        self.logger.warning( 'There are {0} events requested with {1} weighted ones.'.format( self.nEvents, self.nEvents_weighted ) )
        self.nEvents_weighted = 100 * self.nEvents
        self.logger.warning( 'Forcing weighted events to be {0}.'.format( self.nEvents_weighted ) )


  ## Run normal event generation
  def generate( self, filter_name='', filter_args='' ) :
    self.generateRunCard()
    self.generateEvents()

  ## Initialise runcard with generic options
  def generateRunCard(self) :

    ## Finalise registered decorators
    for run_card_decorator in self.run_card_decorators :
      if hasattr( run_card_decorator, 'finalise' ) : run_card_decorator.finalise()

    ## Print list of configurable parameters for users
    self.logger.info( '** User configurable parameters for this process **' )
    self.logger.info( ': Configurable parameter :   Current value   : Description' )
    for value_tuple in sorted( self.configurable_parameters.values(), key=lambda x: x[0].lower() ) :
      self.logger.info( ': {0:<22} : {1:>17} : {2}'.format( value_tuple[0], getattr(self, value_tuple[0]), value_tuple[1] ) )

    ## Add configurable parameters to fixed list
    [ self.fix_parameter( parameter=value_tuple[0], desc=value_tuple[1] ) for value_tuple in self.configurable_parameters.values() ]

    ## Write out final runcard
    self.logger.info( 'Writing Prophecy runcard to {0}'.format( self.run_card_path ) )
    with open( self.run_card_path, 'w' ) as f :
      for parameter_tuple in sorted( self.fixed_parameters, key=lambda x: x[0].lower() ) :
        name, value, desc = parameter_tuple
        # Set starting value to first in list when multiple values are provided
        if isinstance(value,list) :
          value = value[0]
          self.__enable_reweighting = True
        output_line = '{0:<30}! {1}'.format( '{0}={1}'.format( name, value ), desc )
        f.write( '{0}\n'.format(output_line) )
        self.logger.info( 'Wrote {0}'.format( output_line ) )

    ## Print final preparation message
    self.logger.info( 'Using executable: {0}'.format( self._prophecy_executable ) )
    self.logger.info( 'Number of events and weight events: {0} {1}.'.format( self.nEvents, self.nEvents_weighted ) )


  ## Run normal event generation
  def generateEvents(self) :
    ## Initialise timer
    time_start = time.time()
    self.logger.info( 'Starting Prophecy LHEF event generation at {0}'.format( time.ctime( time_start ) ) )

    ## Setup heartbeat thread
    heartbeat = HeartbeatTimer(600., "{}/eventLoopHeartBeat.txt".format(self.__run_directory))
    heartbeat.setName("heartbeat thread")
    heartbeat.daemon = True  # Allow program to exit if this is the only live thread
    heartbeat.start()

    ## Remove any existing .lhe files to avoid repeated events
    self.logger.info( 'Removing old Prophecy LHE files' )
    try :
      os.remove( 'plot*.lhe plot*.ev*ts' )
    except OSError :
      pass

    ## The following is only needed for newer installation of prophecy?? RDS 2019/07
    ## Create dirs UNWEIGHTEDEVENTS and HISTUNWEIGHTED
    if not os.path.exists('UNWEIGHTEDEVENTS'):
      os.makedirs('UNWEIGHTEDEVENTS')
      self.logger.info( 'Created directory UNWEIGHTEDEVENTS' )

    ## Initialise generation process tracker
    self.running_process = []

    self.runProphecy()

    ## Display generation output until finished then kill heartbeat thread
    heartbeat.cancel()

    ## Print timing information
    generation_end = time.time()
    elapsed_time = generation_end - time_start
    self.logger.info( 'Running nominal Prophecy took {0} for {1} events => {2:6.3f} Hz'.format( HeartbeatTimer.readable_duration(elapsed_time), self.nEvents, self.nEvents / elapsed_time ) )

    self.logger.info( 'Removing Prophecy born LHE file' )
    try :
      os.remove( 'plot_unweighted_born.lhe' )
    except OSError :
      pass

    ## Move output to correctly named file
    try :
      os.rename( 'UNWEIGHTEDEVENTS/plot_unweighted.lhe', self.output_events_file_name )
      self.logger.info( 'Moved plot_unweighted.lhe to {0}'.format(self.output_events_file_name) )
    except OSError :
      self.logger.warning( 'No output LHEF file found! Probably because the Prophecy process was killed before finishing.' )

    ## Print finalisation message
    self.logger.info( 'Finished at {0}'.format( time.asctime() ) )
    return


  ## Register configurable parameter
  def add_parameter( self, configurable_name, value, desc='', parameter=None ) :
    setattr( self, configurable_name, value ) # add new attribute
    prophecy_parameter = parameter if parameter is not None else configurable_name
    self.configurable_parameters[prophecy_parameter] = ( configurable_name, desc )


  ## Alias to DecoratorFactory.decorate
  def add_parameter_set( self, parameter_set, **kwargs ) :
    return decorate( self, parameter_set, **kwargs )


  ## Output a heartbeat message
  def emit_heartbeat(self, duration) :
    message = 'Heartbeat: Prophecy generation has been running for {0} in total'.format( RepeatingTimer.human_readable_time_interval(duration) ) # noqa: F821
    self.logger.info( message )
    with open( '{0}/eventLoopHeartBeat.txt'.format( self.__run_directory ), 'w' ) as f : f.write( message )

  ## Register non-configurable parameter
  def fix_parameter( self, parameter, value=None, desc='' ) :
    # Get previously set value if not overwriting
    if value is None : value = getattr( self, parameter )
    # Remove it from the configurable list if it was there
    for prophecy_parameter, configurable_name_tuple in self.configurable_parameters.items() :
      # Retrieve Prophecy parameter name and description if there is a match
      if parameter == configurable_name_tuple[0] :
        parameter, desc = prophecy_parameter, configurable_name_tuple[1]
        self.configurable_parameters.pop(prophecy_parameter)
        break
    self.fixed_parameters.append( (parameter, value, desc) )

  def runProphecy(configurator) :
    configurator.running_process.append(subprocess.Popen( [configurator._prophecy_executable,''], stdout=subprocess.PIPE, stdin=open(configurator.run_card_path), stderr=subprocess.STDOUT ) )

    while configurator.running_process :
     # Write output buffer if any
     for process in configurator.running_process :
       while True :
         output = process.stdout.readline().rstrip()
         if len(output) == 0 : break
         configurator.logger.info( '{0}'.format(output) )
       if process.poll() is not None : # process has ended
         # Flush buffer and print final output (if any) to screen
         process.stdout.flush()
         while True :
           output = process.stdout.readline().rstrip()
           if len(output) == 0 : break
           configurator.logger.info( '{0}'.format(output) )
         # Close output stream and remove process from list
         process.stdout.close()
         configurator.running_process.remove( process )
         configurator.logger.info( 'Prophecy finished - all done.' )

  ## Get run directory
  @property
  def run_directory(self) :
    return self.__run_directory


  ## Get dictionary of configurable parameters
  @property
  def configurable_parameters(self) :
    return self.__configurable_parameters


  ## Get list of non-configurable parameters
  @property
  def fixed_parameters(self) :
    return self.__fixed_parameters


  ## Get handle to logger
  @property
  def logger(self) :
    return self.__logger


  ## Get output file name
  @property
  def output_events_file_name(self) :
    return self.__output_events_file_name

  ## Set output file name
  @output_events_file_name.setter
  def output_events_file_name(self, value) :
    self.__output_events_file_name = value


  ## Get Prophecy directory
  @property
  def prophecy_directory(self) :
    return self.__prophecy_directory


  ## Get list of enabled run card decorators
  @property
  def run_card_decorators(self) :
    return self.__run_card_decorators


  ## Get full path to runcard
  @property
  def run_card_path(self) :
    return '{0}/prophecy.input'.format( self.run_directory )

