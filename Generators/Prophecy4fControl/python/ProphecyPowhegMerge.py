# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import os, subprocess, time, tarfile
from AthenaCommon import Logging
from PowhegControl.utility import HeartbeatTimer

## Base class for configurable objects in the jobOptions
#
#  All subprocesses inherit from this class
class ProphecyPowhegMerge(object) :
  ## Set up run directory and path to Prophecy
  __run_directory = os.environ['PATH']

  ## Setup athena-compatible logger
  __logger = Logging.logging.getLogger('ProphecyPowhegMerger')

  ## This must be defined by each derived class - don't change it in the jobOptions!
  _merger_executable = 'mergeProphecy4f.exe'

  def __init__( self, runArgs=None, opts=None ) :

    ## This needs to be set so that Generate_trf finds an appropriate file format for showering
    self.__output_events_file_name = 'ProphecyPowhegMergedOTF._1.events'

    
    ## Using default output names from PowhegConfig_base and ProphecyConfig
    self.__input_powheg_file_name        = 'PowhegOTF._1.events'
    self.__input_prophecy4e_file_name    = 'ProphecyOTF4e._1.events'
    self.__input_prophecy4mu_file_name   = 'ProphecyOTF4mu._1.events'
    self.__input_prophecy2e2mu_file_name = 'ProphecyOTF2e2mu._1.events'
    self.__random_seed                   = 0
    
    ## Initialise values from runArgs
    if runArgs is None :
      self.logger.warning( 'No run arguments found! Using defaults.' )
    else :
      # Read values from runArgs
      # Set inputGeneratorFile to match output events file. Otherwise Generate_trf check will fail.
      runArgs.inputGeneratorFile = self.output_events_file_name

  ## Initialise runcard with generic options
  def merge(self) :

    self.logger.info( 'Starting ProphecyPowhegMerger merge' )

    powhegLHE        = self.input_powheg_file_name
    prophecyLHE4e    = self.input_prophecy4e_file_name
    prophecyLHE4mu   = self.input_prophecy4mu_file_name
    prophecyLHE2e2mu = self.input_prophecy2e2mu_file_name
    random_seed      = str(self.random_seed)

    myinputfiles  = powhegLHE
    genInputFiles = myinputfiles.split(',')
    numberOfFiles = len(genInputFiles)
    # if there is a single file, make a symlink.  If multiple files, merge them into one output eventsFile
    if numberOfFiles > 0:
      allFiles = []
      for file in genInputFiles:
        file_1 = file
        # untar as needed
        if tarfile.is_tarfile(file):
          tar = tarfile.open(file)
          tar.extractall()
          tar.close()
          file_1 = file.replace("tar.gz.1","events")
          self.logger.info( 'Extracted tar file, and renaming {0} to {1}'.format ( file, file_1 ) )
          pass
           
        # The only input format where merging is permitted is LHE
        with open(file_1, 'r') as f:
          first_line = f.readline()
          self.logger.info( 'first_line {0}'.format ( first_line ) )
          if(not ("LesHouche" in first_line)):
            raise RuntimeError("%s is NOT a LesHouche file" % file)
          allFiles.append(file_1)
      powhegLHE_input = "merged_powheg_events.lhe"
      self.merge_lhe_files(allFiles, powhegLHE_input)
    
    ## Initialise timer
    time_start = time.time()
    self.logger.info( 'Input files: {0} {1} {2} {3}'.format( powhegLHE_input, prophecyLHE4e, prophecyLHE4mu, prophecyLHE2e2mu ) )

    ## Setup heartbeat thread
    heartbeat = HeartbeatTimer(600., "{}/eventLoopHeartBeat.txt".format(self.__run_directory))
    heartbeat.setName("heartbeat thread")
    heartbeat.daemon = True  # Allow program to exit if this is the only live thread
    heartbeat.start()

    ## check if input files exist
    self.logger.info( 'Checking if {0} exists.'.format( powhegLHE_input ) )
    if not os.path.isfile( powhegLHE_input ):
      self.logger.error( 'File {0} does NOT exist.'.format( powhegLHE_input ) )
      raise ValueError('File {0} does NOT exist.'.format( powhegLHE_input ))

    self.logger.info( 'Checking if {0} exists.'.format( prophecyLHE4e ) )
    if not os.path.isfile( prophecyLHE4e ):
      self.logger.error( 'File {0} does NOT exist.'.format( prophecyLHE4e ) )
      raise ValueError('File {0} does NOT exist.'.format( prophecyLHE4e ))

    self.logger.info( 'Checking if {0} exists.'.format( prophecyLHE4mu ) )
    if not os.path.isfile( prophecyLHE4mu ):
      self.logger.error( 'File {0} does NOT exist.'.format( prophecyLHE4mu ) )
      raise ValueError('File {0} does NOT exist.'.format( prophecyLHE4mu ))

    self.logger.info( 'Checking if {0} exists.'.format( prophecyLHE2e2mu ) )
    if not os.path.isfile( prophecyLHE2e2mu ):
      self.logger.error( 'File {0} does NOT exist.'.format( prophecyLHE2e2mu ) )
      raise ValueError('File {0} does NOT exist.'.format( prophecyLHE2e2mu ))

    self.logger.info( 'Input files found. Moving them to temporary files to produce properly named final output {0}.'.format( self.output_events_file_name ) )

    try :
      os.rename( prophecyLHE4e, prophecyLHE4e + '.tmp' )
    except OSError :
      self.logger.error( 'Moving of file {0} failed - not expected.'.format( prophecyLHE4e ) )

    try :
      os.rename( prophecyLHE4mu, prophecyLHE4mu + '.tmp' )
    except OSError :
      self.logger.error( 'Moving of file {0} failed - not expected.'.format( prophecyLHE4mu ) )

    try :
      os.rename( prophecyLHE2e2mu, prophecyLHE2e2mu + '.tmp' )
    except OSError :
      self.logger.error( 'Moving of file {0} failed - not expected.'.format( prophecyLHE2e2mu ) )

    self.running_process = []

    self.runMerging(powhegLHE_input, prophecyLHE4e + '.tmp', prophecyLHE4mu + '.tmp', prophecyLHE2e2mu + '.tmp', random_seed)
    ## Display generation output until finished then kill heartbeat thread
    heartbeat.cancel()

    ## Print timing information
    generation_end = time.time()
    elapsed_time = generation_end - time_start
    self.logger.info( 'Running ProphecyPowhegMerger took {0}.'.format( HeartbeatTimer.readable_duration(elapsed_time) ) )

    self.logger.info( 'Removing initial LHE files of Prophecy and Powheg stored as *tmp.' )

    ## Print finalisation message
    self.logger.info( 'Finished at {0}'.format( time.asctime() ) )
    return


  def runMerging(configurator, powhegLHE, prophecyLHE4e, prophecyLHE4mu, prophecyLHE2e2mu, random_seed, stdin=None) :
    configurator.logger.info( 'runMerging on {0}, {1}, {2} and {3}'.format( powhegLHE, prophecyLHE4e, prophecyLHE4mu, prophecyLHE2e2mu ) )
    if configurator.logger.level >= Logging.logging.DEBUG :
      configurator.running_process.append(subprocess.Popen( [configurator._merger_executable,'--inPowheg',powhegLHE,'--inProphecy4e',prophecyLHE4e,'--inProphecy4mu',prophecyLHE4mu,'--inProphecy2e2mu',prophecyLHE2e2mu,'--outLHE',configurator.output_events_file_name,'--randomSeed',random_seed,'--debug'], stdout=subprocess.PIPE, stdin=stdin, stderr=subprocess.STDOUT ) )
    else :
      configurator.running_process.append(subprocess.Popen( [configurator._merger_executable,'--inPowheg',powhegLHE,'--inProphecy4e',prophecyLHE4e,'--inProphecy4mu',prophecyLHE4mu,'--inProphecy2e2mu',prophecyLHE2e2mu,'--outLHE',configurator.output_events_file_name,'--randomSeed',random_seed,'--debug'], stdout=subprocess.PIPE, stdin=stdin, stderr=subprocess.STDOUT ) )
    configurator.logger.info( 'runMerging run mergeProphecy4f: --inPowheg {0} --inProphecy4e {1} --inProphecy4mu {2} --inProphecy2e2mu {3} --outLHE {4} --randomSeed {5}'.format( powhegLHE, prophecyLHE4e, prophecyLHE4mu, prophecyLHE2e2mu, configurator.output_events_file_name,random_seed) )

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

  ## Set input Powheg file name
  @input_powheg_file_name.setter
  def input_powheg_file_name(self, value) :
    self.__input_powheg_file_name = value

  ## Get input Prophecy file name
  @property
  def input_prophecy4e_file_name(self) :
    return self.__input_prophecy4e_file_name

  ## Get input Prophecy file name
  @property
  def input_prophecy4mu_file_name(self) :
    return self.__input_prophecy4mu_file_name

  @property
  def input_prophecy2e2mu_file_name(self) :
    return self.__input_prophecy2e2mu_file_name

  @property
  def random_seed(self) :
    return self.__random_seed
  
  ## Set input Prophecy file name
  @input_prophecy4e_file_name.setter
  def input_prophecy4e_file_name(self, value) :
    self.__input_prophecy4e_file_name = value

  ## Set input Prophecy file name
  @input_prophecy4mu_file_name.setter
  def input_prophecy4mu_file_name(self, value) :
    self.__input_prophecy4mu_file_name = value

  @input_prophecy2e2mu_file_name.setter
  def input_prophecy2e2mu_file_name(self, value) :
    self.__input_prophecy2e2mu_file_name = value

  @random_seed.setter
  def random_seed(self, value) :
    self.__random_seed = value

  ## Get handle to logger
  @property
  def logger(self) :
    return self.__logger

  # This function merges a list of input LHE file to make one outputFile.  The header is taken from the first
  # file, but the number of events is updated to equal the total number of events in all the input files
  def merge_lhe_files(self, listOfFiles, outputFile):
      if(os.path.exists(outputFile)):
        self.logger.info( 'outputFile {0} already exists.  Will rename to  {1}.OLD'.format ( outputFile, outputFile ) )
        os.rename(outputFile,outputFile+".OLD")
      output = open(outputFile,'w')
      holdHeader = ""
      nevents=0
      for file in listOfFiles:
         cmd = "grep /event "+file+" | wc -l"
         nevents+=int(subprocess.check_output(cmd,stderr=subprocess.STDOUT,shell=True))
  
      for file in listOfFiles:
         inHeader = True
         header = ""
         self.logger.info( '*** Starting to merge file {0}'.format ( file ) )
         for line in open(file,"r"):
           ## Reading first event signals that we are done with all the header information
           ## Using this approach means the script will properly handle any metadata stored
           ## at the beginning of the file.  Note:  aside from the number of events, no metadata
           ## is updated after the first header is read (eg the random number seed recorded will be
           ## that of the first file.
            if("<event" in line and inHeader):
               inHeader = False
               if(len(holdHeader)<1):
                  holdHeader = header
                  output.write(header)
               output.write(line)
               ## each input file ends with "</LesHouchesEvents>".  We don't want to write this out until all
               ## the files have been read.  The elif below writes out all the events.
            elif(not inHeader and not ("</LesHouchesEvents>" in line)):
                output.write(line)
            if(inHeader):
              ## Format for storing number of events different in MG and Powheg 
               if("nevents" in line):
                 ## MG5 format is "n = nevents"
                  tmp = line.split("=")
                  line = line.replace(tmp[0],str(nevents))
               elif("numevts" in line):
                 ## Powheg format is numevts n
                  tmp = line.split(" ")
                  nnn = str(nevents)
                  line = line.replace(tmp[1],nnn)
               header+=line
      output.write("</LesHouchesEvents>\n")
      output.close()
    
