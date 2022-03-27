# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import os, subprocess, time
from DecoratorFactory import decorate
from AthenaCommon import Logging
from PowhegControl.utility import HeartbeatTimer, RepeatingTimer
import collections

## Base class for configurable objects in the jobOptions
#
#  All subprocesses inherit from this class
class Hto4lConfig(object) :
  ## Set up run directory and path to Hto4l
  __run_directory = os.environ['PWD']

  # for local copy of Hto4l use:
  # __hto4l_directory = os.environ['TestArea']
  # For the lcg version of Hto4l use:
  # __hto4l_directory = '/cvmfs/sft-nightlies.cern.ch/lcg/views/dev4/Tue/x86_64-slc6-gcc62-opt/bin'
  # __hto4l_directory = '/cvmfs/sft-nightlies.cern.ch/lcg/views/dev4/Tue/x86_64-slc6-gcc62-opt/bin'
  # __hto4l_directory = '/cvmfs/sft-nightlies.cern.ch/lcg/nightlies/dev4/Tue/MCGenerators/hto4l/v2.01/x86_64-slc6-gcc62-opt/bin'
  # __hto4l_directory = '/cvmfs/sft.cern.ch/lcg/releases/LCG_81f/MCGenerators/hto4l/2.01/x86_64-slc6-gcc49-opt/bin'
  # __hto4l_directory = os.environ['HTO4LPATH']

  
  ## Setup athena-compatible logger
  __logger = Logging.logging.getLogger('Hto4lControl')

  ## This must be defined by each derived class - don't change it in the jobOptions!
  # _hto4l_executable = __hto4l_directory + '/Hto4l/Hto4l-v2.0/Hto4l'
  # for local copy of Hto4l use:
  # _hto4l_executable = __hto4l_directory + '/runGrid/Hto4l'
  # For the lcg version of Hto4l use:
  # _hto4l_executable = __hto4l_directory + '/Hto4l'

  # define Hto4l executable
  _hto4l_executable = 'Hto4l'

  def __init__( self, runArgs=None, opts=None ) :

    ## This needs to be set so that Generate_trf finds an appropriate file format for showering
    self.__output_events_file_name = 'Hto4lOTF._1.events'

    ## Set up lists of parameters and decorators
    self.__fixed_parameters = []
    # self.__configurable_parameters = {}
    self.__configurable_parameters = collections.OrderedDict()
    self.__run_card_decorators = []

    ## Add universal functionality
    self.add_parameter_set( 'fromDefault' )

    ## Initialise values from runArgs
    if runArgs is None :
      self.logger.warning( 'No run arguments found! Using defaults.' )
    else :
      # Read values from runArgs
      if hasattr(runArgs,'maxEvents') and runArgs.maxEvents > 0 :
        self.nEvents = int( runArgs.maxEvents + 0.5 ) # should be overwritten in the jobOpt - depends on 4e/4mu/2e2mu mode
      if hasattr(runArgs,'randomSeed') :
        self.random_seed = runArgs.randomSeed
        pass
      pass

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
    # for value_tuple in sorted( self.configurable_parameters.values(), key=lambda x: x[0].lower() ) :
    #   self.logger.info( ': {0:<22} : {1:>17} : {2}'.format( value_tuple[0], getattr(self, value_tuple[0]), value_tuple[1] ) )
    for value_tuple in self.configurable_parameters.values() :
      self.logger.info( ': {0:<22} : {1:>17} : {2}'.format( value_tuple[0], getattr(self, value_tuple[0]), value_tuple[1] ) )

    ## Add configurable parameters to fixed list
    [ self.fix_parameter( parameter=value_tuple[0], desc=value_tuple[1] ) for value_tuple in self.configurable_parameters.values() ]

    ## Write out final runcard
    self.logger.info( 'Writing Hto4l runcard to {0}'.format( self.run_card_path ) )
    with open( self.run_card_path, 'w' ) as f :
      # for parameter_tuple in sorted( self.fixed_parameters, key=lambda x: x[0].lower() ) :
      for parameter_tuple in self.fixed_parameters :
        name, value, desc = parameter_tuple
        # Set starting value to first in list when multiple values are provided
        if isinstance(value,list) :
          value = value[0]
          self.__enable_reweighting = True
        # output_line = '{0:<30}! {1}'.format( '{0}={1}'.format( name, value ), desc )
        output_line = '{0:<30}! {1}'.format( '{0}'.format( value ), desc )
        f.write( '{0}\n'.format(output_line) )
        self.logger.info( 'Wrote {0}'.format( output_line ) )

    ## Print final preparation message
    self.logger.info( 'Using executable: {0}'.format( self._hto4l_executable ) )


  ## Run normal event generation
  def generateEvents(self) :
    ## Initialise timer
    time_start = time.time()
    self.logger.info( 'Starting Hto4l LHEF event generation at {0}'.format( time.ctime( time_start ) ) )
    self.logger.info( 'Number of events: {0}.'.format( self.nEvents ) )

    # Setup heartbeat thread
    heartbeat = HeartbeatTimer(600., "{}/eventLoopHeartBeat.txt".format(self.__run_directory))
    heartbeat.setName("heartbeat thread")
    heartbeat.daemon = True  # Allow program to exit if this is the only live thread
    heartbeat.start()


    ## Initialise generation process tracker
    self.running_process = []

    self.runHto4l()

    ## Display generation output until finished then kill heartbeat thread
    heartbeat.cancel()

    ## Print timing information
    generation_end = time.time()
    elapsed_time = generation_end - time_start
    self.logger.info( 'Running nominal Hto4l took {0} for {1} events => {2:6.3f} Hz'.format( HeartbeatTimer.readable_duration(elapsed_time), self.nEvents, self.nEvents / elapsed_time ) )

    # self.logger.info( 'Removing Hto4l born LHE file' )
    # try :
    #   os.remove( 'plot_unweighted_born.lhe' )
    # except OSError :
    #   pass

    # ## Move output to correctly named file
    # try :
    #   os.rename( 'plot_unweighted.lhe', self.output_events_file_name )
    #   self.logger.info( 'Moved plot_unweighted.lhe to {0}'.format(self.output_events_file_name) )
    # except OSError :
    #   self.logger.warning( 'No output LHEF file found! Probably because the Hto4l process was killed before finishing.' )

    ## Print finalisation message
    self.logger.info( 'Finished at {0}'.format( time.asctime() ) )
    return


  ## Register configurable parameter
  def add_parameter( self, configurable_name, value, desc='', parameter=None ) :
    setattr( self, configurable_name, value ) # add new attribute
    hto4l_parameter = parameter if parameter is not None else configurable_name
    self.configurable_parameters[hto4l_parameter] = ( configurable_name, desc )


  ## Alias to DecoratorFactory.decorate
  def add_parameter_set( self, parameter_set, **kwargs ) :
    return decorate( self, parameter_set, **kwargs )


  ## Output a heartbeat message
  def emit_heartbeat(self, duration) :
    message = 'Heartbeat: Hto4l generation has been running for {0} in total'.format( RepeatingTimer.human_readable_time_interval(duration) )
    self.logger.info( message )
    with open( '{0}/eventLoopHeartBeat.txt'.format( self.__run_directory ), 'w' ) as f : f.write( message )


  ## Register non-configurable parameter
  def fix_parameter( self, parameter, value=None, desc='' ) :
    # Get previously set value if not overwriting
    if value is None : value = getattr( self, parameter )
    # Remove it from the configurable list if it was there
    for hto4l_parameter, configurable_name_tuple in self.configurable_parameters.items() :
      # Retrieve Hto4l parameter name and description if there is a match
      if parameter == configurable_name_tuple[0] :
        parameter, desc = hto4l_parameter, configurable_name_tuple[1]
        self.configurable_parameters.pop(hto4l_parameter)
        break
    self.fixed_parameters.append( (parameter, value, desc) )

  def runHto4l(configurator) :
    configurator.running_process.append(subprocess.Popen( [configurator._hto4l_executable,''], stdout=subprocess.PIPE, stdin=open(configurator.run_card_path), stderr=subprocess.STDOUT ) )

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
         configurator.logger.info( 'Hto4l finished - all done.' )

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

  # ## Get Hto4l directory
  # @property
  # def hto4l_directory(self) :
  #   return self.__hto4l_directory


  ## Get list of enabled run card decorators
  @property
  def run_card_decorators(self) :
    return self.__run_card_decorators


  ## Get full path to runcard
  @property
  def run_card_path(self) :
    return '{0}/hto4l.input'.format( self.run_directory )

