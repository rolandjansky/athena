from __future__ import print_function
from future.utils import iteritems
from builtins import object
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @package PyJobTransforms.transform
#
# @brief Main package for new style ATLAS job transforms
# @details Core class for ATLAS job transforms
# @author atlas-comp-transforms-dev@cern.ch
# @version $Id: transform.py 785618 2016-11-21 22:03:04Z uworlika $
# 

__version__ = '$Revision'

import argparse
import os
import os.path as path
import re
import sys
import traceback

import logging
msg = logging.getLogger(__name__)

import PyJobTransforms.trfValidation as trfValidation
import PyJobTransforms.trfExceptions as trfExceptions

from PyJobTransforms.trfSignal import setTrfSignalHandlers, resetTrfSignalHandlers
from PyJobTransforms.trfArgs import addStandardTrfArgs, addFileValidationArguments, addValidationArguments
from PyJobTransforms.trfLogger import setRootLoggerLevel, stdLogLevels
from PyJobTransforms.trfArgClasses import trfArgParser, argFile, argHISTFile, argument
from PyJobTransforms.trfExitCodes import trfExit
from PyJobTransforms.trfUtils import shQuoteStrings, infanticide, pickledDump, JSONDump, cliToKey, convertToStr
from PyJobTransforms.trfUtils import isInteractiveEnv, calcCpuTime, calcWallTime
from PyJobTransforms.trfReports import trfJobReport, defaultFileReport
from PyJobTransforms.trfExe import transformExecutor
from PyJobTransforms.trfGraph import executorGraph

## @class transform
#  @brief Core transform class
#  @note Every transform should only have one transform class instantiated
class transform(object):
    
    ## @brief Initialise a job transform
    #  @param standardSignalHandlers Boolean to set signal handlers. Default @c True.
    #  @param standardValidationArgs Boolean to set standard validation options. Default @c True.
    #  @param trfName Name of the transform. Default is executable name with .py rstripped.
    #  @param executor Executor list
    def __init__(self, standardSignalHandlers = True, standardTrfArgs = True, standardValidationArgs=True, 
                 trfName = path.basename(sys.argv[0]).rsplit('.py', 1)[0], 
                 executor = set([transformExecutor(),]), exeArgs = None, description = ''):
        '''Transform class initialiser'''
        msg.debug('Welcome to ATLAS job transforms')
        
        ## @brief Get transform starting timestamp as early as possible
        self._transformStart = os.times()
        msg.debug('transformStart time is {0}'.format(self._transformStart))

        self._inFileValidationStart = None
        self._inFileValidationStop = None
        self._outFileValidationStart = None
        self._outFileValidationStop = None

        ## @brief Get trf pre-data as early as possible
        self._trfPredata = os.environ.get('TRF_PREDATA')

        ## Transform _name
        self._name = trfName        
        
        ## @note Holder for arguments this trf understands
        #  Use @c argparse.SUPPRESS to have non-given arguments unset, rather than None
        #  Support reading arguments from a file using the notation @c @file 
        self.parser = trfArgParser(description='Transform {0}. {1}'.format(trfName, description),
                                   argument_default=argparse.SUPPRESS,
                                   fromfile_prefix_chars='@')

        if standardTrfArgs:
            addStandardTrfArgs(self.parser)

        if standardValidationArgs:
            addValidationArguments(self.parser)
            addFileValidationArguments(self.parser)

        
        ## Argument dictionary for this transform
        self._argdict = dict()
        
        ## Dsta dictionary place holder (this maps data types to their argFile instances)
        self._dataDictionary = dict()
        
        
        # Transform executor list - initalise with an empty set
        self._executors = set()
        self._executorDictionary = {}
        
        # If we were passed executors at construction time then append them to the set:
        if executor is not None:
            self.appendToExecutorSet(executor)
        
        ## Transform exit code/message holders
        self._exitCode = None
        self._exitMsg = None

        ## Report object for this transform
        self._report = trfJobReport(parentTrf = self)
        
        ## Transform processed events
        self._processedEvents = None
        
        # Setup standard signal handling if asked
        if standardSignalHandlers:
            setTrfSignalHandlers(self._exitWithReport)
            msg.debug('Standard signal handlers established')

            
    @property
    def name(self):
        return self._name
            
    @property
    def exitCode(self):
        if self._exitCode is None:
            msg.warning('Transform exit code getter: _exitCode is unset, returning "TRF_UNKNOWN"')
            return trfExit.nameToCode('TRF_UNKNOWN')
        else:
            return self._exitCode
        
    @property
    def exitMsg(self):
        if self._exitMsg is None:
            msg.warning('Transform exit message getter: _exitMsg is unset, returning empty string')
            return ''
        else:
            return self._exitMsg
    
    @property
    def argdict(self):
        return self._argdict
    
    @property
    def dataDictionary(self):
        return self._dataDictionary
    
    @property
    def report(self):
        return self._report
    
    @property
    def transformStart(self):
        return self._transformStart

    @property
    def transformSetupCpuTime(self):
        transformSetupCpuTime = None
        if self._transformStart and self._inFileValidationStart:
            transformSetupCpuTime = calcCpuTime(self._transformStart, self._inFileValidationStart)

        return transformSetupCpuTime

    @property
    def transformSetupWallTime(self):
        transformSetupWallTime = None
        if self._transformStart and self._inFileValidationStart:
            transformSetupWallTime = calcWallTime(self._transformStart, self._inFileValidationStart)

        return transformSetupWallTime

    @property
    def inFileValidationCpuTime(self):
        inFileValidationCpuTime = None
        if self._inFileValidationStart and self._inFileValidationStop:
            inFileValidationCpuTime = calcCpuTime(self._inFileValidationStart, self._inFileValidationStop)

        return inFileValidationCpuTime

    @property
    def inFileValidationWallTime(self):
        inFileValidationWallTime = None
        if self._inFileValidationStart and self._inFileValidationStop:
            inFileValidationWallTime = calcWallTime(self._inFileValidationStart, self._inFileValidationStop)

        return inFileValidationWallTime

    @property
    def outFileValidationCpuTime(self):
        outFileValidationCpuTime = None
        if self._outFileValidationStart and self._outFileValidationStop:
            outFileValidationCpuTime = calcCpuTime(self._outFileValidationStart, self._outFileValidationStop)

        return outFileValidationCpuTime

    @property
    def outFileValidationWallTime(self):
        outFileValidationWallTime = None
        if self._outFileValidationStart and self._outFileValidationStop:
            outFileValidationWallTime = calcWallTime(self._outFileValidationStart, self._outFileValidationStop)

        return outFileValidationWallTime

    @property
    def outFileValidationStop(self):
        return self._outFileValidationStop

    @property
    def trfPredata(self):
        return self._trfPredata
    
    @property
    def executors(self):
        return self._executors
    
    @property
    def processedEvents(self):
        return self._processedEvents
    
    def getProcessedEvents(self):
        nEvts = None
        for executionStep in self._executorPath:
            executor = self._executorDictionary[executionStep['name']]
            if executor.conf.firstExecutor:
                nEvts = executor.eventCount
        return nEvts

    def appendToExecutorSet(self, executors):
        # Normalise to something iterable
        if isinstance(executors, transformExecutor):
            executors = [executors,]
        elif not isinstance(executors, (list, tuple, set)):
            raise trfExceptions.TransformInternalException(trfExit.nameToCode('TRF_INTERNAL'), 
                                                           'Transform was initialised with an executor which was not a simple executor or an executor set')
    
        # TRY TO DEPRECATE SETTING trf IN THE EXECUTOR - USE CONF!
        # Executor book keeping: set parent link back to me for all executors
        # Also setup a dictionary, indexed by executor name and check that name is unique
        ## Setting conf here not working - too early to get the dataDictionary
        for executor in executors:
            executor.trf = self
            if executor.name in self._executorDictionary:
                raise trfExceptions.TransformInternalException(trfExit.nameToCode('TRF_INTERNAL'), 
                                                               'Transform has been initialised with two executors with the same name ({0})'
                                                               ' - executor names must be unique'.format(executor.name))
            self._executors.add(executor)
            self._executorDictionary[executor.name] = executor


    ## @brief Parse command line arguments for a transform
    def parseCmdLineArgs(self, args):
        msg.info('Transform command line was: %s', ' '.join(shQuoteStrings(sys.argv)))

        try:
            # Use the argparse infrastructure to get the actual command line arguments
            self._argdict=vars(self.parser.parse_args(args))

            # Need to know if any input or output files were set - if so then we suppress the
            # corresponding parameters from AMI
            inputFiles = outputFiles = False
            for k, v in iteritems(self._argdict):
                if k.startswith('input') and isinstance(v, argFile):
                    inputFiles = True
                elif k.startswith('output') and isinstance(v, argFile):
                    outputFiles = True
            msg.debug("CLI Input files: {0}; Output files {1}".format(inputFiles, outputFiles))

            # Now look for special arguments, which expand out to other parameters
            # Note that the pickled argdict beats AMIConfig because dict.update() will overwrite
            # (However, we defend the real command line against updates from either source)
            extraParameters = {}
            # AMI configuration?
            if 'AMIConfig' in self._argdict:
                msg.debug('Given AMI tag configuration {0}'.format(self._argdict['AMIConfig']))
                from PyJobTransforms.trfAMI import TagInfo
                tag=TagInfo(self._argdict['AMIConfig'].value)
                updateDict = {}
                for k, v in iteritems(dict(tag.trfs[0])):
                    # Convert to correct internal key form
                    k = cliToKey(k)
                    if inputFiles and k.startswith('input'):
                        msg.debug('Suppressing argument {0} from AMI'
                                  ' because input files have been specified on the command line'.format(k))
                        continue
                    if outputFiles and k.startswith('output'):
                        msg.debug('Suppressing argument {0} from AMI'
                                  ' because output files have been specified on the command line'.format(k))
                        continue
                    updateDict[k] = v
                extraParameters.update(updateDict)

            # JSON arguments?
            if 'argJSON' in self._argdict:
                try:
                    import json
                    msg.debug('Given JSON encoded arguments in {0}'.format(self._argdict['argJSON']))
                    argfile = open(self._argdict['argJSON'], 'r')
                    jsonParams = json.load(argfile)
                    msg.debug('Read: {0}'.format(jsonParams))
                    extraParameters.update(convertToStr(jsonParams))
                    argfile.close()
                except Exception as e:
                    raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_ERROR'), 'Error when deserialising JSON file {0} ({1})'.format(self._argdict['argJSON'], e))
            
            # Event Service
            if 'eventService' in self._argdict and self._argdict['eventService'].value:
                updateDict = {}
                updateDict['athenaMPMergeTargetSize'] = '*:0'
                updateDict['checkEventCount'] = False
                updateDict['outputFileValidation'] = False
                extraParameters.update(updateDict)
                
            # Process anything we found
            for k,v in iteritems(extraParameters):
                msg.debug('Found this extra argument: {0} with value: {1} ({2})'.format(k, v, type(v)))
                if k not in self.parser._argClass:
                    raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_ERROR'), 'Argument "{0}" not known (try "--help")'.format(k))
                if k in self._argdict:
                    msg.debug('Ignored {0}={1} as extra parameter because this argument was given on the command line.'.format(k, v))
                    continue
                # For callable classes we instantiate properly, otherwise we set the value for simple arguments
                if '__call__' in dir(self.parser._argClass[k]):
                    self._argdict[k] = self.parser._argClass[k](v)
                else:
                    self._argdict[k] = v
                msg.debug('Argument {0} set to {1}'.format(k, self._argdict[k]))

            # Set the key name as an argument property - useful to be able to look bask at where this
            # argument came from
            for k, v in iteritems(self._argdict):
                if isinstance(v, argument):
                    v.name = k
                    
            # Now we parsed all arguments, if a pickle/json dump is requested do it here and exit
            if 'dumpPickle' in self._argdict:
                msg.info('Now dumping pickled version of command line to {0}'.format(self._argdict['dumpPickle']))
                pickledDump(self._argdict)
                sys.exit(0)

            # Now we parsed all arguments, if a pickle/json dump is requested do it here and exit
            if 'dumpJSON' in self._argdict:
                msg.info('Now dumping JSON version of command line to {0}'.format(self._argdict['dumpJSON']))
                JSONDump(self._argdict)
                sys.exit(0)

        except trfExceptions.TransformArgException as e:
            msg.critical('Argument parsing failure: {0!s}'.format(e))
            self._exitCode = e.errCode
            self._exitMsg = e.errMsg
            self._report.fast = True
            self.generateReport()
            sys.exit(self._exitCode)
            
        except trfExceptions.TransformAMIException as e:
            msg.critical('AMI failure: {0!s}'.format(e))
            self._exitCode = e.errCode
            self._exitMsg = e.errMsg
            sys.exit(self._exitCode)

        self.setGlobalLogLevel()
        

    ## @brief Check transform argument dictionary and set the correct root logger option
    def setGlobalLogLevel(self):
        if 'verbose' in self._argdict:
            setRootLoggerLevel(stdLogLevels['DEBUG'])
        elif 'loglevel' in self._argdict:
            if self._argdict['loglevel'] in stdLogLevels:
                msg.info("Loglevel option found - setting root logger level to %s", 
                         logging.getLevelName(stdLogLevels[self._argdict['loglevel']]))
                setRootLoggerLevel(stdLogLevels[self._argdict['loglevel']])
            else:
                msg.warning('Unrecognised loglevel ({0}) given - ignored'.format(self._argdict['loglevel']))

        
    ## @brief Execute transform
    # @details This function calls the actual transform execution class and
    # sets \c self.exitCode,  \c self.exitMsg and \c self.processedEvents transform data members.
    # @return None.
    def execute(self):
        msg.debug('Entering transform execution phase')
  
        try:
            # Intercept a few special options here      
            if 'dumpargs' in self._argdict:
                self.parser.dumpArgs()
                sys.exit(0)
                
            # Graph stuff!
            msg.info('Resolving execution graph')
            self._setupGraph()
            
            if 'showSteps' in self._argdict:
                for exe in self._executors:
                    print("Executor Step: {0} (alias {1})".format(exe.name, exe.substep))
                    if msg.level <= logging.DEBUG:
                        print(" {0} -> {1}".format(exe.inData, exe.outData))
                sys.exit(0)
                        
            if 'showGraph' in self._argdict:
                print(self._executorGraph)
                sys.exit(0)
                
            # Graph stuff!
            msg.info('Starting to trace execution path')            
            self._tracePath()
            msg.info('Execution path found with {0} step(s): {1}'.format(len(self._executorPath),
                                                                         ' '.join([exe['name'] for exe in self._executorPath])))

            if 'showPath' in self._argdict:
                msg.debug('Execution path list is: {0}'.format(self._executorPath))
                # Now print it nice
                print('Executor path is:')
                for node in self._executorPath:
                    print('  {0}: {1} -> {2}'.format(node['name'], list(node['input']), list(node['output'])))
                sys.exit(0)

            msg.debug('Execution path is {0}'.format(self._executorPath))
                
            # Prepare files for execution (separate method?)
            for dataType in [ data for data in self._executorGraph.data if 'NULL' not in data ]:
                if dataType in self._dataDictionary:
                    msg.debug('Data type {0} maps to existing argument {1}'.format(dataType, self._dataDictionary[dataType]))
                else:
                    fileName = 'tmp.' + dataType
                    # How to pick the correct argFile class?
                    for (prefix, suffix) in (('tmp', ''), ('output', 'File'), ('input', 'File')):
                        stdArgName = prefix + dataType + suffix
                        if stdArgName in self.parser._argClass:
                            msg.debug('Matched data type {0} to argument {1}'.format(dataType, stdArgName))
                            self._dataDictionary[dataType] = self.parser._argClass[stdArgName](fileName)
                            self._dataDictionary[dataType].io = 'temporary'
                            break
                    if dataType not in self._dataDictionary:
                        if 'HIST' in fileName:
                            self._dataDictionary[dataType] = argHISTFile(fileName, io='temporary', type=dataType.lower())
                            
                        else:
                            self._dataDictionary[dataType] = argFile(fileName, io='temporary', type=dataType.lower())
                            msg.debug('Did not find any argument matching data type {0} - setting to plain argFile: {1}'.format(dataType, self._dataDictionary[dataType]))
                    self._dataDictionary[dataType].name = fileName

            # Now we can set the final executor configuration properly, with the final dataDictionary
            for executor in self._executors:
                executor.conf.setFromTransform(self)

            self.validateInFiles()
            
            for executionStep in self._executorPath:
                msg.debug('Now preparing to execute {0}'.format(executionStep))
                executor = self._executorDictionary[executionStep['name']]
                executor.preExecute(input = executionStep['input'], output = executionStep['output'])
                try:
                    executor.execute()
                    executor.postExecute()
                finally:
                    executor.validate()
             
            self._processedEvents = self.getProcessedEvents()
            self.validateOutFiles()
            
            msg.debug('Transform executor succeeded')
            self._exitCode = 0
            self._exitMsg = trfExit.codeToName(self._exitCode)
            
        except trfExceptions.TransformNeedCheckException as e:
            msg.warning('Transform executor signaled NEEDCHECK condition: {0}'.format(e.errMsg))
            self._exitCode = e.errCode
            self._exitMsg = e.errMsg
            self.generateReport(fast=False)

        except trfExceptions.TransformException as e:
            msg.critical('Transform executor raised %s: %s' % (e.__class__.__name__, e.errMsg))
            self._exitCode = e.errCode
            self._exitMsg = e.errMsg
            # Try and write a job report...
            self.generateReport(fast=True)

        finally:
            # Clean up any orphaned processes and exit here if things went bad
            infanticide(message=True)
            if self._exitCode:
                msg.warning('Transform now exiting early with exit code {0} ({1})'.format(self._exitCode, self._exitMsg))
                sys.exit(self._exitCode)
        
    ## @brief Setup the executor graph
    #  @note This function might need to be called again when the number of 'substeps' is unknown
    #  just based on the input data types - e.g., DigiMReco jobs don't know how many RDOtoESD
    #  steps they need to run until after digitisation.
    def _setupGraph(self):        
        # Get input/output data
        self._inputData = list()
        self._outputData = list()
        
        for key, value in iteritems(self._argdict):
            # Note specifier [A-Za-z0-9_]+? makes this match non-greedy (avoid swallowing the optional 'File' suffix)
            m = re.match(r'(input|output|tmp)([A-Za-z0-9_]+?)(File)?$', key)
            # N.B. Protect against taking argunents which are not type argFile
            if m and isinstance(value, argFile):
                if m.group(1) == 'input':
                    self._inputData.append(m.group(2))
                else:
                    self._outputData.append(m.group(2))
                self._dataDictionary[m.group(2)] = value
                
        ## @note If we have no real data then add the pseudo datatype NULL, which allows us to manage
        #  transforms which can run without data
        if len(self._inputData) == 0:
            self._inputData.append('inNULL')
        if len(self._outputData) == 0:
            self._outputData.append('outNULL')            
        msg.debug('Transform has this input data: {0}; output data {1}'.format(self._inputData, self._outputData))
        
        # Now see if we have any steering - manipulate the substep inputs and outputs before we
        # setup the graph
        if 'steering' in self._argdict:
            msg.debug('Now applying steering to graph: {0}'.format(self._argdict['steering'].value))
            self._doSteering()
        
        # Setup the graph and topo sort it
        self._executorGraph = executorGraph(self._executors, self._inputData, self._outputData)
        self._executorGraph.doToposort()
    
    
    ## @brief Trace the path through the executor graph
    #  @note This function might need to be called again when the number of 'substeps' is unknown
    #  just based on the input data types - e.g., DigiMReco jobs don't know how many RDOtoESD
    #  steps they need to run until after digitisation.
    def _tracePath(self):
        self._executorGraph.findExecutionPath()
        
        self._executorPath = self._executorGraph.execution
        if len(self._executorPath) == 0:
            raise trfExceptions.TransformSetupException(trfExit.nameToCode('TRF_SETUP'), 
                                                        'Execution path finding resulted in no substeps being executed'
                                                        '(Did you correctly specify input data for this transform?)')
        # Tell the first executor that they are the first
        self._executorDictionary[self._executorPath[0]['name']].conf.firstExecutor = True

    ## @brief Setup steering, which manipulates the graph before we trace the path
    #  for this transform
    #  @param steeringDict Manual steering dictionary (if specified, used instead of the 
    #  steering from the @c steering argument - pay attention to the input structure!
    def _doSteering(self, steeringDict = None):
        if not steeringDict:
            steeringDict = self._argdict['steering'].value
        for substep, steeringValues in iteritems(steeringDict):
            foundSubstep = False
            for executor in self._executors:
                if executor.name == substep or executor.substep == substep:
                    foundSubstep = True
                    msg.debug('Updating {0} with {1}'.format(executor.name, steeringValues))
                    # Steering consists of tuples with (in/out, +/-, datatype) 
                    for steeringValue in steeringValues:
                        if steeringValue[0] == 'in':
                            startSet = executor.inData
                        else:
                            startSet = executor.outData
                        origLen = len(startSet)
                        msg.debug('Data values to be modified are: {0}'.format(startSet))
                        if steeringValue[1] == '+':
                            startSet.add(steeringValue[2])
                            if len(startSet) != origLen + 1:
                                raise trfExceptions.TransformSetupException(trfExit.nameToCode('TRF_GRAPH_STEERING_ERROR'),
                                                                            'Attempting to add data type {0} from {1} {2} fails (original set of data: {3}). Was this datatype already there?'.format(steeringValue[2], executor.name, steeringValue[1], startSet))
                        else:
                            startSet.discard(steeringValue[2])
                            if len(startSet) != origLen - 1:
                                raise trfExceptions.TransformSetupException(trfExit.nameToCode('TRF_GRAPH_STEERING_ERROR'),
                                                                            'Attempting to remove data type {0} from {1} {2} fails (original set of data: {3}). Was this datatype even present?'.format(steeringValue[2], executor.name, steeringValue[1], startSet))
                    msg.debug('Updated data values to: {0}'.format(startSet))
            if not foundSubstep:
                raise trfExceptions.TransformSetupException(trfExit.nameToCode('TRF_GRAPH_STEERING_ERROR'),
                                                            'This transform has no executor/substep {0}'.format(substep))                                
        

    ## @brief Return the last executor which actually executed
    #  @return Last executor which has @c _hasExecuted == @c True, or the very first executor if we didn't even start yet
    @property
    def lastExecuted(self):
        # Just make sure we have the path traced
        if not hasattr(self, '_executorPath') or len(self._executorPath) == 0:
            return None
            
        lastExecutor = self._executorDictionary[self._executorPath[0]['name']]
        for executorStep in self._executorPath[1:]:
            if self._executorDictionary[executorStep['name']].hasExecuted:
                lastExecutor = self._executorDictionary[executorStep['name']]
        return lastExecutor


    ## @brief Transform report generator
    #  @param fast If True ensure that no external calls are made for file metadata (this is 
    #  used to generate reports in a hurry after a crash or a forced exit)
    #  @param fileReport Dictionary giving the type of report to make for each type of file.
    #  This dictionary has to have all io types as keys and valid values are:
    #  @c None - skip this io type; @c 'full' - Provide all details; @c 'name' - only dataset and
    #  filename will be reported on. 
    #  @param reportType Iterable with report types to generate, otherwise a sensible default
    #  is used (~everything, plus the Tier0 report at Tier0)
    def generateReport(self, reportType=None, fast=False, fileReport = defaultFileReport):
        msg.debug('Transform report generator')

        if 'reportType' in self._argdict:
            if reportType is not None:
                msg.info('Transform requested report types {0} overridden by command line to {1}'.format(reportType, self._argdict['reportType'].value))
            reportType = self._argdict['reportType'].value

        if reportType is None:
            reportType = ['json', ]
            # Only generate the Tier0 report at Tier0 ;-)
            # (It causes spurious warnings for some grid jobs with background files (e.g., digitisation)
            if 'TZHOME' in os.environ:
                reportType.append('gpickle')

            if not isInteractiveEnv():
                reportType.append('text')
                msg.debug('Detected Non-Interactive environment. Enabled text report')

        if 'reportName' in self._argdict:
            baseName = classicName = self._argdict['reportName'].value
        else:
            baseName = 'jobReport'
            classicName = 'metadata'

        try:
            # Text: Writes environment variables and machine report in text format.
            if reportType is None or 'text' in reportType:
                envName = baseName if 'reportName' in self._argdict else 'env'  # Use fallback name 'env.txt' if it's not specified.
                self._report.writeTxtReport(filename='{0}.txt'.format(envName), fast=fast, fileReport=fileReport)
            # JSON
            if reportType is None or 'json' in reportType:
                self._report.writeJSONReport(filename='{0}.json'.format(baseName), fast=fast, fileReport=fileReport)
            # Classic XML
            if reportType is None or 'classic' in reportType:
                self._report.writeClassicXMLReport(filename='{0}.xml'.format(classicName), fast=fast)
            # Classic gPickle
            if reportType is None or 'gpickle' in reportType:
                self._report.writeGPickleReport(filename='{0}.gpickle'.format(baseName), fast=fast)
            # Pickled version of the JSON report for pilot
            if reportType is None or 'pilotPickle' in reportType:
                self._report.writePilotPickleReport(filename='{0}Extract.pickle'.format(baseName), fast=fast, fileReport=fileReport)

        except trfExceptions.TransformTimeoutException as reportException:
            msg.error('Received timeout when writing report ({0})'.format(reportException))
            msg.error('Report writing is aborted - sorry. Transform will exit with TRF_METADATA_CALL_FAIL status.')
            if ('orphanKiller' in self._argdict):
                infanticide(message=True, listOrphans=True)
            else:
                infanticide(message=True)
            sys.exit(trfExit.nameToCode('TRF_METADATA_CALL_FAIL'))

        except trfExceptions.TransformException as reportException:
            # This is a bad one!
            msg.critical('Attempt to write job report failed with exception {0!s}: {1!s}'.format(reportException.__class__.__name__, reportException))
            msg.critical('Stack trace now follows:\n{0}'.format(traceback.format_exc()))
            msg.critical('Job reports are likely to be missing or incomplete - sorry')
            msg.critical('Please report this as a transforms bug!')
            msg.critical('Before calling the report generator the transform status was: {0}; exit code {1}'.format(self._exitMsg, self._exitCode))
            msg.critical('Now exiting with a transform internal error code')
            if ('orphanKiller' in self._argdict):
                infanticide(message=True, listOrphans=True)
            else:
                infanticide(message=True)
            sys.exit(trfExit.nameToCode('TRF_INTERNAL'))
    

    # Description stolen from old trfs...
    ## @brief Common signal handler.
    #  @details This function is installed in place of the default signal handler and attempts to terminate the 
    #  transform gracefully. When a signal is caught by the transform, the stdout from the running application process 
    #  (i.e. @c athena.py) is allowed to continue uninterrupted and write it's stdout to the log file (to retrieve 
    #  the traceback) before the associated job report records the fact that a signal has been caught and complete 
    #  the report accordingly. 
    #  @param signum Signal number. Not used since this is a common handle assigned to predefined signals using the 
    #  @c _installSignalHandlers(). This param is still required to satisfy the requirements of @c signal.signal().
    #  @param frame Not used. Provided here to satisfy the requirements of @c signal.signal().
    #  @return Does not return. Raises SystemExit exception.
    #  @exception SystemExit()
    def _exitWithReport(self, signum, frame):
        msg.critical('Transform received signal {0}'.format(signum))
        msg.critical('Stack trace now follows:\n{0!s}'.format(''.join(traceback.format_stack(frame))))
        self._exitCode = 128+signum
        self._exitMsg = 'Transform received signal {0}'.format(signum)
        
        # Reset signal handlers now - we don't want to recurse if the same signal arrives again (e.g. multiple ^C)
        resetTrfSignalHandlers()

        msg.critical('Attempting to write reports with known information...')
        self.generateReport(fast=True)
        if ('orphanKiller' in self._argdict):
            infanticide(message=True, listOrphans=True)
        else:
            infanticide(message=True)

        sys.exit(self._exitCode)
    
    ## @brief Setter for transform's validation dictionary
    #  @details This function updates the validation dictionary for the transform,
    #  updating values which are passed in the \c newValidationOptions argument.
    #  @param newValidationOptions Dictionary (or tuples) to update validation 
    #  dictionary with
    #  @return None
    def updateValidationDict(self, newValidationOptions):
        self.validation.update(newValidationOptions)

    ## @brief Getter function for transform validation dictionary
    #  @return Validiation dictionary
    def getValidationDict(self):
        return self.validation
    
    ## @brief Getter for a specific validation option
    #  @param key Validation dictionary key
    #  @return Valdiation key value or @c None if this key is absent
    def getValidationOption(self, key):
        if key in self.validation:
            return self.validation[key]
        else:
            return None

    ## @brief Return a list of fileArgs used by the transform
    #  @param \c io Filter files by io attribute
    #  @return List of argFile instances
    def getFiles(self, io = None):
        res = []
        msg.debug('Looking for file arguments matching: io={0}'.format(io))
        for argName, arg in iteritems(self._argdict):
            if isinstance(arg, argFile):
                msg.debug('Argument {0} is argFile type ({1!s})'.format(argName, arg))
                if io is not None and arg.io != io:
                    continue
                msg.debug('Argument {0} matches criteria'.format(argName))
                res.append(arg)
        return res


    def validateInFiles(self):
        if self._inFileValidationStart is None:
            self._inFileValidationStart = os.times()
            msg.debug('inFileValidationStart time is {0}'.format(self._inFileValidationStart))

        if (('skipFileValidation' in self._argdict and self._argdict['skipFileValidation'] is True) or
            ('skipInputFileValidation' in self._argdict and self._argdict['skipInputFileValidation'] is True) or
            ('fileValidation' in self._argdict and self._argdict['fileValidation'].value is False) or
            ('inputFileValidation' in self._argdict and self._argdict['inputFileValidation'].value is False)
            ):
            msg.info('Standard input file validation turned off for transform %s.', self.name)
        else:
            msg.info('Validating input files')
            if 'parallelFileValidation' in self._argdict:
                trfValidation.performStandardFileValidation(dictionary=self._dataDictionary, io='input', parallelMode=self._argdict['parallelFileValidation'].value )
            else:
                trfValidation.performStandardFileValidation(dictionary=self._dataDictionary, io='input')

        self._inFileValidationStop = os.times()
        msg.debug('inFileValidationStop time is {0}'.format(self._inFileValidationStop))

    def validateOutFiles(self):
        if self._outFileValidationStart is None:
            self._outFileValidationStart = os.times()
            msg.debug('outFileValidationStart time is {0}'.format(self._outFileValidationStart))

        if (('skipFileValidation' in self._argdict and self._argdict['skipFileValidation'] is True) or
            ('skipOutputFileValidation' in self._argdict and self._argdict['skipOutputFileValidation'] is True) or
            ('fileValidation' in self._argdict and self._argdict['fileValidation'].value is False) or
            ('outputFileValidation' in self._argdict and self._argdict['outputFileValidation'].value is False)
            ):
            msg.info('Standard output file validation turned off for transform %s.', self.name)
        else:
            msg.info('Validating output files')
            if 'parallelFileValidation' in self._argdict:
                trfValidation.performStandardFileValidation(dictionary=self._dataDictionary, io='output', parallelMode=self._argdict['parallelFileValidation'].value )
            else:
                trfValidation.performStandardFileValidation(dictionary=self._dataDictionary, io='output')

        self._outFileValidationStop = os.times()
        msg.debug('outFileValidationStop time is {0}'.format(self._outFileValidationStop))
