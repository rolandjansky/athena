# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @package PyJobTransforms.trfExe
#
# @brief Transform execution functions
# @details Standard transform executors
# @author atlas-comp-transforms-dev@cern.ch
# @version $Id: trfExe.py 792052 2017-01-13 13:36:51Z mavogel $

import copy
import json
import math
import os
import os.path as path
import re
import shutil
import signal
import subprocess
import sys
import time

import logging
from fnmatch import fnmatch
msg = logging.getLogger(__name__)

from PyJobTransforms.trfJobOptions import JobOptionsTemplate
from PyJobTransforms.trfUtils import asetupReport, unpackDBRelease, setupDBRelease, cvmfsDBReleaseCheck, forceToAlphaNum
from PyJobTransforms.trfUtils import ValgrindCommand, isInteractiveEnv, calcCpuTime, calcWallTime
from PyJobTransforms.trfUtils import bind_port
from PyJobTransforms.trfExitCodes import trfExit
from PyJobTransforms.trfLogger import stdLogLevels
from PyJobTransforms.trfMPTools import detectAthenaMPProcs, athenaMPOutputHandler


import PyJobTransforms.trfExceptions as trfExceptions
import PyJobTransforms.trfValidation as trfValidation
import PyJobTransforms.trfArgClasses as trfArgClasses
import PyJobTransforms.trfEnv as trfEnv

## @note This class contains the configuration information necessary to run an executor.
#  In most cases this is simply a collection of references to the parent transform, however,
#  abstraction is done via an instance of this class so that 'lightweight' executors can
#  be run for auxiliary purposes (e.g., file merging after AthenaMP was used, where the merging
#  is outside of the main workflow, but invoked in the main executor's "postExecute" method).
class executorConfig(object):

    ## @brief Configuration for an executor
    #  @param argdict Argument dictionary for this executor
    #  @param dataDictionary Mapping from input data names to argFile instances
    #  @param firstExecutor Boolean set to @c True if we are the first executor 
    def __init__(self, argdict={}, dataDictionary={}, firstExecutor=False):
        self._argdict = argdict
        self._dataDictionary = dataDictionary
        self._firstExecutor = firstExecutor
       
    @property
    def argdict(self):
        return self._argdict
    
    @argdict.setter
    def argdict(self, value):
        self._argdict = value

    @property
    def dataDictionary(self):
        return self._dataDictionary
    
    @dataDictionary.setter
    def dataDictionary(self, value):
        self._dataDictionary = value

    @property
    def firstExecutor(self):
        return self._firstExecutor
    
    @firstExecutor.setter
    def firstExecutor(self, value):
        self._firstExecutor = value
        
    ## @brief Set configuration properties from the parent transform
    #  @note  It's not possible to set firstExecutor here as the transform holds
    #  the name of the first executor, which we don't know... (should we?)
    def setFromTransform(self, trf):
        self._argdict = trf.argdict
        self._dataDictionary = trf.dataDictionary
        
    ## @brief Add a new object to the argdict
    def addToArgdict(self, key, value):
        self._argdict[key] = value

    ## @brief Add a new object to the dataDictionary
    def addToDataDictionary(self, key, value):
        self._dataDictionary[key] = value


## Executors always only even execute a single step, as seen by the transform
class transformExecutor(object):
    
    ## @brief Base class initaliser for transform executors
    #  @param name Transform name
    #  @param trf Parent transform
    #  @param conf executorConfig object (if @None then set from the @c trf directly)
    #  @param inData Data inputs this transform can start from. This should be a list, tuple or set
    #  consisting of each input data type. If a tuple (or list) is passed as a set member then this is interpreted as
    #  meaning that all of the data members in that tuple are necessary as an input.
    #  @note Curiously, sets are not allowed to be members of sets (they are not hashable, so no sub-sets)
    #  @param outData List of outputs this transform can produce (list, tuple or set can be used)
    def __init__(self, name = 'Dummy', trf = None, conf = None, inData = set(), outData = set()):
        # Some information to produce helpful log messages
        
        self._name = forceToAlphaNum(name)
        # Data this executor can start from and produce
        # Note we transform NULL to inNULL and outNULL as a convenience
        self._inData = set(inData)
        self._outData = set(outData)
        if 'NULL' in self._inData:
            self._inData.remove('NULL')
            self._inData.add('inNULL')
        if 'NULL' in self._outData:
            self._outData.remove('NULL')
            self._outData.add('outNULL')
            
        # It's forbidden for an executor to consume and produce the same datatype
        dataOverlap = self._inData & self._outData
        if len(dataOverlap) > 0:
            raise trfExceptions.TransformSetupException(trfExit.nameToCode('TRF_GRAPH_ERROR'), 
                                                        'Executor definition error, executor {0} is not allowed to produce and consume the same datatypes. Duplicated input/output types {1}'.format(self._name, ' '.join(dataOverlap)))
        
        ## Executor configuration:
        #  @note that if conf and trf are @c None then we'll probably set the conf up later (this is allowed and
        #  expected to be done once the master transform has figured out what it's doing for this job)
        if conf is not None:
            self.conf = conf
        else:
            self.conf = executorConfig()
            if trf is not None:
                self.conf.setFromTransform(trf)
        
        # Execution status
        self._hasExecuted = False
        self._rc = -1
        self._errMsg = None
        
        # Validation status
        self._hasValidated = False
        self._isValidated = False
        
        # Extra metadata
        # This dictionary holds extra metadata for this executor which will be 
        # provided in job reports
        self._extraMetadata = {}
        
        ## @note Place holders for resource consumption. CPU and walltime are available for all executors
        #  but currently only athena is instrumented to fill in memory stats (and then only if PerfMonSD is
        #  enabled). 
        self._preExeStart = None
        self._exeStart = self._exeStop = None
        self._valStart = self._valStop = None
        self._memStats = {}
        self._eventCount = None
        self._athenaMP = None
        self._dbMonitor = None
        
        # Holder for execution information about any merges done by this executor in MP mode
        self._myMerger = []

        
    ## Now define properties for these data members
    @property
    def myMerger(self):
        return self._myMerger

    @property
    def name(self):
        return self._name
    
    @property
    def substep(self):
        if '_substep' in dir(self):
            return self._substep
        return None
    
    @property
    def trf(self):
        if '_trf' in dir(self):
            return self._trf
        return None
    
    @trf.setter
    def trf(self, value):
        self._trf = value
        
    @property
    def inData(self):
        ## @note Might not be set in all executors...
        if '_inData' in dir(self):
            return self._inData
        return None
    
    @inData.setter
    def inData(self, value):
        self._inData = set(value)
        
    def inDataUpdate(self, value):
        ## @note Protect against _inData not yet being defined
        if '_inData' in dir(self):
            self._inData.update(value)
        else:
            ## @note Use normal setter
            self.inData = value


    @property
    def outData(self):
        ## @note Might not be set in all executors...
        if '_outData' in dir(self):
            return self._outData
        return None

    @outData.setter
    def outData(self, value):
        self._outData = set(value)
    
    def outDataUpdate(self, value):
        ## @note Protect against _outData not yet being defined
        if '_outData' in dir(self):
            self._outData.update(value)
        else:
            ## @note Use normal setter
            self.outData = value
    
    @property
    ## @note This returns the @b actual input data with which this executor ran
    #  (c.f. @c inData which returns all the possible data types this executor could run with) 
    def input(self):
        ## @note Might not be set in all executors...
        if '_input' in dir(self):
            return self._input
        return None
    
    @property
    ## @note This returns the @b actual output data with which this executor ran
    #  (c.f. @c outData which returns all the possible data types this executor could run with) 
    def output(self):
        ## @note Might not be set in all executors...
        if '_output' in dir(self):
            return self._output
        return None
    
    @property
    def extraMetadata(self):
        return self._extraMetadata
    
    @property
    def hasExecuted(self):
        return self._hasExecuted
    
    @property
    def rc(self):
        return self._rc
    
    @property
    def errMsg(self):
        return self._errMsg
    
    @property
    def validation(self):
        return self._validation
    
    @validation.setter
    def validation(self, value):
        self._validation = value
        
    @property
    def hasValidated(self):
        return self._hasValidated
    
    @property
    def isValidated(self):
        return self._isValidated
    
    ## @note At the moment only athenaExecutor sets this property, but that might be changed... 
    @property
    def first(self):
        if hasattr(self, '_first'):
            return self._first
        else:
            return None

    @property
    def preExeStartTimes(self):
        return self._preExeStart
    
    @property
    def exeStartTimes(self):
        return self._exeStart

    @property
    def exeStopTimes(self):
        return self._exeStop

    @property
    def valStartTimes(self):
        return self._valStart

    @property
    def valStopTimes(self):
        return self._valStop
    
    @property
    def preExeCpuTime(self):
        if self._preExeStart and self._exeStart:
            return calcCpuTime(self._preExeStart, self._exeStart)
        else:
            return None

    @property
    def preExeWallTime(self):
        if self._preExeStart and self._exeStart:
            return calcWallTime(self._preExeStart, self._exeStart)
        else:
            return None

    @property
    def cpuTime(self):
        if self._exeStart and self._exeStop:
            return calcCpuTime(self._exeStart, self._exeStop)
        else:
            return None

    @property
    def usrTime(self):
        if self._exeStart and self._exeStop:
            return self._exeStop[2] - self._exeStart[2]
        else:
            return None
        
    @property
    def sysTime(self):
        if self._exeStart and self._exeStop:
            return self._exeStop[3] - self._exeStart[3]
        else:
            return None

    @property
    def wallTime(self):
        if self._exeStart and self._exeStop:
            return calcWallTime(self._exeStart, self._exeStop)
        else:
            return None
        
    @property
    def memStats(self):
        return self._memStats

    @property
    def postExeCpuTime(self):
        if self._exeStop and self._valStart:
            return calcCpuTime(self._exeStop, self._valStart)
        else:
            return None

    @property
    def postExeWallTime(self):
        if self._exeStop and self._valStart:
            return calcWallTime(self._exeStop, self._valStart)
        else:
            return None

    @property
    def validationCpuTime(self):
        if self._valStart and self._valStop:
            return calcCpuTime(self._valStart, self._valStop)
        else:
            return None
    
    @property
    def validationWallTime(self):
        if self._valStart and self._valStop:
            return calcWallTime(self._valStart, self._valStop)
        else:
            return None

    @property
    def cpuTimeTotal(self):
        if self._preExeStart and self._valStop:
            return calcCpuTime(self._preExeStart, self._valStop)
        else:
            return None

    @property
    def wallTimeTotal(self):
        if self._preExeStart and self._valStop:
            return calcWallTime(self._preExeStart, self._valStop)
        else:
            return None
        
    @property
    def eventCount(self):
        return self._eventCount

    @property
    def athenaMP(self):
        return self._athenaMP

    @property
    def dbMonitor(self):
        return self._dbMonitor


    # set start times, if not set already
    def setPreExeStart(self):
        if self._preExeStart is None:
            self._preExeStart = os.times()
            msg.debug('preExeStart time is {0}'.format(self._preExeStart))

    def setValStart(self):
        if self._valStart is None:
            self._valStart = os.times()
            msg.debug('valStart time is {0}'.format(self._valStart))

    def preExecute(self, input = set(), output = set()):
        self.setPreExeStart()
        msg.info('Preexecute for %s' % self._name)
        
    def execute(self):
        self._exeStart = os.times()
        msg.debug('exeStart time is {0}'.format(self._exeStart))
        msg.info('Starting execution of %s' % self._name)
        self._hasExecuted = True
        self._rc = 0
        self._errMsg = ''
        msg.info('%s executor returns %d' % (self._name, self._rc))
        self._exeStop = os.times()
        msg.debug('preExeStop time is {0}'.format(self._exeStop))
        
    def postExecute(self):
        msg.info('Postexecute for %s' % self._name)
        
    def validate(self):
        self.setValStart()
        self._hasValidated = True        
        msg.info('Executor %s has no validation function - assuming all ok' % self._name)
        self._isValidated = True
        self._errMsg = ''
        self._valStop = os.times()
        msg.debug('valStop time is {0}'.format(self._valStop))
    
    ## Convenience function
    def doAll(self, input=set(), output=set()):
        self.preExecute(input, output)
        self.execute()
        self.postExecute()
        self.validate()

## @brief Special executor that will enable a logfile scan as part of its validation 
class logscanExecutor(transformExecutor):
    def __init__(self, name = 'Logscan'):
        super(logscanExecutor, self).__init__(name=name)
        self._errorMaskFiles = None
        self._logFileName = None

    def preExecute(self, input = set(), output = set()):
        self.setPreExeStart()
        msg.info('Preexecute for %s' % self._name)
        if 'logfile' in self.conf.argdict:
            self._logFileName = self.conf.argdict['logfile'].value
        
    def validate(self):
        self.setValStart()
        msg.info("Starting validation for {0}".format(self._name))
        if self._logFileName:
            ## TODO: This is  a cut'n'paste from the athenaExecutor
            #  We really should factorise this and use it commonly
            if 'ignorePatterns' in self.conf.argdict:
                igPat = self.conf.argdict['ignorePatterns'].value
            else:
                igPat = []
            if 'ignoreFiles' in self.conf.argdict:
                ignorePatterns = trfValidation.ignorePatterns(files = self.conf.argdict['ignoreFiles'].value, extraSearch=igPat)
            elif self._errorMaskFiles is not None:
                ignorePatterns = trfValidation.ignorePatterns(files = self._errorMaskFiles, extraSearch=igPat)
            else:
                ignorePatterns = trfValidation.ignorePatterns(files = athenaExecutor._defaultIgnorePatternFile, extraSearch=igPat)
            
            # Now actually scan my logfile
            msg.info('Scanning logfile {0} for errors'.format(self._logFileName))
            self._logScan = trfValidation.athenaLogFileReport(logfile = self._logFileName, ignoreList = ignorePatterns)
            worstError = self._logScan.worstError()
    
            # In general we add the error message to the exit message, but if it's too long then don't do
            # that and just say look in the jobReport
            if worstError['firstError']:
                if len(worstError['firstError']['message']) > athenaExecutor._exitMessageLimit:
                    if 'CoreDumpSvc' in worstError['firstError']['message']:
                        exitErrorMessage = "Core dump at line {0} (see jobReport for further details)".format(worstError['firstError']['firstLine'])
                    elif 'G4Exception' in worstError['firstError']['message']:
                        exitErrorMessage = "G4 exception at line {0} (see jobReport for further details)".format(worstError['firstError']['firstLine'])
                    else:
                        exitErrorMessage = "Long {0} message at line {1} (see jobReport for further details)".format(worstError['level'], worstError['firstError']['firstLine'])
                else:
                    exitErrorMessage = "Logfile error in {0}: \"{1}\"".format(self._logFileName, worstError['firstError']['message'])
            else:
                exitErrorMessage = "Error level {0} found (see athena logfile for details)".format(worstError['level'])
            
            # Very simple: if we get ERROR or worse, we're dead, except if ignoreErrors=True
            if worstError['nLevel'] == stdLogLevels['ERROR'] and ('ignoreErrors' in self.conf.argdict and self.conf.argdict['ignoreErrors'].value is True):
                msg.warning('Found ERRORs in the logfile, but ignoring this as ignoreErrors=True (see jobReport for details)')
            elif worstError['nLevel'] >= stdLogLevels['ERROR']:
                self._isValidated = False
                msg.error('Fatal error in athena logfile (level {0})'.format(worstError['level']))
                raise trfExceptions.TransformLogfileErrorException(trfExit.nameToCode('TRF_EXEC_LOGERROR'), 
                                                                       'Fatal error in athena logfile: "{0}"'.format(exitErrorMessage))

        # Must be ok if we got here!
        msg.info('Executor {0} has validated successfully'.format(self.name))
        self._isValidated = True
        self._errMsg = ''        

        self._valStop = os.times()
        msg.debug('valStop time is {0}'.format(self._valStop))


class echoExecutor(transformExecutor):
    def __init__(self, name = 'Echo', trf = None):
        
        # We are only changing the default name here
        super(echoExecutor, self).__init__(name=name, trf=trf)

    
    def execute(self):
        self._exeStart = os.times()
        msg.debug('exeStart time is {0}'.format(self._exeStart))
        msg.info('Starting execution of %s' % self._name)        
        msg.info('Transform argument dictionary now follows:')
        for k, v in self.conf.argdict.iteritems():
            print "%s = %s" % (k, v)
        self._hasExecuted = True
        self._rc = 0
        self._errMsg = ''
        msg.info('%s executor returns %d' % (self._name, self._rc))
        self._exeStop = os.times()
        msg.debug('exeStop time is {0}'.format(self._exeStop))


class scriptExecutor(transformExecutor):
    def __init__(self, name = 'Script', trf = None, conf = None, inData = set(), outData = set(), 
                 exe = None, exeArgs = None, memMonitor = True):
        # Name of the script we want to execute
        self._exe = exe
        
        # With arguments (currently this means paste in the corresponding _argdict entry)
        self._exeArgs = exeArgs
        
        super(scriptExecutor, self).__init__(name=name, trf=trf, conf=conf, inData=inData, outData=outData)
        
        self._extraMetadata.update({'script' : exe})
        
        # Decide if we echo script output to stdout
        self._echoOutput = False

        # Can either be written by base class or child   
        self._cmd = None
        
        self._memMonitor = memMonitor

    @property
    def exe(self):
        return self._exe
    
    @exe.setter
    def exe(self, value):
        self._exe = value
        self._extraMetadata['script'] = value

    @property
    def exeArgs(self):
        return self._exeArgs
    
    @exeArgs.setter
    def exeArgs(self, value):
        self._exeArgs = value
#        self._extraMetadata['scriptArgs'] = value

    def preExecute(self, input = set(), output = set()):
        self.setPreExeStart()
        msg.debug('scriptExecutor: Preparing for execution of {0} with inputs {1} and outputs {2}'.format(self.name, input, output))

        self._input = input
        self._output = output
        
        ## @note If an inherited class has set self._cmd leave it alone
        if self._cmd is None:
            self._buildStandardCommand()
        msg.info('Will execute script as %s', self._cmd)
        
        # Define this here to have it for environment detection messages
        self._logFileName = "log.{0}".format(self._name)
        
        ## @note Query the environment for echo configuration
        # Let the manual envars always win over auto-detected settings
        if 'TRF_ECHO' in os.environ:
            msg.info('TRF_ECHO envvar is set - enabling command echoing to stdout')
            self._echoOutput = True
        elif 'TRF_NOECHO' in os.environ:
            msg.info('TRF_NOECHO envvar is set - disabling command echoing to stdout')
            self._echoOutput = False
        # PS1 is for sh, bash; prompt is for tcsh and zsh
        elif isInteractiveEnv():
            msg.info('Interactive environment detected (stdio or stdout is a tty) - enabling command echoing to stdout')
            self._echoOutput = True
        elif 'TZHOME' in os.environ:
            msg.info('Tier-0 environment detected - enabling command echoing to stdout')
            self._echoOutput = True
        if self._echoOutput == False:
            msg.info('Batch/grid running - command outputs will not be echoed. Logs for {0} are in {1}'.format(self._name, self._logFileName))

        # Now setup special loggers for logging execution messages to stdout and file
        self._echologger = logging.getLogger(self._name)
        self._echologger.setLevel(logging.INFO)
        self._echologger.propagate = False
        
        self._exeLogFile = logging.FileHandler(self._logFileName, mode='w')
        self._exeLogFile.setFormatter(logging.Formatter('%(asctime)s %(message)s', datefmt='%H:%M:%S'))
        self._echologger.addHandler(self._exeLogFile)
        
        if self._echoOutput:
            self._echostream = logging.StreamHandler(sys.stdout)
            self._echostream.setFormatter(logging.Formatter('%(name)s %(asctime)s %(message)s', datefmt='%H:%M:%S'))
            self._echologger.addHandler(self._echostream)

    def _buildStandardCommand(self):
        if self._exe:
            self._cmd = [self.exe, ]
        else:
            raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_EXEC_SETUP_FAIL'), 
                                                            'No executor set in {0}'.format(self.__class__.__name__))
        for arg in self.exeArgs:
            if arg in self.conf.argdict:
                # If we have a list then add each element to our list, else just str() the argument value
                # Note if there are arguments which need more complex transformations then
                # consider introducing a special toExeArg() method.
                if isinstance(self.conf.argdict[arg].value, list):
                    self._cmd.extend([ str(v) for v in self.conf.argdict[arg].value])
                else:
                    self._cmd.append(str(self.conf.argdict[arg].value))


    def execute(self):
        self._hasExecuted = True
        msg.info('Starting execution of {0} ({1})'.format(self._name, self._cmd))
        
        self._exeStart = os.times()
        msg.debug('exeStart time is {0}'.format(self._exeStart))
        if ('execOnly' in self.conf.argdict and self.conf.argdict['execOnly'] == True):
            msg.info('execOnly flag is set - execution will now switch, replacing the transform')
            os.execvp(self._cmd[0], self._cmd)

        try:
            p = subprocess.Popen(self._cmd, shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, bufsize = 1)
            
            if self._memMonitor:
                try:
                    self._memSummaryFile = 'mem.summary.' + self._name + '.json'
                    memMonitorCommand = ['MemoryMonitor', '--pid', str(p.pid), '--filename', 'mem.full.' + self._name, 
                                         '--json-summary', self._memSummaryFile, '--interval', '30']
                    mem_proc = subprocess.Popen(memMonitorCommand, shell = False, close_fds=True)
                    # TODO - link mem.full.current to mem.full.SUBSTEP
                except Exception, e:
                    msg.warning('Failed to spawn memory monitor for {0}: {1}'.format(self._name, e))
                    self._memMonitor = False
            
            while p.poll() is None:
                line = p.stdout.readline()
                if line:
                    self._echologger.info(line.rstrip())
            # Hoover up remaining buffered output lines
            for line in p.stdout:
                self._echologger.info(line.rstrip())
    
            self._rc = p.returncode
            msg.info('%s executor returns %d' % (self._name, self._rc))
            self._exeStop = os.times()
            msg.debug('exeStop time is {0}'.format(self._exeStop))
        except OSError as e:
            errMsg = 'Execution of {0} failed and raised OSError: {1}'.format(self._cmd[0], e)
            msg.error(errMsg)
            raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_EXEC'), errMsg)
        finally:
            if self._memMonitor:
                try:
                    mem_proc.send_signal(signal.SIGUSR1)
                    countWait = 0
                    while (not mem_proc.poll()) and countWait < 10:
                        time.sleep(0.1)
                        countWait += 1
                except OSError, UnboundLocalError:
                    pass
        
        
    def postExecute(self):
        if hasattr(self._exeLogFile, 'close'):
            self._exeLogFile.close()
        if self._memMonitor:
            try:
                memFile = open(self._memSummaryFile)
                self._memStats = json.load(memFile)
            except Exception, e:
                msg.warning('Failed to load JSON memory summmary file {0}: {1}'.format(self._memSummaryFile, e))
                self._memMonitor = False
                self._memStats = {}
            

    def validate(self):
        if self._valStart is None:
            self._valStart = os.times()
            msg.debug('valStart time is {0}'.format(self._valStart))
        self._hasValidated = True
        
        ## Check rc
        if self._rc == 0:
            msg.info('Executor {0} validated successfully (return code {1})'.format(self._name, self._rc))
            self._isValidated = True
            self._errMsg = ''
        else:
            # Want to learn as much as possible from the non-zero code
            # this is a bit hard in general, although one can do signals.
            # Probably need to be more specific per exe, i.e., athena non-zero codes
            self._isValidated = False
            if self._rc < 0:
                # Map return codes to what the shell gives (128 + SIGNUM)
                self._rc = 128 - self._rc
            if trfExit.codeToSignalname(self._rc) != "":
                self._errMsg = '{0} got a {1} signal (exit code {2})'.format(self._name, trfExit.codeToSignalname(self._rc), self._rc)
            else:
                self._errMsg = 'Non-zero return code from %s (%d)' % (self._name, self._rc)
            raise trfExceptions.TransformValidationException(trfExit.nameToCode('TRF_EXEC_FAIL'), self._errMsg)

        ## Check event counts (always do this by default)
        #  Do this here so that all script executors have this by default (covers most use cases with events)
        if 'checkEventCount' in self.conf.argdict.keys() and self.conf.argdict['checkEventCount'].returnMyValue(exe=self) is False:
            msg.info('Event counting for substep {0} is skipped'.format(self.name))
        else:
            checkcount=trfValidation.eventMatch(self)
            checkcount.decide()
            self._eventCount = checkcount.eventCount
            msg.info('Event counting for substep {0} passed'.format(self.name))

        self._valStop = os.times()
        msg.debug('valStop time is {0}'.format(self._valStop))



class athenaExecutor(scriptExecutor):
    _exitMessageLimit = 200 # Maximum error message length to report in the exitMsg
    _defaultIgnorePatternFile = ['atlas_error_mask.db']
    
    ## @brief Initialise athena executor
    #  @param name Executor name
    #  @param trf Parent transform
    #  @param skeletonFile athena skeleton job options file (optionally this can be a list of skeletons
    #  that will be given to athena.py in order); can be set to @c None to disable writing job options 
    #  files at all
    #  @param inputDataTypeCountCheck List of input datatypes to apply preExecute event count checks to;
    #  default is @c None, which means check all inputs
    #  @param exe Athena execution script
    #  @param exeArgs Transform argument names whose value is passed to athena
    #  @param substep The athena substep this executor represents (alias for the name)
    #  @param inputEventTest Boolean switching the skipEvents < inputEvents test
    #  @param perfMonFile Name of perfmon file for this substep (used to retrieve vmem/rss information) @b DEPRECATED
    #  @param tryDropAndReload Boolean switch for the attempt to add '--drop-and-reload' to athena args
    #  @param extraRunargs Dictionary of extra runargs to write into the job options file, using repr
    #  @param runtimeRunargs Dictionary of extra runargs to write into the job options file, using str
    #  @param literalRunargs List of extra lines to write into the runargs file
    #  @param dataArgs List of datatypes that will always be given as part of this transform's runargs
    #  even if not actually processed by this substep (used, e.g., to set random seeds for some generators)
    #  @param checkEventCount Compare the correct number of events in the output file (either input file size or maxEvents)
    #  @param errorMaskFiles List of files to use for error masks in logfile scanning (@c None means not set for this
    #  executor, so use the transform or the standard setting)
    #  @param manualDataDictionary Instead of using the inData/outData parameters that binds the data types for this
    #  executor to the workflow graph, run the executor manually with these data parameters (useful for 
    #  post-facto executors, e.g., for AthenaMP merging)
    #  @param memMonitor Enable subprocess memory monitoring
    #  @param disableMP Ensure that AthenaMP is not used (i.e., also unset 
    #  @c ATHENA_PROC_NUMBER before execution)
    #  @note The difference between @c extraRunargs, @runtimeRunargs and @literalRunargs is that: @c extraRunargs 
    #  uses repr(), so the RHS is the same as the python object in the transform; @c runtimeRunargs uses str() so 
    #  that a string can be interpreted at runtime; @c literalRunargs allows the direct insertion of arbitary python
    #  snippets into the runArgs file.
    def __init__(self, name = 'athena', trf = None, conf = None, skeletonFile = 'PyJobTransforms/skeleton.dummy.py', inData = set(), 
                 outData = set(), inputDataTypeCountCheck = None, exe = 'athena.py', exeArgs = ['athenaopts'], substep = None, inputEventTest = True,
                 perfMonFile = None, tryDropAndReload = True, extraRunargs = {}, runtimeRunargs = {},
                 literalRunargs = [], dataArgs = [], checkEventCount = False, errorMaskFiles = None,
                 manualDataDictionary = None, memMonitor = True, disableMP = False):
        
        self._substep = forceToAlphaNum(substep)
        self._inputEventTest = inputEventTest
        self._tryDropAndReload = tryDropAndReload
        self._extraRunargs = extraRunargs
        self._runtimeRunargs = runtimeRunargs
        self._literalRunargs = literalRunargs
        self._dataArgs = dataArgs
        self._errorMaskFiles = errorMaskFiles
        self._inputDataTypeCountCheck = inputDataTypeCountCheck
        self._disableMP = disableMP

        if perfMonFile:
            self._perfMonFile = None
            msg.warning("Resource monitoring from PerfMon is now deprecated")
        
        # SkeletonFile can be None (disable) or a string or a list of strings - normalise it here
        if type(skeletonFile) is str:
            self._skeleton = [skeletonFile]
        else:
            self._skeleton = skeletonFile
            
        super(athenaExecutor, self).__init__(name=name, trf=trf, conf=conf, inData=inData, outData=outData, exe=exe, 
                                             exeArgs=exeArgs, memMonitor=memMonitor)
        
        # Add athena specific metadata
        self._extraMetadata.update({'substep': substep})

        # Setup JO templates
        if self._skeleton is not None:
            self._jobOptionsTemplate = JobOptionsTemplate(exe = self, version = '$Id: trfExe.py 792052 2017-01-13 13:36:51Z mavogel $')
        else:
            self._jobOptionsTemplate = None

    @property
    def inputDataTypeCountCheck(self):
        return self._inputDataTypeCountCheck
    
    @inputDataTypeCountCheck.setter
    def inputDataTypeCountCheck(self, value):
        self._inputDataTypeCountCheck = value
        
    @property
    def substep(self):
        return self._substep

    @property
    def disableMP(self):
        return self._disableMP
    
    @disableMP.setter
    def disableMP(self, value):
        self._disableMP = value
        
    def preExecute(self, input = set(), output = set()):
        self.setPreExeStart()
        msg.debug('Preparing for execution of {0} with inputs {1} and outputs {2}'.format(self.name, input, output))
        
        # Check we actually have events to process!
        inputEvents = 0
        dt = ""
        if self._inputDataTypeCountCheck is None:
            self._inputDataTypeCountCheck = input
        for dataType in self._inputDataTypeCountCheck:
            thisInputEvents = self.conf.dataDictionary[dataType].nentries
            if thisInputEvents > inputEvents:
                inputEvents = thisInputEvents
                dt = dataType

        # Now take into account skipEvents and maxEvents
        if ('skipEvents' in self.conf.argdict and 
            self.conf.argdict['skipEvents'].returnMyValue(name=self._name, substep=self._substep, first=self.conf.firstExecutor) is not None):
            mySkipEvents = self.conf.argdict['skipEvents'].returnMyValue(name=self._name, substep=self._substep, first=self.conf.firstExecutor)
        else:
            mySkipEvents = 0

        if ('maxEvents' in self.conf.argdict and 
            self.conf.argdict['maxEvents'].returnMyValue(name=self._name, substep=self._substep, first=self.conf.firstExecutor) is not None):
            myMaxEvents = self.conf.argdict['maxEvents'].returnMyValue(name=self._name, substep=self._substep, first=self.conf.firstExecutor)
        else:
            myMaxEvents = -1
        
        # Any events to process...?
        if (self._inputEventTest and mySkipEvents > 0 and mySkipEvents >= inputEvents):
            raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_NOEVENTS'),
                                                           'No events to process: {0} (skipEvents) >= {1} (inputEvents of {2}'.format(mySkipEvents, inputEvents, dt))
        
        try:
            # Expected events to process
            if (myMaxEvents != -1):
                if (self.inData and next(iter(self.inData)) == 'inNULL'):
                    expectedEvents = myMaxEvents
                else:
                    expectedEvents = min(inputEvents-mySkipEvents, myMaxEvents)
            else:
                expectedEvents = inputEvents-mySkipEvents
        except TypeError as e:
            # catching type error from UNDEFINED inputEvents count
            msg.info('input event count is UNDEFINED, setting expectedEvents to 0')
            expectedEvents = 0
        
        # Try to detect AthenaMP mode and number of workers
        if self._disableMP:
            self._athenaMP = 0
        else:
            self._athenaMP = detectAthenaMPProcs(self.conf.argdict)

            # Small hack to detect cases where there are so few events that it's not worthwhile running in MP mode
            # which also avoids issues with zero sized files
            if expectedEvents < self._athenaMP:
                msg.info("Disabling AthenaMP as number of input events to process is too low ({0} events for {1} workers)".format(expectedEvents, self._athenaMP))
                self._disableMP = True
                self._athenaMP = 0
            
        # And if this is (still) athenaMP, then set some options for workers and output file report
        if self._athenaMP:
            self._athenaMPWorkerTopDir = 'athenaMP-workers-{0}-{1}'.format(self._name, self._substep)
            self._athenaMPFileReport = 'athenaMP-outputs-{0}-{1}'.format(self._name, self._substep)
            self._athenaMPEventOrdersFile = 'athenamp_eventorders.txt.{0}'.format(self._name)
            if 'athenaMPUseEventOrders' in self.conf.argdict and self.conf._argdict['athenaMPUseEventOrders'].value is True:
                self._athenaMPReadEventOrders = True
            else:
                self._athenaMPReadEventOrders = False          
            # Decide on scheduling
            if ('athenaMPStrategy' in self.conf.argdict and 
                (self.conf.argdict['athenaMPStrategy'].returnMyValue(name=self._name, substep=self._substep, first=self.conf.firstExecutor) is not None)):
                self._athenaMPStrategy = self.conf.argdict['athenaMPStrategy'].returnMyValue(name=self._name, substep=self._substep, first=self.conf.firstExecutor)
            else:
                self._athenaMPStrategy = 'SharedQueue'

            # See if we have options for the target output file size
            if 'athenaMPMergeTargetSize' in self.conf.argdict:
                for dataType in output:
                    if dataType in self.conf.argdict['athenaMPMergeTargetSize'].value:
                        self.conf._dataDictionary[dataType].mergeTargetSize = self.conf.argdict['athenaMPMergeTargetSize'].value[dataType] * 1000000 # Convert from MB to B
                        msg.info('Set target merge size for {0} to {1}'.format(dataType, self.conf._dataDictionary[dataType].mergeTargetSize))
                    else:
                        # Use a globbing strategy
                        matchedViaGlob = False
                        for mtsType, mtsSize in self.conf.argdict['athenaMPMergeTargetSize'].value.iteritems():
                            if fnmatch(dataType, mtsType):
                                self.conf._dataDictionary[dataType].mergeTargetSize = mtsSize * 1000000 # Convert from MB to B
                                msg.info('Set target merge size for {0} to {1} from "{2}" glob'.format(dataType, self.conf._dataDictionary[dataType].mergeTargetSize, mtsType))
                                matchedViaGlob = True
                                break
                        if not matchedViaGlob and "ALL" in self.conf.argdict['athenaMPMergeTargetSize'].value:
                            self.conf._dataDictionary[dataType].mergeTargetSize = self.conf.argdict['athenaMPMergeTargetSize'].value["ALL"] * 1000000 # Convert from MB to B
                            msg.info('Set target merge size for {0} to {1} from "ALL" value'.format(dataType, self.conf._dataDictionary[dataType].mergeTargetSize))
         
            # For AthenaMP jobs we ensure that the athena outputs get the suffix _000
            # so that the mother process output file (if it exists) can be used directly
            # as soft linking can lead to problems in the PoolFileCatalog (see ATLASJT-317)
            for dataType in output:
                self.conf._dataDictionary[dataType].originalName = self.conf._dataDictionary[dataType].value[0]
                if 'eventService' not in self.conf.argdict or 'eventService' in self.conf.argdict and self.conf.argdict['eventService'].value==False:
                    self.conf._dataDictionary[dataType].value[0] += "_000"
                    msg.info("Updated athena output filename for {0} to {1}".format(dataType, self.conf._dataDictionary[dataType].value[0]))
        else:
            self._athenaMPWorkerTopDir = self._athenaMPFileReport = None


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
            if path.islink(dbrelease):
                dbrelease = path.realpath(dbrelease)
            if dbrelease:
                # Classic tarball - filename format is DBRelease-X.Y.Z.tar.gz
                dbdMatch = re.match(r'DBRelease-([\d\.]+)\.tar\.gz', path.basename(dbrelease))
                if dbdMatch:
                    msg.debug('DBRelease setting {0} matches classic tarball file'.format(dbrelease))
                    if not os.access(dbrelease, os.R_OK):
                        msg.warning('Transform was given tarball DBRelease file {0}, but this is not there'.format(dbrelease))
                        msg.warning('I will now try to find DBRelease {0} in cvmfs'.format(dbdMatch.group(1)))
                        dbrelease = dbdMatch.group(1)
                        dbsetup = cvmfsDBReleaseCheck(dbrelease)
                    else:
                        # Check if the DBRelease is setup
                        msg.debug('Setting up {0} from {1}'.format(dbdMatch.group(1), dbrelease))
                        unpacked, dbsetup = unpackDBRelease(tarball=dbrelease, dbversion=dbdMatch.group(1))
                        if unpacked:
                            # Now run the setup.py script to customise the paths to the current location...
                            setupDBRelease(dbsetup)
                # For cvmfs we want just the X.Y.Z release string (and also support 'current')
                else:
                    dbsetup = cvmfsDBReleaseCheck(dbrelease)
        
        ## Look for environment updates and perpare the athena command line
        self._envUpdate = trfEnv.environmentUpdate()
        self._envUpdate.setStandardEnvironment(self.conf.argdict, name=self.name, substep=self.substep)
        self._prepAthenaCommandLine() 
        
                
        super(athenaExecutor, self).preExecute(input, output)
        
        # Now we always write a wrapper, because it's very convenient for re-running individual substeps
        # This will have asetup and/or DB release setups in it
        # Do this last in this preExecute as the _cmd needs to be finalised
        msg.info('Now writing wrapper for substep executor {0}'.format(self._name))
        self._writeAthenaWrapper(asetup=asetupString, dbsetup=dbsetup)
        msg.info('Athena will be executed in a subshell via {0}'.format(self._cmd))
        
                
    def postExecute(self):
        super(athenaExecutor, self).postExecute()

        # If this was an athenaMP run then we need to update output files
        if self._athenaMP:
            outputDataDictionary = dict([ (dataType, self.conf.dataDictionary[dataType]) for dataType in self._output ])
            ## @note Update argFile values to have the correct outputs from the MP workers
            skipFileChecks=False
            if 'eventService' in self.conf.argdict and self.conf.argdict['eventService'].value:
                skipFileChecks=True
            athenaMPOutputHandler(self._athenaMPFileReport, self._athenaMPWorkerTopDir, outputDataDictionary, self._athenaMP, skipFileChecks, self.conf.argdict)
            for dataType in self._output:
                if self.conf.dataDictionary[dataType].io == "output" and len(self.conf.dataDictionary[dataType].value) > 1:
                    self._smartMerge(self.conf.dataDictionary[dataType])
        
        if 'TXT_JIVEXMLTGZ' in self.conf.dataDictionary:
            self._targzipJiveXML()


    def validate(self):
        self.setValStart()
        self._hasValidated = True
        deferredException = None
        
        ## Our parent will check the RC for us
        try:
            super(athenaExecutor, self).validate()
        except trfExceptions.TransformValidationException, e:
            # In this case we hold this exception until the logfile has been scanned
            msg.error('Validation of return code failed: {0!s}'.format(e))
            deferredException = e
                
        # Logfile scan setup
        # Always use ignorePatterns from the command line
        # For patterns in files, pefer the command line first, then any special settings for
        # this executor, then fallback to the standard default (atlas_error_mask.db)
        if 'ignorePatterns' in self.conf.argdict:
            igPat = self.conf.argdict['ignorePatterns'].value
        else:
            igPat = []
        if 'ignoreFiles' in self.conf.argdict:
            ignorePatterns = trfValidation.ignorePatterns(files = self.conf.argdict['ignoreFiles'].value, extraSearch=igPat)
        elif self._errorMaskFiles is not None:
            ignorePatterns = trfValidation.ignorePatterns(files = self._errorMaskFiles, extraSearch=igPat)
        else:
            ignorePatterns = trfValidation.ignorePatterns(files = athenaExecutor._defaultIgnorePatternFile, extraSearch=igPat)
        
        # Now actually scan my logfile
        msg.info('Scanning logfile {0} for errors in substep {1}'.format(self._logFileName, self._substep))
        self._logScan = trfValidation.athenaLogFileReport(logfile=self._logFileName, substepName=self._substep,
                                                          ignoreList=ignorePatterns)
        worstError = self._logScan.worstError()
        self._dbMonitor = self._logScan.dbMonitor()
        

        # In general we add the error message to the exit message, but if it's too long then don't do
        # that and just say look in the jobReport
        if worstError['firstError']:
            if len(worstError['firstError']['message']) > athenaExecutor._exitMessageLimit:
                if 'CoreDumpSvc' in worstError['firstError']['message']:
                    exitErrorMessage = "Core dump at line {0} (see jobReport for further details)".format(worstError['firstError']['firstLine'])
                elif 'G4Exception' in worstError['firstError']['message']:
                    exitErrorMessage = "G4 exception at line {0} (see jobReport for further details)".format(worstError['firstError']['firstLine'])
                else:
                    exitErrorMessage = "Long {0} message at line {1} (see jobReport for further details)".format(worstError['level'], worstError['firstError']['firstLine'])
            else:
                exitErrorMessage = "Logfile error in {0}: \"{1}\"".format(self._logFileName, worstError['firstError']['message'])
        else:
            exitErrorMessage = "Error level {0} found (see athena logfile for details)".format(worstError['level'])

        # If we failed on the rc, then abort now
        if deferredException is not None:
            # Add any logfile information we have
            if worstError['nLevel'] >= stdLogLevels['ERROR']:
                deferredException.errMsg = deferredException.errMsg + "; {0}".format(exitErrorMessage)
            raise deferredException
        
        
        # Very simple: if we get ERROR or worse, we're dead, except if ignoreErrors=True
        if worstError['nLevel'] == stdLogLevels['ERROR'] and ('ignoreErrors' in self.conf.argdict and self.conf.argdict['ignoreErrors'].value is True):
            msg.warning('Found ERRORs in the logfile, but ignoring this as ignoreErrors=True (see jobReport for details)')
        elif worstError['nLevel'] >= stdLogLevels['ERROR']:
            self._isValidated = False
            msg.error('Fatal error in athena logfile (level {0})'.format(worstError['level']))
            raise trfExceptions.TransformLogfileErrorException(trfExit.nameToCode('TRF_EXEC_LOGERROR'), 
                                                                   'Fatal error in athena logfile: "{0}"'.format(exitErrorMessage))

        # Must be ok if we got here!
        msg.info('Executor {0} has validated successfully'.format(self.name))
        self._isValidated = True

        self._valStop = os.times()
        msg.debug('valStop time is {0}'.format(self._valStop))

    ## @brief Prepare the correct command line to be used to invoke athena
    def _prepAthenaCommandLine(self):
        ## Start building up the command line
        #  N.B. it's possible we might have cases where 'athena' and 'athenaopt' should be substep args
        #  but at the moment this hasn't been requested.
        if 'athena' in self.conf.argdict:
            self._exe = self.conf.argdict['athena'].value
        self._cmd = [self._exe]
        
        # Find options for the current substep. Name is prioritised (e.g. RAWtoESD) over alias (e.g. r2e). Last look for 'all'
        currentSubstep = None
        if 'athenaopts' in self.conf.argdict:
            if self.name in self.conf.argdict['athenaopts'].value:
                currentSubstep = self.name
                if self.substep in self.conf.argdict['athenaopts'].value:
                    msg.info('Athenaopts found for {0} and {1}, joining options. '
                             'Consider changing your configuration to use just the name or the alias of the substep.'
                             .format(currentSubstep, self.substep))
                    self.conf.argdict['athenaopts'].value[currentSubstep].extend(self.conf.argdict['athenaopts'].value[self.substep])
                    del self.conf.argdict['athenaopts'].value[self.substep]
                    msg.debug('Athenaopts: {0}'.format(self.conf.argdict['athenaopts'].value))
            elif self.substep in self.conf.argdict['athenaopts'].value:
                currentSubstep = self.substep
            elif 'all' in self.conf.argdict['athenaopts'].value:
                currentSubstep = 'all'

        # See if there's a preloadlibs and a request to update LD_PRELOAD for athena
        preLoadUpdated = dict()
        if 'LD_PRELOAD' in self._envUpdate._envdict:
            preLoadUpdated[currentSubstep] = False
            if 'athenaopts' in self.conf.argdict:
                if currentSubstep is not None:
                    for athArg in self.conf.argdict['athenaopts'].value[currentSubstep]:
                        # This code is pretty ugly as the athenaopts argument contains
                        # strings which are really key/value pairs
                        if athArg.startswith('--preloadlib'):
                            try:
                                i = self.conf.argdict['athenaopts'].value[currentSubstep].index(athArg)
                                v = athArg.split('=', 1)[1]
                                msg.info('Updating athena --preloadlib option for substep {1} with: {0}'.format(self._envUpdate.value('LD_PRELOAD'), self.name))
                                newPreloads = ":".join(set(v.split(":")) | set(self._envUpdate.value('LD_PRELOAD').split(":")))
                                self.conf.argdict['athenaopts']._value[currentSubstep][i] = '--preloadlib={0}'.format(newPreloads)
                            except Exception, e:
                                msg.warning('Failed to interpret athena option: {0} ({1})'.format(athArg, e))
                            preLoadUpdated[currentSubstep] = True
                        break
            if not preLoadUpdated[currentSubstep]:
                msg.info('Setting athena preloadlibs for substep {1} to: {0}'.format(self._envUpdate.value('LD_PRELOAD'), self.name))
                if 'athenaopts' in self.conf.argdict:
                    if currentSubstep is not None:
                        self.conf.argdict['athenaopts'].value[currentSubstep].append("--preloadlib={0}".format(self._envUpdate.value('LD_PRELOAD')))
                    else:
                        self.conf.argdict['athenaopts'].value['all'] = ["--preloadlib={0}".format(self._envUpdate.value('LD_PRELOAD'))]
                else:
                    self.conf.argdict['athenaopts'] = trfArgClasses.argSubstepList(["--preloadlib={0}".format(self._envUpdate.value('LD_PRELOAD'))])

        # Now update command line with the options we have (including any changes to preload)
        if 'athenaopts' in self.conf.argdict:
            if currentSubstep is None and "all" in self.conf.argdict['athenaopts'].value:
                self._cmd.extend(self.conf.argdict['athenaopts'].value['all'])
            elif currentSubstep in self.conf.argdict['athenaopts'].value:
                self._cmd.extend(self.conf.argdict['athenaopts'].value[currentSubstep])
        
        ## Add --drop-and-reload if possible (and allowed!)
        if self._tryDropAndReload:
            if 'valgrind' in self.conf._argdict and self.conf._argdict['valgrind'].value is True:
                msg.info('Disabling "--drop-and-reload" because the job is configured to use Valgrind')
            elif 'athenaopts' in self.conf.argdict:
                athenaConfigRelatedOpts = ['--config-only','--drop-and-reload','--drop-configuration','--keep-configuration']
                # Note for athena options we split on '=' so that we properly get the option and not the whole "--option=value" string
                if currentSubstep is None:
                    currentSubstep = 'all'
                if currentSubstep in self.conf.argdict['athenaopts'].value:
                    conflictOpts = set(athenaConfigRelatedOpts).intersection(set([opt.split('=')[0] for opt in self.conf.argdict['athenaopts'].value[currentSubstep]]))
                    if len(conflictOpts) > 0:
                        msg.info('Not appending "--drop-and-reload" to athena command line because these options conflict: {0}'.format(list(conflictOpts)))
                    else:
                        msg.info('Appending "--drop-and-reload" to athena options')
                        self._cmd.append('--drop-and-reload')
                else:
                    msg.info('No Athenaopts for substep {0}, appending "--drop-and-reload" to athena options'.format(self.name))
                    self._cmd.append('--drop-and-reload')
            else:
                # This is the 'standard' case - so drop and reload should be ok
                msg.info('Appending "--drop-and-reload" to athena options')
                self._cmd.append('--drop-and-reload')
        else:
            msg.info('Skipping test for "--drop-and-reload" in this executor')
            
        # Add topoptions
        if self._skeleton is not None:
            self._cmd += self._topOptionsFiles
            msg.info('Updated script arguments with topoptions: %s' % self._cmd)


    ## @brief Write a wrapper script which runs asetup and then Athena.
    def _writeAthenaWrapper(
        self,
        asetup = None,
        dbsetup = None
        ):
        self._originalCmd = self._cmd
        self._asetup      = asetup
        self._dbsetup     = dbsetup
        self._wrapperFile = 'runwrapper.{name}.sh'.format(name = self._name)
        msg.debug(
            'Preparing wrapper file {wrapperFileName} with ' +
            'asetup={asetupStatus} and dbsetup={dbsetupStatus}'.format(
                wrapperFileName = self._wrapperFile,
                asetupStatus    = self._asetup,
                dbsetupStatus   = self._dbsetup
            )
        )
        try:
            with open(self._wrapperFile, 'w') as wrapper:
                print >>wrapper, '#! /bin/sh'
                if asetup:
                    print >>wrapper, "# asetup"
                    print >>wrapper, 'echo Sourcing {AtlasSetupDirectory}/scripts/asetup.sh {asetupStatus}'.format(
                        AtlasSetupDirectory = os.environ['AtlasSetup'],
                        asetupStatus        = asetup
                    )
                    print >>wrapper, 'source {AtlasSetupDirectory}/scripts/asetup.sh {asetupStatus}'.format(
                        AtlasSetupDirectory = os.environ['AtlasSetup'],
                        asetupStatus        = asetup
                    )
                    print >>wrapper, 'if [ ${?} != "0" ]; then exit 255; fi '
                    #check for TestArea if it exists then source setup.sh, used for local patches areas
                    print >>wrapper, '#Need to check and source for  TestArea/build/cfgdir/setup.sh  '
                    print >>wrapper, 'if [ ${TestArea} ]; then '
                    print >>wrapper, '    cfgdir=`/bin/ls ${TestArea}/build/ | grep gcc` '
                    print >>wrapper, '    if [ ${?} == "0" ]; then ' 
                    print >>wrapper, '        echo "${TestArea}/build/${cfgdir} exist , will source setup.sh " '
                    print >>wrapper, '        source ${TestArea}/build/${cfgdir}/setup.sh '
                    print >>wrapper, '    fi '
                    print >>wrapper, 'fi '

                if dbsetup:
                    dbroot = path.dirname(dbsetup)
                    dbversion = path.basename(dbroot)
                    print >>wrapper, "# DBRelease setup"
                    print >>wrapper, 'echo Setting up DBRelease {dbroot} environment'.format(dbroot = dbroot)
                    print >>wrapper, 'export DBRELEASE={dbversion}'.format(dbversion = dbversion)
                    print >>wrapper, 'export CORAL_AUTH_PATH={directory}'.format(directory = path.join(dbroot, 'XMLConfig'))
                    print >>wrapper, 'export CORAL_DBLOOKUP_PATH={directory}'.format(directory = path.join(dbroot, 'XMLConfig'))
                    print >>wrapper, 'export TNS_ADMIN={directory}'.format(directory = path.join(dbroot, 'oracle-admin'))
                    print >>wrapper, 'DATAPATH={dbroot}:$DATAPATH'.format(dbroot = dbroot)
                if self._disableMP:
                    print >>wrapper, "# AthenaMP explicitly disabled for this executor"
                    print >>wrapper, "export ATHENA_PROC_NUMBER=0"
                if self._envUpdate.len > 0:
                    print >>wrapper, "# Customised environment"
                    for envSetting in  self._envUpdate.values:
                        if not envSetting.startswith('LD_PRELOAD'):
                            print >>wrapper, "export", envSetting
                # If Valgrind is engaged, a serialised Athena configuration file
                # is generated for use with a subsequent run of Athena with
                # Valgrind.
                if 'valgrind' in self.conf._argdict and self.conf._argdict['valgrind'].value is True:
                    msg.info('Valgrind engaged')
                    # Define the file name of the serialised Athena
                    # configuration.
                    AthenaSerialisedConfigurationFile = "{name}Conf.pkl".format(
                        name = self._name
                    )
                    # Run Athena for generation of its serialised configuration.
                    print >>wrapper, ' '.join(self._cmd), "--config-only={0}".format(AthenaSerialisedConfigurationFile)
                    print >>wrapper, 'if [ $? != "0" ]; then exit 255; fi'
                    # Generate a Valgrind command, suppressing or ussing default
                    # options as requested and extra options as requested.
                    if 'valgrindDefaultOpts' in self.conf._argdict:
                        defaultOptions = self.conf._argdict['valgrindDefaultOpts'].value
                    else:
                        defaultOptions = True
                    if 'valgrindExtraOpts' in self.conf._argdict:
                        extraOptionsList = self.conf._argdict['valgrindExtraOpts'].value
                    else:
                        extraOptionsList = None
                    msg.debug("requested Valgrind command basic options: {options}".format(options = defaultOptions))
                    msg.debug("requested Valgrind command extra options: {options}".format(options = extraOptionsList))
                    command = ValgrindCommand(
                        defaultOptions = defaultOptions,
                        extraOptionsList = extraOptionsList,
                        AthenaSerialisedConfigurationFile = \
                            AthenaSerialisedConfigurationFile
                    )
                    msg.debug("Valgrind command: {command}".format(command = command))
                    print >>wrapper, command
                else:
                    msg.info('Valgrind not engaged')
                    # run Athena command
                    print >>wrapper, ' '.join(self._cmd)
            os.chmod(self._wrapperFile, 0755)
        except (IOError, OSError) as e:
            errMsg = 'error writing athena wrapper {fileName}: {error}'.format(
                fileName = self._wrapperFile,
                error = e
            )
            msg.error(errMsg)
            raise trfExceptions.TransformExecutionException(
                trfExit.nameToCode('TRF_EXEC_SETUP_WRAPPER'),
                errMsg
            )
        self._cmd = [path.join('.', self._wrapperFile)]


    ## @brief Manage smart merging of output files
    #  @param fileArg File argument to merge
    def _smartMerge(self, fileArg):
        ## @note only file arguments which support selfMerge() can be merged
        if 'selfMerge' not in dir(fileArg):
            msg.info('Files in {0} cannot merged (no selfMerge() method is implemented)'.format(fileArg.name))
            return
        
        if fileArg.mergeTargetSize == 0:
            msg.info('Files in {0} will not be merged as target size is set to 0'.format(fileArg.name))
            return

        ## @note Produce a list of merge jobs - this is a list of lists
        mergeCandidates = [list()]
        currentMergeSize = 0
        for fname in fileArg.value:
            size = fileArg.getSingleMetadata(fname, 'file_size')
            if type(size) not in (int, long):
                msg.warning('File size metadata for {0} was not correct, found type {1}. Aborting merge attempts.'.format(fileArg, type(size)))
                return
            # if there is no file in the job, then we must add it
            if len(mergeCandidates[-1]) == 0:
                msg.debug('Adding file {0} to current empty merge list'.format(fname))
                mergeCandidates[-1].append(fname)
                currentMergeSize += size
                continue
            # see if adding this file gets us closer to the target size (but always add if target size is negative)
            if fileArg.mergeTargetSize < 0 or math.fabs(currentMergeSize + size - fileArg.mergeTargetSize) < math.fabs(currentMergeSize - fileArg.mergeTargetSize):
                msg.debug('Adding file {0} to merge list {1} as it gets closer to the target size'.format(fname, mergeCandidates[-1]))
                mergeCandidates[-1].append(fname)
                currentMergeSize += size
                continue
            # close this merge list and start a new one
            msg.debug('Starting a new merge list with file {0}'.format(fname))
            mergeCandidates.append([fname])
            currentMergeSize = size
            
        msg.debug('First pass splitting will merge files in this way: {0}'.format(mergeCandidates))
        
        if len(mergeCandidates) == 1:
            # Merging to a single file, so use the original filename that the transform
            # was started with
            mergeNames = [fileArg.originalName]
        else:
            # Multiple merge targets, so we need a set of unique names
            counter = 0
            mergeNames = []
            for mergeGroup in mergeCandidates:
                # Note that the individual worker files get numbered with 3 digit padding,
                # so these non-padded merges should be fine
                mergeName = fileArg.originalName + '_{0}'.format(counter)
                while path.exists(mergeName):
                    counter += 1
                    mergeName = fileArg.originalName + '_{0}'.format(counter)
                mergeNames.append(mergeName)
                counter += 1
        # Now actually do the merges
        for targetName, mergeGroup, counter in zip(mergeNames, mergeCandidates, range(len(mergeNames))):
            msg.info('Want to merge files {0} to {1}'.format(mergeGroup, targetName))
            if len(mergeGroup) <= 1:
                msg.info('Skip merging for single file')
            else:
                ## We want to parallelise this part!
                self._myMerger.append(fileArg.selfMerge(output=targetName, inputs=mergeGroup, counter=counter, argdict=self.conf.argdict))


    def _targzipJiveXML(self):
        #tgzipping JiveXML files
        targetTGZName = self.conf.dataDictionary['TXT_JIVEXMLTGZ'].value[0]
        if os.path.exists(targetTGZName):
            os.remove(targetTGZName)

        import tarfile
        fNameRE = re.compile("JiveXML\_\d+\_\d+.xml")

        # force gz compression
        tar = tarfile.open(targetTGZName, "w:gz")
        for fName in os.listdir('.'):
            matches = fNameRE.findall(fName)
            if len(matches) > 0:
                if fNameRE.findall(fName)[0] == fName:
                    msg.info('adding %s to %s' % (fName, targetTGZName))
                    tar.add(fName)

        tar.close()
        msg.info('JiveXML compression: %s has been written and closed.' % (targetTGZName))


## @brief Athena executor where failure is not consisered fatal
class optionalAthenaExecutor(athenaExecutor):

    # Here we validate, but will suppress any errors
    def validate(self):
        self.setValStart()
        try:
            super(optionalAthenaExecutor, self).validate()
        except trfExceptions.TransformValidationException, e:
            # In this case we hold this exception until the logfile has been scanned
            msg.warning('Validation failed for {0}: {1}'.format(self._name, e))
            self._isValidated = False
            self._errMsg = e.errMsg
            self._rc = e.errCode
        self._valStop = os.times()
        msg.debug('valStop time is {0}'.format(self._valStop))


class hybridPOOLMergeExecutor(athenaExecutor):
    ## @brief Initialise hybrid POOL merger athena executor
    #  @param name Executor name
    #  @param trf Parent transform
    #  @param skeletonFile athena skeleton job options file
    #  @param exe Athena execution script
    #  @param exeArgs Transform argument names whose value is passed to athena
    #  @param substep The athena substep this executor represents
    #  @param inputEventTest Boolean switching the skipEvents < inputEvents test
    #  @param perfMonFile Name of perfmon file for this substep (used to retrieve vmem/rss information)
    #  @param tryDropAndReload Boolean switch for the attempt to add '--drop-and-reload' to athena args
    #  @param hybridMerge Boolean activating hybrid merger (if set to 'None' then the hybridMerge will
    #  be used if n_inputs <= 16, otherwise a classic merge will happen for better downstream i/o 
    #  performance) 
    def __init__(self, name = 'hybridPOOLMerge', trf = None, conf = None, skeletonFile = 'RecJobTransforms/skeleton.MergePool_tf.py', inData = set(), 
                 outData = set(), exe = 'athena.py', exeArgs = ['athenaopts'], substep = None, inputEventTest = True,
                 perfMonFile = None, tryDropAndReload = True, hybridMerge = None, extraRunargs = {},
                 manualDataDictionary = None, memMonitor = True):
        
        # By default we will do a hybridMerge
        self._hybridMerge = hybridMerge
        self._hybridMergeTmpFile = 'events.pool.root'
        super(hybridPOOLMergeExecutor, self).__init__(name, trf=trf, conf=conf, skeletonFile=skeletonFile, inData=inData, 
                                                      outData=outData, exe=exe, exeArgs=exeArgs, substep=substep,
                                                      inputEventTest=inputEventTest, perfMonFile=perfMonFile, 
                                                      tryDropAndReload=tryDropAndReload, extraRunargs=extraRunargs,
                                                      manualDataDictionary=manualDataDictionary, memMonitor=memMonitor)
    
    def preExecute(self, input = set(), output = set()):
        self.setPreExeStart()
        # Now check to see if the fastPoolMerger option was set
        if 'fastPoolMerge' in self.conf.argdict:
            msg.info('Setting hybrid merge to {0}'.format(self.conf.argdict['fastPoolMerge'].value))
            self._hybridMerge =  self.conf.argdict['fastPoolMerge'].value
        else:
            # Hybrid merging really needs some proper validation, so only use
            # it if specifically requested
            msg.info("Automatic hybrid merging is disabled use the '--fastPoolMerge' flag if you want to switch it on")
            self._hybridMerge = False
            
        if self._hybridMerge:
            # If hybridMerge is activated then we process no events at the athena step,
            # so set a ridiculous skipEvents value
            msg.info("Setting skipEvents=1000000 to skip event processing during athena metadata merge")
            self._extraRunargs.update({'skipEvents': 1000000})
        
        super(hybridPOOLMergeExecutor, self).preExecute(input=input, output=output)

    
    def execute(self):
        # First call the parent executor, which will manage the athena execution for us
        super(hybridPOOLMergeExecutor, self).execute()
        
        # Now, do we need to do the fast event merge?
        if not self._hybridMerge:
            return
        
        # Save the stub file for debugging...
        stubFile = self.conf.dataDictionary[list(self._output)[0]].value[0]
        stubFileSave = stubFile + ".tmp"
        msg.info('Saving metadata stub file {0} to {1}'.format(stubFile, stubFileSave))
        shutil.copy(stubFile, stubFileSave)

        # Now do the hybrid merge steps - note we disable checkEventCount for this - it doesn't make sense here
        fastConf = copy.copy(self.conf)
        fastConf.addToArgdict('checkEventCount', trfArgClasses.argSubstepBool("all:False", runarg=False))
        fastEventMerge1 = scriptExecutor(name='fastEventMerge_step1', conf=fastConf, inData=self._inData, outData=self._outData,
                                        exe='mergePOOL', exeArgs=None)
        fastEventMerge1._cmd = ['mergePOOL', '-o', self._hybridMergeTmpFile]
        for fname in self.conf.dataDictionary[list(self._input)[0]].value:
            fastEventMerge1._cmd.extend(['-i', fname])
        fastEventMerge1._cmd.extend(['-e', 'MetaData', '-e', 'MetaDataHdrDataHeaderForm', '-e', 'MetaDataHdrDataHeader', '-e', 'MetaDataHdr', '-e', 'MetaDataHdrForm'])

        msg.debug('Constructed this command line for fast event merge step 1: {0}'.format(fastEventMerge1._cmd))
        fastEventMerge1.doAll()
        

        fastEventMerge2 = scriptExecutor(name='fastEventMerge_step2', conf=fastConf, inData=self._inData, outData=self._outData,
                                        exe='mergePOOL', exeArgs=None)
        fastEventMerge2._cmd = ['mergePOOL', '-o', self._hybridMergeTmpFile]
        fastEventMerge2._cmd.extend(['-i', self.conf.dataDictionary[list(self._output)[0]].value[0]])

        msg.debug('Constructed this command line for fast event merge step 2: {0}'.format(fastEventMerge2._cmd))
        fastEventMerge2.doAll()
        
        # Ensure we count all the mergePOOL stuff in the resource report
        self._exeStop = os.times()
        msg.debug('exeStop time is {0}'.format(self._exeStop))

        # And finally...
        msg.info('Renaming {0} to {1}'.format(self._hybridMergeTmpFile, self.conf.dataDictionary[list(self._output)[0]].value[0]))
        try:
            os.rename(self._hybridMergeTmpFile, self.conf.dataDictionary[list(self._output)[0]].value[0])
            self.conf.dataDictionary[list(self._output)[0]]._resetMetadata()
            # Stupid PoolFileCatalog now has the wrong GUID for the output file. Delete it for safety.
            if os.access('PoolFileCatalog.xml', os.R_OK):
                os.unlink('PoolFileCatalog.xml')
        except (IOError, OSError) as e:
            raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_OUTPUT_FILE_ERROR'), 
                                                            'Exception raised when renaming {0} to {1}: {2}'.format(self._hybridMergeTmpFile, self.conf.dataDictionary[list(self._output)[0]].value[0], e))


## @brief Specialist executor to manage the handling of multiple implicit input
#  and output files within the reduction framework. 
class reductionFrameworkExecutor(athenaExecutor):
    
    ## @brief Take inputDAODFile and setup the actual outputs needed
    #  in this job.
    def preExecute(self, input=set(), output=set()):
        self.setPreExeStart()
        msg.debug('Preparing for execution of {0} with inputs {1} and outputs {2}'.format(self.name, input, output))

        if 'NTUP_PILEUP' not in output:
            if 'reductionConf' not in self.conf.argdict:
                raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_REDUCTION_CONFIG_ERROR'),
                                                                'No reduction configuration specified')

            if 'DAOD' not in output:
                raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_REDUCTION_CONFIG_ERROR'),
                                                                'No base name for DAOD reduction')
        
            for reduction in self.conf.argdict['reductionConf'].value:
                dataType = 'DAOD_' + reduction
                outputName = 'DAOD_' + reduction + '.' + self.conf.argdict['outputDAODFile'].value[0]
                msg.info('Adding reduction output type {0}'.format(dataType))
                output.add(dataType)
                newReduction = trfArgClasses.argPOOLFile(outputName, io='output', runarg=True, type='AOD',
                                                     name=reduction)
                # References to _trf - can this be removed?
                self.conf.dataDictionary[dataType] = newReduction
            
            # Clean up the stub file from the executor input and the transform's data dictionary
            # (we don't remove the actual argFile instance)
            output.remove('DAOD')
            del self.conf.dataDictionary['DAOD']
            del self.conf.argdict['outputDAODFile']
        
            msg.info('Data dictionary is now: {0}'.format(self.conf.dataDictionary))
            msg.info('Input/Output: {0}/{1}'.format(input, output))
        
        msg.info('Data dictionary is now: {0}'.format(self.conf.dataDictionary))
        msg.info('Input/Output: {0}/{1}'.format(input, output))
        super(reductionFrameworkExecutor, self).preExecute(input, output)


## @brief Specialist executor to manage the handling of multiple implicit input
#  and output files within the reduction framework. 
#  @note This is the temporary executor used for NTUP->DNTUP. It will be dropped
#  after the move to D(x)AOD.
class reductionFrameworkExecutorNTUP(athenaExecutor):
    
    ## @brief Take inputDNTUPFile and setup the actual outputs needed
    #  in this job.
    def preExecute(self, input=set(), output=set()):
        self.setPreExeStart()
        msg.debug('Preparing for execution of {0} with inputs {1} and outputs {2}'.format(self.name, input, output))

        if 'reductionConf' not in self.conf.argdict:
            raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_REDUCTION_CONFIG_ERROR'),
                                                            'No reduction configuration specified')
        if 'DNTUP' not in output:
            raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_REDUCTION_CONFIG_ERROR'),
                                                            'No base name for DNTUP reduction')
        
        for reduction in self.conf.argdict['reductionConf'].value:
            dataType = 'DNTUP_' + reduction
            # Prodsys 1 request - don't add a suffix, but replace DNTUP with DNTUP_TYPE
            outputName = self.conf.argdict['outputDNTUPFile'].value[0].replace('DNTUP', dataType)
            if outputName == self.conf.argdict['outputDNTUPFile'].value[0]:
                # Rename according to the old scheme
                outputName = self.conf.argdict['outputDNTUPFile'].value[0] + '_' + reduction + '.root'
            msg.info('Adding reduction output type {0}, target filename {1}'.format(dataType, outputName))
            output.add(dataType)
            newReduction = trfArgClasses.argNTUPFile(outputName, io='output', runarg=True, type='NTUP', subtype=dataType,
                                                     name=reduction, treeNames=['physics'])
            self.conf.dataDictionary[dataType] = newReduction
            
        # Clean up the stub file from the executor input and the transform's data dictionary
        # (we don't remove the actual argFile instance)
        output.remove('DNTUP')
        del self.conf.dataDictionary['DNTUP']
        del self.conf.argdict['outputDNTUPFile']
        
        msg.info('Data dictionary is now: {0}'.format(self.conf.dataDictionary))
        msg.info('Input/Output: {0}/{1}'.format(input, output))
        
        super(reductionFrameworkExecutorNTUP, self).preExecute(input, output)


## @brief Specialist execution class for merging DQ histograms
class DQMergeExecutor(scriptExecutor):
    def __init__(self, name='DQHistMerge', trf=None, conf=None, inData=set(['HIST_AOD', 'HIST_ESD']), outData=set(['HIST']),
                 exe='DQHistogramMerge.py', exeArgs = [], memMonitor = True):
        
        self._histMergeList = 'HISTMergeList.txt'
        
        super(DQMergeExecutor, self).__init__(name=name, trf=trf, conf=conf, inData=inData, outData=outData, exe=exe, 
                                              exeArgs=exeArgs, memMonitor=memMonitor)


    def preExecute(self, input = set(), output = set()):
        self.setPreExeStart()
        msg.debug('Preparing for execution of {0} with inputs {1} and outputs {2}'.format(self.name, input, output))

        super(DQMergeExecutor, self).preExecute(input=input, output=output)

        # Write the list of files to be merged
        with open(self._histMergeList, 'w') as DQMergeFile:
            for dataType in input:
                for fname in self.conf.dataDictionary[dataType].value:
                    self.conf.dataDictionary[dataType]._getNumberOfEvents([fname])
                    print >>DQMergeFile, fname
            
        self._cmd.append(self._histMergeList)
        
        # Add the output file
        if len(output) != 1:
            raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_EXEC_SETUP_FAIL'),
                                                            'One (and only one) output file must be given to {0} (got {1})'.format(self.name, len(output)))
        outDataType = list(output)[0]
        self._cmd.append(self.conf.dataDictionary[outDataType].value[0])
        
        # Set the run_post_processing to False
        self._cmd.append('False')

    def validate(self):
        self.setValStart()
        super(DQMergeExecutor, self).validate()

        exitErrorMessage = ''
        # Base class validation successful, Now scan the logfile for missed errors.
        try:
            logScan = trfValidation.scriptLogFileReport(self._logFileName)
            worstError = logScan.worstError()

            # In general we add the error message to the exit message, but if it's too long then don't do
            # that and just say look in the jobReport
            if worstError['firstError']:
                if len(worstError['firstError']['message']) > logScan._msgLimit:
                    exitErrorMessage = "Long {0} message at line {1}" \
                                       " (see jobReport for further details)".format(worstError['level'],
                                        worstError['firstError']['firstLine'])
                else:
                    exitErrorMessage = "Logfile error in {0}: \"{1}\"".format(self._logFileName,
                                                                              worstError['firstError']['message'])
        except (OSError, IOError) as e:
            exitCode = trfExit.nameToCode('TRF_EXEC_LOGERROR')
            raise trfExceptions.TransformValidationException(exitCode,
                  'Exception raised while attempting to scan logfile {0}: {1}'.format(self._logFileName, e))

        if worstError['nLevel'] == stdLogLevels['ERROR'] and (
                'ignoreErrors' in self.conf.argdict and self.conf.argdict['ignoreErrors'].value is True):
            msg.warning('Found ERRORs in the logfile, but ignoring this as ignoreErrors=True (see jobReport for details)')

        elif worstError['nLevel'] >= stdLogLevels['ERROR']:
            self._isValidated = False
            msg.error('Fatal error in script logfile (level {0})'.format(worstError['level']))
            exitCode = trfExit.nameToCode('TRF_EXEC_LOGERROR')
            raise trfExceptions.TransformLogfileErrorException(exitCode, 'Fatal error in script logfile: "{0}"'.format(exitErrorMessage))

        # Must be ok if we got here!
        msg.info('Executor {0} has validated successfully'.format(self.name))
        self._isValidated = True

        self._valStop = os.times()
        msg.debug('valStop time is {0}'.format(self._valStop))


## @brief Specialist execution class for merging NTUPLE files
class NTUPMergeExecutor(scriptExecutor):

    def preExecute(self, input = set(), output = set()):
        self.setPreExeStart()
        msg.debug('[NTUP] Preparing for execution of {0} with inputs {1} and outputs {2}'.format(self.name, input, output))

        # Basic command, and allow overwrite of the output file
        if self._exe is None:
            self._exe = 'hadd'
        self._cmd = [self._exe, "-f"]
        
        
        # Add the output file
        if len(output) != 1:
            raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_EXEC_SETUP_FAIL'),
                                                            'One (and only one) output file must be given to {0} (got {1})'.format(self.name, len(output)))
        outDataType = list(output)[0]
        self._cmd.append(self.conf.dataDictionary[outDataType].value[0])
        # Add to be merged to the cmd chain
        for dataType in input:
            self._cmd.extend(self.conf.dataDictionary[dataType].value)

        super(NTUPMergeExecutor, self).preExecute(input=input, output=output)


## @brief Specalise the script executor to deal with the BS merge oddity of excluding empty DRAWs 
class bsMergeExecutor(scriptExecutor):

    def preExecute(self, input = set(), output = set()):
        self.setPreExeStart()
        self._inputBS = list(input)[0]
        self._outputBS = list(output)[0]
        self._maskedFiles = []
        self._useStubFile = False
        if 'maskEmptyInputs' in self.conf.argdict and self.conf.argdict['maskEmptyInputs'].value is True:
            eventfullFiles = []
            for fname in self.conf.dataDictionary[self._inputBS].value:
                nEvents = self.conf.dataDictionary[self._inputBS].getSingleMetadata(fname, 'nentries')
                msg.debug('Found {0} events in file {1}'.format(nEvents, fname))
                if isinstance(nEvents, int) and nEvents > 0:
                    eventfullFiles.append(fname)
            self._maskedFiles = list(set(self.conf.dataDictionary[self._inputBS].value) - set(eventfullFiles))
            if len(self._maskedFiles) > 0:
                msg.info('The following input files are masked because they have 0 events: {0}'.format(' '.join(self._maskedFiles)))
                if len(eventfullFiles) == 0:
                    if 'emptyStubFile' in self.conf.argdict and path.exists(self.conf.argdict['emptyStubFile'].value):
                        self._useStubFile = True
                        msg.info("All input files are empty - will use stub file {0} as output".format(self.conf.argdict['emptyStubFile'].value))
                    else:
                        raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_INPUT_FILE_ERROR'), 
                                                                        'All input files had zero events - aborting BS merge')            
        
        # Write the list of input files to a text file, so that testMergedFiles can swallow it
        self._mergeBSFileList = '{0}.list'.format(self._name)
        self._mergeBSLogfile = '{0}.out'.format(self._name)
        try:
            with open(self._mergeBSFileList, 'w') as BSFileList:
                for fname in self.conf.dataDictionary[self._inputBS].value:
                    if fname not in self._maskedFiles:
                        print >>BSFileList, fname
        except (IOError, OSError) as e:
            errMsg = 'Got an error when writing list of BS files to {0}: {1}'.format(self._mergeBSFileList, e)
            msg.error(errMsg)
            raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_EXEC_SETUP_WRAPPER'), errMsg)
        
        # Hope that we were given a correct filename...
        self._outputFilename = self.conf.dataDictionary[self._outputBS].value[0]
        if self._outputFilename.endswith('._0001.data'):
            self._doRename = False
            self._outputFilename = self._outputFilename.split('._0001.data')[0]    
        elif self.conf.argdict['allowRename'].value == True:
            # OK, non-fatal, we go for a renaming
            msg.info('Output filename does not end in "._0001.data" will proceed, but be aware that the internal filename metadata will be wrong')
            self._doRename = True
        else:
            # No rename allowed, so we are dead...
            errmsg = 'Output filename for outputBS_MRGFile must end in "._0001.data" or infile metadata will be wrong'
            raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_OUTPUT_FILE_ERROR'), errmsg)
        
        # Set the correct command for execution
        self._cmd = [self._exe, self._mergeBSFileList, '0', self._outputFilename] 
        
        super(bsMergeExecutor, self).preExecute(input=input, output=output)
    
    def execute(self):
        if self._useStubFile:
            # Need to fake execution!
            self._exeStart = os.times()
            msg.debug('exeStart time is {0}'.format(self._exeStart))
            msg.info("Using stub file for empty BS output - execution is fake")
            if self._outputFilename != self.conf.argdict['emptyStubFile'].value:
                os.rename(self.conf.argdict['emptyStubFile'].value, self._outputFilename)            
            self._memMonitor = False
            self._hasExecuted = True
            self._rc = 0
            self._exeStop = os.times()
            msg.debug('exeStop time is {0}'.format(self._exeStop))
        else:
            super(bsMergeExecutor, self).execute()
        
    def postExecute(self):
        if self._useStubFile:
            pass
        elif self._doRename:
            self._expectedOutput = self._outputFilename + '._0001.data'
            msg.info('Renaming {0} to {1}'.format(self._expectedOutput, self.conf.dataDictionary[self._outputBS].value[0]))
            try:
                os.rename(self._outputFilename + '._0001.data', self.conf.dataDictionary[self._outputBS].value[0])
            except OSError, e:
                raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_OUTPUT_FILE_ERROR'), 
                                                                'Exception raised when renaming {0} to {1}: {2}'.format(self._outputFilename, self.conf.dataDictionary[self._outputBS].value[0], e))
        super(bsMergeExecutor, self).postExecute()
        
                

class tagMergeExecutor(scriptExecutor):
    
    def preExecute(self, input = set(), output = set()):
        self.setPreExeStart()
        # Just need to write the customised CollAppend command line
        self._cmd = [self._exe, '-src']
        for dataType in input:
            for fname in self.conf.dataDictionary[dataType].value:
                self._cmd.extend(['PFN:{0}'.format(fname), 'RootCollection'])
        self._cmd.extend(['-dst', 'PFN:{0}'.format(self.conf.dataDictionary[list(output)[0]].value[0]), 'RootCollection', '-nevtcached', '5000'])
        
        # In AthenaMP jobs the output file can be created empty, which CollAppend does not like
        # so remove it
        if os.access(self.conf.dataDictionary[list(output)[0]].value[0], os.F_OK):
            os.unlink(self.conf.dataDictionary[list(output)[0]].value[0]) 
        
        super(tagMergeExecutor, self).preExecute(input=input, output=output)

        
    def validate(self):
        self.setValStart()
        super(tagMergeExecutor, self).validate()
        
        # Now scan the logfile...
        try:
            msg.debug('Scanning TAG merging logfile {0}'.format(self._logFileName))
            with open(self._logFileName) as logfile:
                for line in logfile:
                    # Errors are signaled by 'error' (case independent) and NOT ('does not exist' or 'hlterror')
                    # Logic copied from Tier 0 TAGMerge_trf.py
                    if 'error' in line.lower():
                        if 'does not exist' in line:
                            continue
                        if 'hlterror' in line:
                            continue
                        raise trfExceptions.TransformValidationException(trfExit.nameToCode('TRF_EXEC_LOGERROR'),
                                                                         'Found this error message in the logfile {0}: {1}'.format(self._logFileName, line))
        except (OSError, IOError) as e:
                        raise trfExceptions.TransformValidationException(trfExit.nameToCode('TRF_EXEC_LOGERROR'),
                                                                         'Exception raised while attempting to scan logfile {0}: {1}'.format(self._logFileName, e))            
        self._valStop = os.times()
        msg.debug('valStop time is {0}'.format(self._valStop))


## @brief Archive transform - use tar
class archiveExecutor(scriptExecutor):

    def preExecute(self, input = set(), output = set()):
        self.setPreExeStart()
        self._memMonitor = False

        if 'exe' in self.conf.argdict:
            self._exe = self.conf.argdict['exe']

        if self._exe == 'tar':
            self._cmd = [self._exe, '-c', '-v',]
            self._cmd.extend(['-f', self.conf.argdict['outputArchFile'].value[0]])
            if 'compressionType' in self.conf.argdict:
                if self.conf.argdict['compressionType'] == 'gzip':
                    self._cmd.append('-z')
                elif self.conf.argdict['compressionType'] == 'bzip2':
                    self._cmd.append('-j')
                elif self.conf.argdict['compressionType'] == 'none':
                    pass
        elif self._exe == 'zip':
            self._cmd = [self._exe]
            if 'compressionLevel' in self.conf.argdict:
                self._cmd.append(self.conf.argdict['compressionLevel'])
            self._cmd.extend([self.conf.argdict['outputArchFile'].value[0]])
            if '.' not in self.conf.argdict['outputArchFile'].value[0]:
                errmsg = 'Output filename must end in ".", ".zip" or ".anyname" '
                raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_OUTPUT_FILE_ERROR'), errmsg)
        self._cmd.extend(self.conf.argdict['inputDataFile'].value)
        super(archiveExecutor, self).preExecute(input=input, output=output)
