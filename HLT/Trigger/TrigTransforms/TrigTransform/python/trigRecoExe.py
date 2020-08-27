#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @brief: Trigger executor to call base transforms
# @details: Based on athenaExecutor with some modifications
# @author: Mark Stockton

import logging
msg = logging.getLogger("PyJobTransforms." + __name__)

import os
import fnmatch
import re
import subprocess
import six

from PyJobTransforms.trfExe import athenaExecutor

#imports for preExecute
from PyJobTransforms.trfUtils import asetupReport, cvmfsDBReleaseCheck, lineByLine
import PyJobTransforms.trfEnv as trfEnv
import PyJobTransforms.trfExceptions as trfExceptions
from PyJobTransforms.trfExitCodes import trfExit as trfExit
import TrigTransform.dbgAnalysis as dbgStream
from TrigTransform.trigTranslate import getTranslated as getTranslated


class trigRecoExecutor(athenaExecutor):
    # Trig_reco_tf.py executor for BS-BS step
    # used to setup input files/arguments and change output filenames
    
    #preExe is based on athenaExecutor but with key changes:
    # - removed athenaMP
    # - removed environment so does not require the noimf notcmalloc flags
    # - added swap of argument name for runargs file so that athenaHLT reads it in

    def preExecute(self, input = set(), output = set()):
        msg.debug('Preparing for execution of {0} with inputs {1} and outputs {2}'.format(self.name, input, output))
        ## Try to detect AthenaMP mode
        #self._athenaMP = self._detectAthenaMP()
        #
        ## And if this is athenaMP, then set some options for workers and output file report
        #if self._athenaMP:
        #    self._athenaMPWorkerTopDir = 'athenaMP-workers-{0}-{1}'.format(self._name, self._substep)
        #    self._athenaMPFileReport = 'athenaMP-outputs-{0}-{1}'.format(self._name, self._substep)
        #else:
        #    self._athenaMPWorkerTopDir = self._athenaMPFileReport = None


        # Check we actually have events to process!
        if (self._inputEventTest and 'skipEvents' in self.conf.argdict and 
            self.conf.argdict['skipEvents'].returnMyValue(name=self._name, substep=self._substep, first=self.conf.firstExecutor) is not None):
            msg.debug('Will test for events to process')
            for dataType in input:
                inputEvents = self.conf.dataDictionary[dataType].nentries
                msg.debug('Got {0} events for {1}'.format(inputEvents, dataType))
                if not isinstance(inputEvents, six.integer_types):
                    msg.warning('Are input events countable? Got nevents={0} so disabling event count check for this input'.format(inputEvents))
                elif self.conf.argdict['skipEvents'].returnMyValue(name=self._name, substep=self._substep, first=self.conf.firstExecutor) >= inputEvents:
                    raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_NOEVENTS'),
                                                                    'No events to process: {0} (skipEvents) >= {1} (inputEvents of {2}'.format(self.conf.argdict['skipEvents'].returnMyValue(name=self._name, substep=self._substep, first=self.conf.firstExecutor), inputEvents, dataType))
    
        ## Write the skeleton file and prep athena
        if self._skeleton is not None:
            inputFiles = dict()
            for dataType in input:
                inputFiles[dataType] = self.conf.dataDictionary[dataType]
            outputFiles = dict()
            for dataType in output:
                outputFiles[dataType] = self.conf.dataDictionary[dataType]
                
            # See if we have any 'extra' file arguments
            for dataType, dataArg in self.conf.dataDictionary.items():
                if dataArg.io == 'input' and self._name in dataArg.executor:
                    inputFiles[dataArg.subtype] = dataArg
                
            msg.info('Input Files: {0}; Output Files: {1}'.format(inputFiles, outputFiles))
            
            # Get the list of top options files that will be passed to athena (=runargs file + all skeletons)
            self._topOptionsFiles = self._jobOptionsTemplate.getTopOptions(input = inputFiles, 
                                                                           output = outputFiles)

        ## Add input/output file information - this can't be done in __init__ as we don't know what our
        #  inputs and outputs will be then
        if len(input) > 0:
            self._extraMetadata['inputs'] = list(input)
        if len(output) > 0:
            self._extraMetadata['outputs'] = list(output)

        ## Do we need to run asetup first?
        asetupString = None
        if 'asetup' in self.conf.argdict:
            asetupString = self.conf.argdict['asetup'].returnMyValue(name=self._name, substep=self._substep, first=self.conf.firstExecutor)
        else:
            msg.info('Asetup report: {0}'.format(asetupReport()))
        
        ## DBRelease configuration
        dbrelease = dbsetup = None
        if 'DBRelease' in self.conf.argdict:
            dbrelease = self.conf.argdict['DBRelease'].returnMyValue(name=self._name, substep=self._substep, first=self.conf.firstExecutor)
            if dbrelease:
                # Classic tarball - filename format is DBRelease-X.Y.Z.tar.gz
                dbdMatch = re.match(r'DBRelease-([\d\.]+)\.tar\.gz', os.path.basename(dbrelease))
                if dbdMatch:
                    msg.debug('DBRelease setting {0} matches classic tarball file'.format(dbrelease))
                    if not os.access(dbrelease, os.R_OK):
                        msg.warning('Transform was given tarball DBRelease file {0}, but this is not there'.format(dbrelease))
                        msg.warning('I will now try to find DBRelease {0} in cvmfs'.format(dbdMatch.group(1)))
                        dbrelease = dbdMatch.group(1)
                        dbsetup = cvmfsDBReleaseCheck(dbrelease)
                    else:
                        # Check if the DBRelease is setup
                        unpacked, dbsetup = unpackDBRelease(tarball=dbrelease, dbversion=dbdMatch.group(1))
                        if unpacked:
                            # Now run the setup.py script to customise the paths to the current location...
                            setupDBRelease(dbsetup)
                # For cvmfs we want just the X.Y.Z release string (and also support 'current')
                else:
                    dbsetup = cvmfsDBReleaseCheck(dbrelease)
        
        # Look for environment updates and perpare the athena command line
        self._envUpdate = trfEnv.environmentUpdate()
        #above is needed by _prepAthenaCommandLine, but remove the setStandardEnvironment so doesn't include imf or tcmalloc 
        #self._envUpdate.setStandardEnvironment(self.conf.argdict)
        self._prepAthenaCommandLine() 
        
        #to get athenaHLT to read in the relevant parts from the runargs file we have to translate them
        if 'athenaHLT' in self._exe:
            self._cmd.remove('runargs.BSRDOtoRAW.py')
            # get list of translated arguments to be used by athenaHLT
            optionList = getTranslated(self.conf.argdict, name=self._name, substep=self._substep, first=self.conf.firstExecutor, output = outputFiles)
            self._cmd.extend(optionList)

            #Run preRun step debug_stream analysis if debug_stream=True
            if 'debug_stream' in self.conf.argdict:
                inputFiles = dict()
                for dataType in input:
                    inputFiles[dataType] = self.conf.dataDictionary[dataType]
                outputFiles = dict()
                for dataType in output:
                    outputFiles[dataType] = self.conf.dataDictionary[dataType]
                
                #set default file name for debug_stream analysis output
                fileNameDbg = ['debug-stream-monitoring.root']
                if 'HIST_DEBUGSTREAMMON' in output:
                    fileNameDbg = outputFiles['HIST_DEBUGSTREAMMON'].value
                
                #if file exist then rename file to -old.root to keep as backup
                if(os.path.isfile(fileNameDbg[0])):
                    oldOutputFileNameDbg = fileNameDbg[0].replace(".root","_old.root")
                    msg.info('Renaming %s to %s' % (fileNameDbg[0], oldOutputFileNameDbg) )                    
                    os.rename(fileNameDbg[0], oldOutputFileNameDbg)

                #do debug_stream preRun step and get asetup string from debug_stream input files
                dbgAsetupString  = dbgStream.dbgPreRun(inputFiles['BS_RDO'],fileNameDbg)
                # setup asetup from debug_stream if no --asetup r2b:string was given and is not running with tzero/software/patches as TestArea
                if asetupString == None and dbgAsetupString != None : 
                    asetupString = dbgAsetupString
                    msg.info('Will use asetup string for debug_stream analsys %s' % dbgAsetupString)
                    
        #call athenaExecutor parent as the above overrides what athenaExecutor would have done 
        super(athenaExecutor, self).preExecute(input, output)
        
        # Now we always write a wrapper, because it's very convenient for re-running individual substeps
        # This will have asetup and/or DB release setups in it
        # Do this last in this preExecute as the _cmd needs to be finalised
        msg.info('Now writing wrapper for substep executor {0}'.format(self._name))
        self._writeAthenaWrapper(asetup=asetupString, dbsetup=dbsetup)
        msg.info('Athena will be executed in a subshell via {0}'.format(self._cmd))
            
    def _prepAthenaCommandLine(self):
        
        #When running from the DB athenaHLT needs no skeleton file
        msg.info("Before build command line check if reading from DB")
        
        #check if expecting to run from DB
        removeSkeleton = False
        if 'useDB' in self.conf.argdict:
            removeSkeleton=True
        elif 'athenaopts' in self.conf.argdict:
            v = self.conf.argdict['athenaopts'].value
            if '--use-database' in v or '-b' in v:
                removeSkeleton=True
        
        #due to athenaExecutor code don't remove skeleton, otherwise lose runargs too
        #instead remove skeleton from _topOptionsFiles
        if removeSkeleton and self._skeleton is not None:
            msg.info('Read from DB: remove skeleton {0} from command'.format(self._skeleton))
            self._topOptionsFiles.remove(self._skeleton[0])
        else:
            msg.info('Not reading from DB: keep skeleton in command')
        
        #now build command line as in athenaExecutor
        super(trigRecoExecutor, self)._prepAthenaCommandLine()

    #loop over current directory and find the output file matching input pattern
    def _findOutputFiles(self, pattern):
        #list to store the filenames of files matching pattern
        matchedOutputFileNames = []
        #list of input files that could be in the same folder and need ignoring
        ignoreInputFileNames = []
        for dataType, dataArg in self.conf.dataDictionary.items():
            if dataArg.io == 'input':
                ignoreInputFileNames.append(dataArg.value[0])
        #loop over all files in folder to find matching output files
        for file in os.listdir('.'):
            if fnmatch.fnmatch(file, pattern):
                if file in ignoreInputFileNames:
                    msg.info('Ignoring input file: %s' % file)
                else:
                    matchedOutputFileNames.append(file)
        return matchedOutputFileNames

    #run trigbs_extractStream.py to split a stream out of the BS file
    #renames the split file afterwards
    def _splitBSfile(self, outputStream, allStreamsFileName, splitFileName):
        msg.info('Splitting stream %s from BS file' % outputStream)
        splitStreamFailure=0
        try:
            cmd = 'trigbs_extractStream.py -s ' + outputStream + ' ' + allStreamsFileName
            msg.info('running command for splitting (in original asetup env): %s' % cmd)
            splitStreamFailure = subprocess.call(cmd, shell=True)
            msg.debug('trigbs_extractStream.py splitting return code %s' % (splitStreamFailure) )
        except OSError as e:
            raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_OUTPUT_FILE_ERROR'),
                'Exception raised when selecting stream with trigbs_extractStream.py in file {0}: {1}'.format(allStreamsFileName, e))
        if splitStreamFailure != 0:
            msg.error('trigbs_extractStream.py returned error (%s) no split BS file created' % splitStreamFailure)
            return 1
        else:
            #know that the format will be of the form ####._athenaHLT.####.data
            expectedStreamFileName = '*_athenaHLT*.data'
            #list of filenames of files matching expectedStreamFileName
            matchedOutputFileName = self._findOutputFiles(expectedStreamFileName)
            if(len(matchedOutputFileName)):
                self._renamefile(matchedOutputFileName[0], splitFileName)
                return 0
            else:
                msg.error('trigbs_extractStream.py did not created expected file (%s)' % expectedStreamFileName)
                return 1

    #rename a created file - used to overwrite filenames from athenaHLT into the requested argument name
    def _renamefile(self, currentFileName, newFileName):
        msg.info('Renaming file from %s to %s' % (currentFileName, newFileName))
        try:
            os.rename(currentFileName, newFileName)
        except OSError as e:
            msg.error('Exception raised when renaming {0} #to {1}: {2}'.format(currentFileName, newFileName, e))
            raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_OUTPUT_FILE_ERROR'),
                'Exception raised when renaming {0} #to {1}: {2}'.format(currentFileName, newFileName, e))

    def postExecute(self):
                
        #Adding check for HLTMPPU.*Child Issue in the log file
        #   Throws an error message if there so we catch that the child died
        #   Also sets the return code of the mother process to mark the job as failed
        #   Is based on trfValidation.scanLogFile
        log = self._logFileName
        msg.debug('Now scanning logfile {0} for HLTMPPU Child Issues'.format(log))
        # Using the generator so that lines can be grabbed by subroutines if needed for more reporting
        try:
            myGen = lineByLine(log, substepName=self._substep)
        except IOError as e:
            msg.error('Failed to open transform logfile {0}: {1:s}'.format(log, e))
        for line, lineCounter in myGen:
            # Check to see if any of the hlt children had an issue
            if 'Child Issue' in line > -1:
                try:
                    signal = int((re.search('signal ([0-9]*)', line)).group(1))
                except AttributeError:
                    #signal not found in message, so return 1 to highlight failure
                    signal = 1
                msg.error('Detected issue with HLTChild, setting mother return code to %s' % (signal) )
                self._rc = signal

        #Merge child log files into parent log file
        #is needed to make sure all child log files are scanned
        #files are found by searching whole folder rather than relying on nprocs being defined
        try:
            #open original log file (log.BSRDOtoRAW) to merge child files into
            with open(self._logFileName, 'a') as merged_file:
                for file in os.listdir('.'):
                    #expected child log files should be of the format athenaHLT:XX.out and .err
                    if fnmatch.fnmatch(file, 'athenaHLT:*'):
                        msg.info('Merging child log file (%s) into %s' % (file,self._logFileName))
                        with open(file) as log_file:
                            #write header infomation ### Output from athenaHLT:XX.out/err ###
                            merged_file.write('### Output from {} ###\n'.format(file))
                            #write out file line by line
                            for line in log_file:
                                merged_file.write(line)
        except OSError as e:
            raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_OUTPUT_FILE_ERROR'),
                        'Exception raised when merging log files into {0}: {1}'.format(self._logFileName, e))

        msg.info("Check for expert-monitoring.root file")
        #the BS-BS step generates the files:
        #   expert-monitoring.root (from mother process)
        #   athenaHLT_workers/*/expert-monitoring.root (from child processes)        
        #to save on panda it needs to be renamed via the outputHIST_HLTMONFile argument
        expectedFileName = 'expert-monitoring.root'

        #first check athenaHLT step actually completed
        if self._rc != 0:
            msg.info('HLT step failed (with status %s) so skip HIST_HLTMON filename check' % self._rc)
        #next check argument is in dictionary as a requested output
        elif 'outputHIST_HLTMONFile' in self.conf.argdict:

            #rename the mother file
            expectedMotherFileName = 'expert-monitoring-mother.root'
            if(os.path.isfile(expectedFileName)):
                msg.info('Renaming %s to %s' % (expectedFileName, expectedMotherFileName) )
                try:
                    os.rename(expectedFileName, expectedMotherFileName)
                except OSError as e:
                    raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_OUTPUT_FILE_ERROR'),
                        'Exception raised when renaming {0} to {1}: {2}'.format(expectedFileName, expectedMotherFileName, e))
            else:
                msg.error('HLTMON argument defined but mother %s not created' % (expectedFileName ))

            #merge worker files
            expectedWorkerFileName = 'athenaHLT_workers/athenaHLT-01/' + expectedFileName
            if(os.path.isfile(expectedWorkerFileName) and os.path.isfile(expectedMotherFileName)):
                msg.info('Merging worker and mother %s files to %s' % (expectedFileName, self.conf.argdict['outputHIST_HLTMONFile'].value[0]) )
                try:
                    #have checked that at least one worker file exists
                    cmd = 'hadd ' + self.conf.argdict['outputHIST_HLTMONFile'].value[0] + ' athenaHLT_workers/*/expert-monitoring.root expert-monitoring-mother.root'
                    subprocess.call(cmd, shell=True)
                except OSError as e:
                    raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_OUTPUT_FILE_ERROR'),
                        'Exception raised when merging worker and mother {0} files to {1}: {2}'.format(expectedFileName, self.conf.argdict['outputHIST_HLTMONFile'].value[0], e))
            else:
                msg.error('HLTMON argument defined %s but worker %s not created' % (self.conf.argdict['outputHIST_HLTMONFile'].value[0], expectedFileName ))

        else:
            msg.info('HLTMON argument not defined so skip %s check' % expectedFileName)
        
        
        
        msg.info("Search for created BS files, and rename if single file found")
        #The following is needed to handle the BS file being written with a different name (or names)
        #base is from either the tmp value created by the transform or the value entered by the user
        if self._rc != 0:
            msg.error('HLT step failed (with status %s) so skip BS filename check' % self._rc)
        elif 'BS' in self.conf.dataDictionary:
            argInDict = self.conf.dataDictionary['BS']
            #keep dataset in case need to update argument
            dataset_argInDict = argInDict._dataset
            #expected string based on knowing that the format will be of form: ####._HLTMPPy_####.data
            expectedOutputFileName = '*_HLTMPPy_*.data'
            #list of filenames of files matching expectedOutputFileName
            matchedOutputFileNames = self._findOutputFiles(expectedOutputFileName)
            #check there are file matches and rename appropriately
            if(len(matchedOutputFileNames)>1):
                msg.warning('Multiple BS files found: will only rename internal arg')
                msg.info('Renaming internal BS arg from %s to %s' % (argInDict.value[0], matchedOutputFileNames))
                argInDict.multipleOK = True
                argInDict.value = matchedOutputFileNames
                argInDict._dataset = dataset_argInDict
            elif(len(matchedOutputFileNames)):
                msg.info('Single BS file found: will split (if requested) and rename file')

                #TODO (ATR-20974) First check if we want to produce the COST BS output
                #if 'COST' in self.conf.dataDictionary:
                #    splitFailed = self._splitBSfile('Cost', matchedOutputFileNames[0],self.conf.dataDictionary['COST'].value[0])
                #    if(splitFailed):
                #        raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_OUTPUT_FILE_ERROR'),
                #            'Did not produce any BS file when selecting stream with trigbs_extractStream.py in file')

                # If a stream (not All) is selected then slim the output to the particular stream out of the original BS file with many streams
                if 'streamSelection' in self.conf.argdict and self.conf.argdict['streamSelection'].value != "All":
                    splitFailed = self._splitBSfile(self.conf.argdict['streamSelection'].value, matchedOutputFileNames[0], argInDict.value[0])
                    if(splitFailed):
                        raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_OUTPUT_FILE_ERROR'),
                            'Did not produce any BS file when selecting stream with trigbs_extractStream.py in file')
                else:
                    msg.info('Stream "All" requested, so not splitting BS file')
                    self._renamefile(matchedOutputFileNames[0], argInDict.value[0])
            else:
                msg.error('no BS files created with expected name: %s' % expectedOutputFileName )
        else:
            msg.info('BS output filetype not defined so skip BS filename check')

        #Run PostRun step debug_stream analysis if debug_stream=True
        if 'debug_stream' in self.conf.argdict:
            msg.info("debug_stream analysis in postExecute")
    
            #set default file name for debug_stream analysis output
            fileNameDbg = ['debug-stream-monitoring.root']
            if "outputHIST_DEBUGSTREAMMONFile" in self.conf.argdict:
                fileNameDbg= self.conf.argdict["outputHIST_DEBUGSTREAMMONFile"].value                
                msg.info('outputHIST_DEBUGSTREAMMONFile argument is {0}'.format(fileNameDbg) )

            #TODO add merging of mother and child debug files

            if(os.path.isfile(fileNameDbg[0])):
                #keep filename if not defined
                msg.info('Will use file created  in PreRun step {0}'.format(fileNameDbg) )
            else :
                msg.info('No file created  in PreRun step {0}'.format(fileNameDbg) )

            #do debug_stream postRun step
            dbgStream.dbgPostRun(argInDict,fileNameDbg)
            #
            # TODO is the reset now redundant? (not needed for reprocessing)
            #
            #now reset metadata for outputBSFile needed for trf file validation
            self.conf.dataDictionary['BS']._resetMetadata()

        msg.info('Now run athenaExecutor:postExecute')
        super(trigRecoExecutor, self).postExecute()
