# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import os, subprocess, tarfile
from AthenaCommon import Logging
from PowhegControl.utility import LHE
# from ...utility import LHE
# from xml.etree import ElementTree
import shutil

## Base class for configurable objects in the jobOptions
#
#  All subprocesses inherit from this class
class Hto4lPowhegMerge(object) :
  ## Set up run directory and path to Hto4l
  __run_directory = os.environ['PATH']

  ## Setup athena-compatible logger
  __logger = Logging.logging.getLogger('Hto4lPowhegMerger')

  ## This must be defined by each derived class - don't change it in the jobOptions!
  _merger_executable = 'mergeHto4l4f.exe'

  def __init__( self, runArgs=None, opts=None ) :

    ## This needs to be set so that Generate_trf finds an appropriate file format for showering
    self.__output_events_file_name = 'Hto4lPowhegMergedOTF._1.events'
    # self.__output_events_file_name = 'events.lhe'

    
    ## Using default output names from PowhegConfig_base and Hto4lConfig
    self.__output_hto4l4e_file_name    = 'Hto4lOTF4e._1.events'
    self.__output_hto4l4mu_file_name   = 'Hto4lOTF4mu._1.events'
    self.__output_hto4l2e2mu_file_name = 'Hto4lOTF2e2mu._1.events'
    self.__random_seed                   = 0
    
    ## Initialise values from runArgs
    if runArgs is None :
      self.logger.warning( 'No run arguments found! Using defaults.' )
    else :
      # Read values from runArgs
      # if hasattr(runArgs,'outputTXTFile') :
        # self.logger.info( 'outputTXTFile {0} {1}'.format (runArgs.outputTXTFile, type(runArgs.outputTXTFile) ) )
        # self.__output_events_file_name = runArgs.outputTXTFile
        # self.output_events_file_name = runArgs.outputTXTFile.split('.tar.gz')[0]+'.events'
      # Set inputGeneratorFile to match output events file. Otherwise Generate_trf check will fail.

      runArgs.inputGeneratorFile = self.output_events_file_name
      # runArgs.inputGeneratorFile = "Hto4lOTF4mu._1.events"

  ## Initialise runcard with generic options
  def setUpInput(self) :

    self.logger.info( 'entering setUpInput')

    # we require four input files, 1 for 4e, 1 for 4mu, and 2 for 2e2mu. The two for 2e2mu will be merged
    myinputfiles  = self.input_powheg_file_name
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
        allFiles.append(file_1)
        with open(file_1, 'r') as f:
          first_line = f.readline()
          self.logger.info( 'first_line {0}'.format ( first_line ) )
          if(not ("LesHouche" in first_line)):
            raise RuntimeError("%s is NOT a LesHouche file" % file)
          pass
        pass

      self.logger.info( 'Found files: nf {0}'.format( len(allFiles) ) )
      for file in allFiles:
        self.logger.info( ' {0}'.format( file ) )
        pass

      # Merge files for 4e, 4mu and 2e2mu
      self.__input_powheg_to_hto4l_file_name  = "merged_hto4l_events.lhe"
      self.merge_lhe_files(allFiles,    self.input_powheg_to_hto4l_file_name)
      self.logger.info( 'Merged: {0} to {1}'.format ( allFiles,    self.input_powheg_to_hto4l_file_name ) )
    else:
      self.__input_powheg_to_hto4l_file_name  = myinputfiles
      self.logger.info( 'Using single input file: {0}'.format ( self.input_powheg_to_hto4l_file_name ) )

  ## Initialise runcard with generic options
  def merge(self) :

    self.logger.info( 'Starting merge' )

    hto4lLHE4e       = self.output_hto4l4e_file_name
    hto4lLHE4mu      = self.output_hto4l4mu_file_name
    hto4lLHE2e2mu    = self.output_hto4l2e2mu_file_name
#    powhegLHE_output = self.output_events_file_name

    self.logger.info( 'Input files: {0} {1} {2}'.format( hto4lLHE4e, hto4lLHE4mu, hto4lLHE2e2mu ) )
    
    allFiles = []
    allFiles += [ hto4lLHE4e    ]
    allFiles += [ hto4lLHE4mu   ]
    allFiles += [ hto4lLHE2e2mu ]
    self.merge_lhe_files(allFiles, self.output_events_file_name)

    self.logger.info( 'Merged to output file: {0}'.format( self.output_events_file_name ) )

    # Now fix the output file for negative weights
    self.reweight_for_negative_weights(self.output_events_file_name)

    self.logger.info( 'Fixed weights in file: {}'.format( self.output_events_file_name ) )

    
    return

  def reweight_for_negative_weights(self, powheg_LHE_output):
    """Post-process the LHE file to update the weights for a negative Hto4l weight.
    We do two things:
      1) check if the event weight (XWGTUP) is -1 (can be +/-1). If so, remember this so that the 
         weights in the <rwgt> block can be corrected by multiplying by -1
      2) After correcting the weights in the <rwgt> block, overwrite XWGTUP by the weight at id=0.
         Not completely sure if this step is necessary, but is done for Prophecy4f as well.
    Note: we do NOT save the original XWGTUP as an extra weight (i.e. +/-1). Normally, the sign of 
    any weight should indicate this. The one exception is when BOTH PowHeg and Hto4l has a negative 
    weight and so all weights are positive. This is pretty rare.

    #@param powheg_LHE_output  Name of LHE file produced by merge of Hto4l files.

    @author RD Schaffer <r.d.schaffer@cern.ch>
    #"""

    self.logger.info( 'Starting reweight_for_negative_weights' )

    # Get opening and closing strings
    preamble = LHE.preamble(powheg_LHE_output)
    postamble = LHE.postamble(powheg_LHE_output) 

    # open new file for updates
    powheg_LHE_updated = "{}.updated".format(powheg_LHE_output)
    with open(powheg_LHE_updated, "wb") as f_output:
      f_output.write("{}".format(preamble))
      # for each event, check for an -1 Hto4l weight in XWGTUP. If it is -1, correct the <rwgt>
      # weights (-1 * wgt), and save the id=0 weight in XWGTUP.
      for input_event in LHE.event_iterator(powheg_LHE_output):

        output_event = ""
        has_neg_wgt  = False

        # First check for negative weights
        input_lines = input_event.splitlines(True)

        try:  # interpret 2nd line as a general event info line
          NUP, IDPRUP, XWGTUP, SCALUP, AQEDUP, AQCDUP = input_lines[1].split()
          has_neg_wgt = float(XWGTUP) < 0.

          self.logger.info( 'has_neg_wgt {}, XWGTUP {}'.format(has_neg_wgt, XWGTUP))
            
        except ValueError:  # this is not a general event info line
          self.logger.warning( 'could not get first line of event')
          pass
          
        # Now correct weights
        output_event = ""
        if has_neg_wgt:
          for input_line in input_event.splitlines(True):
            self.logger.info( 'input line {}'.format(input_line))              
            output_line = None
            if input_line.find("<wgt") >= 0:
              weights = input_line.split()
              self.logger.info( 'weightsinput {}'.format(weights))              
              try:
                wgt_tag, wid, wgt, wgt_tag_end = input_line.split()
                # We 'multiply' the two weights - if wgt is negative, it now becomes positive.
                # Otherwise, set it to negative
                if wgt[0] == '-':
                  output_line = "{} {}  {} {} \n".format(wgt_tag, wid, wgt[1:], wgt_tag_end)
                else:
                  output_line = "{} {}  -{} {} \n".format(wgt_tag, wid, wgt, wgt_tag_end)
              except Exception:
                pass
              self.logger.info( 'output line {}'.format(output_line))              
              
            # save event lines
            output_event += output_line if output_line is not None else input_line
        else:
          output_event = input_event

        # Overwrite (XWGTUP)
        output_event_updated = LHE.update_XWGTUP_with_reweighted_nominal(output_event)
        f_output.write(output_event_updated)
            
      f_output.write(postamble)
      # f_output.close()

    # Make a backup of the original events
    shutil.move(powheg_LHE_output, "{}.lhe_nominal_weight_updater_backup".format(powheg_LHE_output))
    shutil.move(powheg_LHE_updated, powheg_LHE_output)
    
    self.logger.info( 'wrote file {}'.format(powheg_LHE_output))
  
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

  ## Get input Hto4l file name
  @property
  def input_powheg_to_hto4l_file_name(self) :
    return self.__input_powheg_to_hto4l_file_name

  ## Get output Hto4l file name for 4e
  @property
  def output_hto4l4e_file_name(self) :
    return self.__output_hto4l4e_file_name

  ## Get output Hto4l file name for 4mu
  @property
  def output_hto4l4mu_file_name(self) :
    return self.__output_hto4l4mu_file_name

  ## Get output Hto4l file name for 2e2mu
  @property
  def output_hto4l2e2mu_file_name(self) :
    return self.__output_hto4l2e2mu_file_name

  ## Set output Hto4l file name for 4e
  @output_hto4l4e_file_name.setter
  def output_hto4l4e_file_name(self, value) :
    self.__output_hto4l4e_file_name = value

  ## Set output Hto4l file name for 4mu
  @output_hto4l4mu_file_name.setter
  def output_hto4l4mu_file_name(self, value) :
    self.__output_hto4l4mu_file_name = value

  ## Set output Hto4l file name for 2e2mu
  @output_hto4l2e2mu_file_name.setter
  def output_hto4l2e2mu_file_name(self, value) :
    self.__output_hto4l2e2mu_file_name = value

  @property
  def random_seed(self) :
    return self.__random_seed
  
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
    
