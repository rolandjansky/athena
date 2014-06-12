#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @brief: Trigger executor to call base transforms
# @details: Based on athenaExecutor with some modifications
# @author: Mark Stockton

import logging
msg = logging.getLogger("PyJobTransforms." + __name__)

import os
import fnmatch

from PyJobTransforms.trfExe import athenaExecutor

#imports for preExecute
from PyJobTransforms.trfUtils import asetupReport
import PyJobTransforms.trfEnv as trfEnv
     
class trigRecoExecutor(athenaExecutor):
    # Trig_reco_tf.py executor
    # used to change the extra output filenames
    
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
                
            msg.debug('Input Files: {0}; Output Files: {1}'.format(inputFiles, outputFiles))
            
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
        self._cmd=['-F runargs.BSRDOtoRAW.py' if x=='runargs.BSRDOtoRAW.py' else x for x in self._cmd]
                
        #call athenaExecutor parent as the above overrides what athenaExecutor would have done 
        super(athenaExecutor, self).preExecute(input, output)
        
        # Now we always write a wrapper, because it's very convenient for re-running individual substeps
        # This will have asetup and/or DB release setups in it
        # Do this last in this preExecute as the _cmd needs to be finalised
        msg.info('Now writing wrapper for substep executor {0}'.format(self._name))
        self._writeAthenaWrapper(asetup=asetupString, dbsetup=dbsetup)
        msg.info('Athena will be executed in a subshell via {0}'.format(self._cmd))
            
    def postExecute(self):
                
        #runHLT_standalone.py generates the file expert-monitoring.root
        #to save on panda it is defined as output via the outputHIST_HLTMONFile argument        
        
        #TODO hard-coded default filename!
        fileName = 'expert-monitoring.root'
        #check file is created
        if(os.path.isfile(fileName)):
            #keep filename if not defined
            newOutputFileName = 'expert-monitoring.root'   
            #find transform argument value for name     
            for arg in self.conf.argdict:
                if arg == "outputHIST_HLTMONFile":
                    newOutputFileName = self.conf.argdict[arg].value[0]
            #rename file
            msg.info('Renaming %s to %s' % (fileName, newOutputFileName) )        
            os.rename(fileName, newOutputFileName)
        else:
            for arg in self.conf.argdict:
                if arg == "outputHIST_HLTMONFile":
                    msg.error('HLTMON argument defined but %s not created' % fileName )
                    
        #The following is needed to handle the BS file being written with a different name (or names)
        #base is from either the tmp value created by the transform or the value entered by the user
        originalFileArg = self.conf.dataDictionary['BS']
        fileNameBase = originalFileArg.value[0] + '*'
        fileNameMatches = []
        #loop over all files in folder to find matching outputs
        for file in os.listdir('.'):
            if fnmatch.fnmatch(file, fileNameBase):
                fileNameMatches.append(file)
        #check there are file matches
        if(len(fileNameMatches)):
            originalFileArg.multipleOK = True
            originalFileArg.value = fileNameMatches
            msg.info('Renaming internal BS arg from %s to %s' % (originalFileArg.value[0], fileNameMatches))
        else:
            msg.error('no BS files created with expected name' % fileNameMatches )
        

        msg.info('Now run athenaExecutor:postExecute')
        super(trigRecoExecutor, self).postExecute()
        
    