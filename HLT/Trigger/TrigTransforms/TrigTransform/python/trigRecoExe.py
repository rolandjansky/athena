#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @brief: Trigger executor to call base transforms
# @details: Based on athenaExecutor with some modifications
# @author: Mark Stockton

import logging
msg = logging.getLogger("PyJobTransforms." + __name__)

import os
import fnmatch
import re

from PyJobTransforms.trfExe import athenaExecutor

#imports for preExecute
from PyJobTransforms.trfUtils import asetupReport, cvmfsDBReleaseCheck
import PyJobTransforms.trfEnv as trfEnv
import PyJobTransforms.trfExceptions as trfExceptions
from PyJobTransforms.trfExitCodes import trfExit as trfExit
import TrigTransform.dbgAnalysis as dbgStream
from TrigTransform.trigTranslate import writeTranslate as writeTranslate


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
                if not isinstance(inputEvents, (int, long)):
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
            for dataType, dataArg in self.conf.dataDictionary.iteritems():
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
        
        #to get athenaHLT to read in the relevant parts from the runargs file we have to add the -F option
        if 'athenaHLT' in self._exe:
            self._cmd=['-F runtranslate.BSRDOtoRAW.py' if x=='runargs.BSRDOtoRAW.py' else x for x in self._cmd]
            
            # write runTranslate file to be used by athenaHLT
            writeTranslate('runtranslate.BSRDOtoRAW.py', self.conf.argdict, name=self._name, substep=self._substep, first=self.conf.firstExecutor, output = outputFiles)
            
            #instead of running athenaHLT we can dump the options it has loaded
            #note the -D needs to go after the -F in the command
            if 'dumpOptions' in self.conf.argdict:
                self._cmd=['-F runtranslate.BSRDOtoRAW.py -D' if x=='-F runtranslate.BSRDOtoRAW.py' else x for x in self._cmd]
            
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
            
    def postExecute(self):
                
        msg.info("Check for expert-monitoring.root file")
        #the BS-BS step generates the file expert-monitoring.root
        #to save on panda it needs to be renamed via the outputHIST_HLTMONFile argument
        expectedFileName = 'expert-monitoring.root'
        #first check argument is in dict
        if 'outputHIST_HLTMONFile' in self.conf.argdict:
             #check file is created
             if(os.path.isfile(expectedFileName)):
                 msg.info('Renaming %s to %s' % (expectedFileName, self.conf.argdict['outputHIST_HLTMONFile'].value[0]) ) 
                 try:
                      os.rename(expectedFileName, self.conf.argdict['outputHIST_HLTMONFile'].value[0])
                 except OSError, e:
                      raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_OUTPUT_FILE_ERROR'),
                                    'Exception raised when renaming {0} to {1}: {2}'.format(expectedFileName, self.conf.argdict['outputHIST_HLTMONFile'].value[0], e))
             else:
                 msg.error('HLTMON argument defined %s but %s not created' % (self.conf.argdict['outputHIST_HLTMONFile'].value[0], expectedFileName ))
        else:
            msg.info('HLTMON argument not defined so skip %s check' % expectedFileName)
        
        
        
        msg.info("Search for created BS files, and rename if single file found")
        #The following is needed to handle the BS file being written with a different name (or names)
        #base is from either the tmp value created by the transform or the value entered by the user
        if 'BS' in self.conf.dataDictionary:
            argInDict = self.conf.dataDictionary['BS']
            #create expected string by taking only some of input
            # removes uncertainty of which parts of the filename are used by athenaHLT
            split_argInDict = argInDict.value[0].split('.')
            #drop most of the input string to remove any potential ._####.data 
            # already in argInDict - so just take first element from input
            expectedOutputFileName = split_argInDict[0]
            #expect file from athenaHLT to end _###.data so add ending
            #TODO  a better ending check could be *_????.data
            expectedOutputFileName+='*.data'
            #keep dataset in case need to update argument
            dataset_argInDict = argInDict._dataset
            #list to store the filenames of files matching expectedOutputFileName
            matchedOutputFileNames = []
            #list of input files that could be in the same folder and need ignoring
            ignoreInputFileNames = []
            for dataType, dataArg in self.conf.dataDictionary.iteritems():
                if dataArg.io == 'input':
                    ignoreInputFileNames.append(dataArg.value[0])
            #loop over all files in folder to find matching output files
            for file in os.listdir('.'):
                if fnmatch.fnmatch(file, expectedOutputFileName):
                    if file in ignoreInputFileNames:
                        msg.info('Ignoring input file: %s' % file)
                    else: 
                        matchedOutputFileNames.append(file)
            #check there are file matches and rename appropriately
            if(len(matchedOutputFileNames)>1):
                msg.warning('Multiple BS files found: will only rename internal arg')
                msg.info('Renaming internal BS arg from %s to %s' % (argInDict.value[0], matchedOutputFileNames))
                argInDict.multipleOK = True
                argInDict.value = matchedOutputFileNames
                argInDict._dataset = dataset_argInDict
            elif(len(matchedOutputFileNames)):
                msg.info('Single BS file found: will rename file')
                msg.info('Renaming BS file from %s to %s' % (matchedOutputFileNames[0], argInDict.value[0]))
                try:
                    os.rename(matchedOutputFileNames[0], argInDict.value[0])
                except OSError, e:
                    msg.error('Exception raised when renaming {0} #to {1}: {2}'.format(expectedInput, inputFile, e))
                    raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_OUTPUT_FILE_ERROR'),
                              'Exception raised when renaming {0} #to {1}: {2}'.format(expectedInput, inputFile, e))
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
