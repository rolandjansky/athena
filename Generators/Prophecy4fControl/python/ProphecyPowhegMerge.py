#! /usr/bin/env python
import os, subprocess, time, glob
from AthenaCommon import Logging
from PowhegControl import RepeatingTimer

## Base class for configurable objects in the jobOptions
#
#  All subprocesses inherit from this class
class ProphecyPowhegMerge(object) :
  ## Set up base directory and path to Prophecy
  __base_directory = os.environ['PATH']

  ## Setup athena-compatible logger
  __logger = Logging.logging.getLogger('ProphecyPowhegMerger')

  ## This must be defined by each derived class - don't change it in the jobOptions!
  _merger_executable = 'DRM.exe'

  def __init__( self, runArgs=None, opts=None ) :

    ## This needs to be set so that Generate_trf finds an appropriate file format for showering
    self.__output_events_file_name = 'ProphecyPowhegMergedOTF._1.events'

    ## Using default output names from PowhegConfig_base and ProphecyConfig
    self.__input_powheg_file_name = 'PowhegOTF._1.events'
    self.__input_prophecy_file_name = 'ProphecyOTF._1.events'

    ## Initialise values from runArgs
    if runArgs == None :
      self.logger.warning( 'No run arguments found! Using defaults.' )
    else :
      # Read values from runArgs
      if hasattr(runArgs,'outputTXTFile') :
        self.output_events_file_name = runArgs.outputTXTFile.split('.tar.gz')[0]+'.events'
      ## Set inputGeneratorFile to match output events file. Otherwise Generate_trf check will fail.
      runArgs.inputGeneratorFile = self.output_events_file_name

  ## Initialise runcard with generic options
  def merge(self) :

    powhegLHE=self.input_powheg_file_name
    prophecyLHE=self.input_prophecy_file_name

    ## Initialise timer
    time_start = time.time()
    self.logger.info( 'Starting ProphecyPowhegMerger now.' )

    ## Setup heartbeat thread
    heartbeat = RepeatingTimer( 600., lambda: self.emit_heartbeat( time.time() - time_start ) )
    heartbeat.daemon = True # Allow program to exit if this is the only live thread
    heartbeat.start()

    ## check if input files exist
    self.logger.info( 'Checking if {0} exists.'.format( powhegLHE ) )
    try :
      os.path.isfile( powhegLHE )
    except OSError :
      self.logger.error( 'File {0} does NOT exist.'.format( powhegLHE ) )

    self.logger.info( 'Checking if {0} exists.'.format( prophecyLHE ) )
    try :
      os.path.isfile( prophecyLHE )
    except OSError :
      self.logger.error( 'File {0} does NOT exist.'.format( prophecyLHE ) )

    self.logger.info( 'Input files found. Moving them to temporary files to produce properly named final output {0}.'.format( self.output_events_file_name ) )

    try :
      os.rename( powhegLHE, powhegLHE + '.tmp' )
    except OSError :
      self.logger.error( 'Moving of file {0} failed - not expected.'.format( powhegLHE ) )

    try :
      os.rename( prophecyLHE, prophecyLHE + '.tmp' )
    except OSError :
      self.logger.error( 'Moving of file {0} failed - not expected.'.format( prophecyLHE ) )

    self.running_process = []

    self.runMerging(powhegLHE + '.tmp', prophecyLHE + '.tmp')

    ## Display generation output until finished then kill heartbeat thread
    heartbeat.cancel()

    ## Print timing information
    generation_end = time.time()
    elapsed_time = generation_end - time_start
    self.logger.info( 'Running ProphecyPowhegMerger took {0}.'.format( RepeatingTimer.human_readable_time_interval(elapsed_time) ) )

    self.logger.info( 'Removing initial LHE files of Prophecy and Powheg stored as *tmp.' )
    try :
      for fl in glob.glob('*.lhe.tmp'): os.remove(fl)
      for fl in glob.glob('*.ev*ts.tmp'): os.remove(fl)
    except OSError :
      pass

    ## Print finalisation message
    self.logger.info( 'Finished at {0}'.format( time.asctime() ) )
    return


  def runMerging(configurator, powhegLHE, prophecyLHE, stdin=None) :
    if configurator.logger.level >= Logging.logging.DEBUG :
      configurator.running_process.append(subprocess.Popen( [configurator._merger_executable,'--inPowheg',powhegLHE,'--inProphecy',prophecyLHE,'--outLHE',configurator.output_events_file_name,'--debug'], stdout=subprocess.PIPE, stdin=stdin, stderr=subprocess.STDOUT ) )
    else :
      configurator.running_process.append(subprocess.Popen( [configurator._merger_executable,'--inPowheg',powhegLHE,'--inProphecy',prophecyLHE,'--outLHE',configurator.output_events_file_name], stdout=subprocess.PIPE, stdin=stdin, stderr=subprocess.STDOUT ) )

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
         configurator.logger.info( 'Merging finished - all done.' )

  ## Get output file name
  @property
  def output_events_file_name(self) :
    return self.__output_events_file_name

  ## Get input Powheg file name
  @property
  def input_powheg_file_name(self) :
    return self.__input_powheg_file_name

  ## Get input Prophecy file name
  @property
  def input_prophecy_file_name(self) :
    return self.__input_prophecy_file_name

  ## Get handle to logger
  @property
  def logger(self) :
    return self.__logger
