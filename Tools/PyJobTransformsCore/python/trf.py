# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @package trf
#
#  @brief Main package containing the @em JobTransform class.
#  @details Main module of @em PyJobTransformsCore package containing the base class @em JobTransform.

from __future__ import with_statement, print_function
from past.builtins import execfile
import os, sys, time, stat, re, math, subprocess, signal, inspect
import stat as statconsts
from copy import copy
try:
    import cPickle as pickle
except Exception:
    import pickle
import fileutil
from PyJobTransformsCore import trfenv, trferr, trfconsts, AtlasErrorCodes
from PyJobTransformsCore.trferr import AthenaLogChecker, InputFileError, JobOptionsNotFoundError, TransformArgumentError, TransformDefinitionError, TransformEnvironmentError, TransformErrorDiagnoser, TransformValidationError
from PyJobTransformsCore.trfutil import Author, CommentLine, PostJobOptionsFile, PreJobOptionsFile, SQLiteSupport, TRF_SETTING, VersionString, find_joboptions, get_atlas_release, get_files, setDefaultSignalHandlers, setTrfSignalHandler, strip_suffix
from PyJobTransformsCore.JobReport import JobReport, JobInfo, _extraMetadataDict
from PyJobTransformsCore.TransformLogger import TransformLogger
from PyJobTransformsCore.TransformConfig import TransformConfig
from PyJobTransformsCore.runargs import RunArguments, RunOptions, VALIDATION_DICT
from PyJobTransformsCore.VTimer import vTimer
from PyJobTransformsCore.basic_trfarg import Argument
from PyJobTransformsCore.full_trfarg import OptionArg, JobConfigArg
from AthenaCommon import ExitCodes  

## @class JobTransform
# 
#  @brief Base class for all transformations
#
#  @details All concrete job transformations must derive from this class, and add appropriate arguments to it 
#  (with the @c add() member function, called when instantiating a derived class.)
#
#  Derived classes can override member functions, in particular @c runJob() in the case of composite transforms.
#  @warning Not intended for use directly without subclassing (and adding transform arguments).
class JobTransform(TransformLogger):
    
    ## job transform source file suffix  
    _fileSuffix = '_trf'

    ## run arguments name 
    _runArgsName = 'runArgs'

    ## Command line options shared between composite transforms
    _sharedRunOpts = [ 'athenaopts', 'ignoreunknown', 'ignoreall', 'ignoreerrors', 'tcmalloc', 'leakcheckexecute', 'rss', 'vmem', 'extrametadatadict', 'extraignorefilters', 'usenewmeta', 'omitvalidation', 'command', 'mcinput', 'usesqlite', 'uploadtoami' ]

    ## Options that are also accepted as arguments (via full_trfarg.OptionArg)
    _optionArgNames = [ 'athenaopts', 'ignoreunknown', 'ignoreall', 'ignoreerrors', 'tcmalloc', 'leakcheckexecute', 'rss', 'vmem', 'extrametadatadict', 'extraignorefilters', 'usenewmeta', 'omitvalidation', 'command', 'mcinput', 'usesqlite', 'uploadtoami' ]
    
    ## When @c _ignoreUnknown is @em True, unknown errors are ignored.
    _ignoreUnknown = False
    
    ## When @c _ignoreAll is @em True, the transformation is permitted to execute as long as possible.
    _ignoreAll = False
    
    ## When @c _ignoreErrors is @em True, a successful athena job will force any errors 
    #  detected by the transformation to be ignored.
    _ignoreErrors = False
    
    ## @brief Constructor for the JobTransform class. Not intended for direct use. Vanilla instance is invalid. 
    #  @details JobTransform instance in intended to to be subclassed and customised for specific job transformation purposes 
    #  with input files (optional) and output files.
    #  @param authors List of trfutil.Author instances
    #  @param help Documentation string describing what the transformation does
    #  @param version Version number based on the svn tag. Default value retrieved from @c cmt/version.cmt
    #  @param skeleton Name of skeleton job options file to be used by @c athena.py
    #  @param name Name of transform used in logging. Default value is the file's name with its @em _trf suffix omitted.
    #  @param config TransformConfig instance contains the job's configuration. Default value is @c TransformConfig('defaultConfig')
    #  @param lastInChain Boolean value to define a trf as the last in a chain of trfs. Default value is @c True.
    #  @warning Not intended for use directly without subclassing (and adding transform arguments).
    def __init__( self,
                  authors=[], 
                  help='Base class. Not intended for direct use.', 
                  version='default', 
                  skeleton='default', 
                  name='default',
                  config=TransformConfig('defaultConfig'),
                  lastInChain = True ):
        # set filename of python file that defines the transformation
        base = self.__module__.split('.')[-1]
        if base == '__main__':
            #Get a list of frames
            framelist = inspect.getouterframes(sys._getframe())
            nframes = len(framelist)
            #Last frame in the list should be the toplevel trf 
            #frame is an python tuple object, 2nd item [1] is filename
            filename = os.path.basename(framelist[nframes-1][1])
            # make sure the extension is .py (and not .pyc or .pyo)
            base,ext = os.path.splitext(filename)
            del filename
        ## Transform file name. Accessed via filename().
        self._filename = base + '.py'
        # set the name of the transformation
        if name == 'default':
            # start from filename where constructor is called
            name = os.path.splitext(self._filename)[0]
            # remove default trf name suffix
            name = strip_suffix( name, JobTransform._fileSuffix )
        ## Name of the transform. Accessed via name().
        self._name = name
        # Install new signal handler.
        setTrfSignalHandler( self._completeReport )
        ## @var _skeleton 
        #  Skeleton (top-level) job option for the transforms. Accessed via skeletonFilename().
        if skeleton == 'default':
            self._skeleton = self.defaultSkeletonFilename()
        elif skeleton == '':
            self._skeleton = None
        else:
            self._skeleton = skeleton
        # set the version. Default is svn tag as found in file cmt/version.cmt
        if version == 'default':
            version = 'unknown'
            framelist = inspect.getouterframes(sys._getframe())
            nframes = len(framelist)
            #Last frame in the list should be the toplevel trf 
            #frame is an python tuple object, 2nd item [1] is filename
            filename = os.path.basename(framelist[nframes-1][1])
            dirname = os.path.dirname( filename )
            packroot = os.path.dirname( dirname )
            cmtversion = os.path.join(packroot,'cmt','version.cmt')
            try:
                with open( cmtversion ) as cmtversionfile:
                    for line in cmtversionfile:
                        _v = line.strip()
                        if _v:
                            version = _v
                            break
            except Exception:
                pass
        ## Specify if this is the last trf in a chained series of trfs.
        #  This allows certain processes to be omitted/added depending on it's position in the trf chain. 
        self._lastInChain = lastInChain
        ## Transform version determined using @c cmt/version.cmt. 
        #  @see Accessed via version().
        self._version = version
        ## Transform configuration.
        #  @see Accessed via runConfig().
        self._config = config
        ## Handle to application (athena.py) subprocess.
        self._runJobProcess = None
        ## Associate a empty jobReport with the transform.
        #  @see Accessed via jobReport().
        self._jobReport = JobReport()
        # Name and set the version (based on the current transform) of the associated job report.
        self._jobReport.setProducer(self.name(), self.version())
        ## List of areas to be included in the job report.
        self._jobReportOptions   = []
        ## List of areas not to be included in the job report.
        self._jobReportNotOptions = []
        ## Information (limited by jobReportOptions) printed at start of the jobReport.
        self._startReportOptions = ['Environment','Machine','AtlasRelease','DBRelease']
        ## Information (extending on jobReportNotOptions) omitted at start of the jobReport.
        self._startReportNotOptions = []
        ## Information (limited by jobReportOptions) printed at end of the jobReport.
        self._endReportOptions   = []
        ## Information (extending on jobReportNotOptions) omitted at end of the jobReport.
        self._endReportNotOptions = ['Environment','Machine','RunDirListing']
        # Initialise parent class
        TransformLogger.__init__(self,name)
        # Set logger
        trferr.errorHandler.setLoggerParentName( self.loggerName() )
        ## Handle to the log file associated with this transform.
        self._logFile = None
        ## Name of the log file.
        #  @see Accessed via logFilename().
        self._logFilename = name + '.log'
        ## Name of the file used to store the stderr written out by the transform.
        self._stderrFilename = name + '.stderr'
        ## Name of the file used to store the stdout written out by the transform.
        self._stdoutFilename = name + '.stdout'
        ## List of loggers associated with the transform.
        self._loggers = []
        ## Help text.
        #  @see Accessed via getHelp().
        self._help    = help
        ## List of authors (trfutil.Author instances) associated with the transform.
        #  @see Append using addAuthors().
        self._authors = []
        ## Minimum events permitted. Used in validation checks.
        #  @see Use setMinMaxEvents() to set new values.
        self._minEvents = 0
        ## Maximum events permitted. Used in validation checks.
        #  @see Use setMinMaxEvents() to set new values.
        self._maxEvents = 0
        ## Used to determine the error filters to apply when parsing log file. Value used in job report.
        self._atlasrelease = None
        ## Used in mechanism to determine DBRELEASE. Value used in job report.
        self._dbrelease = None
        ## List of options to be passed to @c athena.py.
        #  @see Append using addAthenaOption().
        self._athenaOptions = []
        ## List of non-athena.py options. Used to pass options to the athena_wrapper.py script.
        self._genericOptions = []
        ## Pre-execute one-liner to be passed to athena.py's @c -c parameter.
        #  @see Set using setPECommand().
        self._PECommand = ''
        ## True when Monte Carlo data is used as input. Enables routines specific to MC input data.
        #  @see Set using enableMCInput().
        self._mcInput = None
        ## True when SQlite is to be used rather than ORACLE. 
        #  @see Set using enableSQLite().
        self._useSQLite = None
        ## Set to true if random throw gives a number smaller than the value of input argument --uploadtoami
        self._exportToAmi = False
        ## Dictionary of key/values of arguments.  
        self._namedArgs = {}
        ## List of arguments in strict order.
        #  @see Accessed via argumentList().
        self._positionalArgs = []
        ## Dictionary of arguments that have default values.
        self._optionalArgs = {}
        ## Dictionary of arguments that do not have default values.
        self._requiredArgs = {}
        ## A empty runargs.RunArguments instance that will hold run arguments as named members with values. 
        #  @see Accessed via runArgs().
        self._runArgs = RunArguments()
        ## List of basic_trfarg.OutputFileArg instances.
        self._outputFiles = []
        ## List of basic_trfarg.InputFileArg instances.
        self._inputFiles = []
        ## List of pre-run actions (primarily) from arguments added.
        self._preRunActions = []
        ## List of post-run actions (primarily) from arguments added.
        self._postRunActions = []
        ## List of job options that will be prepended ahead of the skeleton job option.
        self._preJobOptionsFiles = []
        ## List of job options that will be appended behind the skeleton job options.
        self._postJobOptionsFiles = []
        # store the various jobOptions templates
        ## Template for author details
        self._authorsTemplate = ''
        ## Template for run arguments header
        self._runArgsHeaderTemplate = ''
        ## Template for run arguments code
        self._runArgsCodeTemplate = ''
        ## Template for run arguments header
        self._runArgsArgumentsTemplate = ''
        ## Template for skeleton job option header. 
        #  @warning Used by automatic skeleton job option generator which has not been implemented.
        self._skeletonHeaderTemplate = ''
        ## Template for skeleton job option code
        #  @warning Used by automatic skeleton job option generator which has not been implemented.
        self._skeletonCodeTemplate = ''
        ## Template for the skeletion job option
        #  @warning Used by automatic skeleton job option generator which has not been implemented.
        self._skeletonTemplate = ''
        # set and initialise command line options
        ## Store all available command line options.
        #  @see Accessed via runOptions().
        self._runOpts = RunOptions()
        # Assign actions to command line options.
        runOpts = self._runOpts
        runOpts.getProperty('help').setActionFunction( self.printHelpAndExit )
        runOpts.getProperty('athenaopts').setActionFunction( self.addAthenaOption )
        runOpts.getProperty('argdict').setActionFunction( self.readArgsFromPickle )
        runOpts.getProperty('test').setActionFunction( self.enableTest )
        runOpts.getProperty('command').setActionFunction( self.setPECommand )
        runOpts.getProperty('mcinput').setActionFunction( self.enableMCInput )
        runOpts.getProperty('usesqlite').setActionFunction( self.enableSQLite )
        runOpts.getProperty('ignoreerrors').setActionFunction( self.enableMaskAllErrors )
        runOpts.getProperty('ignoreunknown').setActionFunction( self.enableIgnoreUnknownErrors )
        runOpts.getProperty('ignoreall').setActionFunction( self.enableIgnoreAllErrors )
        runOpts.getProperty('tcmalloc').setActionFunction( self.enableTCMalloc )
        runOpts.getProperty('leakcheckexecute').setActionFunction( self.enableLeakCheckExecute )
        runOpts.getProperty('rss').setActionFunction( self.setRSS )
        runOpts.getProperty('vmem').setActionFunction( self.setVMEM )
        runOpts.getProperty('extrametadatadict').setActionFunction( self.updateExtrasMetadataDict )
        runOpts.getProperty('extraignorefilters').setActionFunction( self.updateIgnoreFilters )
        runOpts.getProperty('omitvalidation').setActionFunction( self.omitValidation )
        runOpts.getProperty('usenewmeta').setActionFunction( self.useNewMeta ) # Experimental
        runOpts.getProperty('uploadtoami').setActionFunction( self.uploadToAMI ) 
#        runOpts.processOptions()
        ## Dictionary of options (full_trfarg.OptionArg instances) as denoted by the @em _optionArgNames class variable.
        self._optionArgs = {}
        for opt in JobTransform._optionArgNames:
            arg = OptionArg(opt,runOpts)
            self._optionArgs[opt.lower()] = arg
            self._addLogger(arg)
        self.addAuthors( authors )
        # define runArgsHeaderTemplate contents
        self.addTemplates( 'runArgsHeader',
                           "# Run arguments file auto-generated on %s by:" % (time.ctime()) ,
                           "# JobTransform: %s" % (name) ,
                           "# Version: %s" % (version) )
        # define skeletonHeaderTemplate contents
        self.addTemplates( 'skeletonHeader',
                           "# JobOptions skeleton file auto-generated on %s by:" % (time.ctime()) ,
                           "# JobTransform: %s" % (name) ,
                           "# Version: %s" % (version) )
        # define skeletonCodeTemplate contents
        self.addTemplates( 'skeletonCode', "import os,sys" )
        # define runArgsCodeTemplate contents
        self.addTemplates( 'runArgsCode',
                           "from PyJobTransformsCore.runargs import RunArguments",
                           "",
                           "%s = RunArguments()" % self.runArgsName() )

    ## @brief Add an new error to the list of errors in the associated job report.
    #  @details An instance of AtlasErrorCodes.ErrorInfo is created and added to list of errors maintained by the transform's job report.
    #  @param acronym Acronym to assign to the error. Defaults to @b ATH_FAILURE. 
    #  Refer to @c share/atlas_error_categories.db for valid acronyms.
    #  @param severity Assign a severity level for the error. Defaults to @b FATAL.
    #  Refer to severity levels defined in @c AtlasErrorCodes.py.
    #  @param **kwargs Optional dictionary of any other parameters that may be recognised by @c AtlasErrorCodes.ErrorInfo.
    #  @return The AtlasErrorCodes.ErrorInfo instance created.
    def addError( self, acronym = "ATH_FAILURE", severity = AtlasErrorCodes.FATAL, **kwargs ):
        e = AtlasErrorCodes.ErrorInfo( acronym = acronym, severity = severity, **kwargs )
        self._jobReport.addError( e )
        return e

    ## @brief Add a new error to the list of validation errors in the associated job report.
    #  @details An instance of AtlasErrorCodes.ErrorInfo is created and added to list of validation 
    #  errors maintained by the transform's job report.
    #  @param e An instance of trferr.TransformValidationError exception.
    #  @return The AtlasErrorCodes.ErrorInfo instance created.
    def addValidationError( self, e ):
        ve = AtlasErrorCodes.ErrorInfo( acronym = e.error, severity = AtlasErrorCodes.FATAL, message = e.message, **e.extras )
        self._jobReport.addValidationError( ve )
        return ve

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
    def _completeReport( self, signum, frame ):
        if self._runJobProcess is not None:
            print ("Signal handler: Killing %s with %s." % ( self._runJobProcess.pid, signal.SIGTERM ))
            os.kill( self._runJobProcess.pid, signal.SIGTERM )
        # Restoring signal handlers.
        setDefaultSignalHandlers()
        self.addError( acronym = 'ATH_FAILURE', severity = AtlasErrorCodes.FATAL, message = 'Signal %s received.' % signum )
        while self._runJobProcess and self._runJobProcess.poll() is None:
            try:
                line = self._runJobProcess.stdout.readline() or ''
            except IOError:
                break
            if line and not self._logFile.closed:
                self._logFile.write(line)
        try:
            self._logFile.close()
        except Exception:
            pass
        try:
            rc = self._runJobProcess.returncode
        except Exception:
            rc = None
        if rc < 0 or rc is None:
            os.system( 'dmesg > dmesg_trf.txt' )
            rc = 8 #ExitCodes---> 8: an unknown exception occurred
        print ("Signal handler: athCode=%s" % rc)
        print ("Signal handler: athAcronym=%s" % str(ExitCodes.what(rc) ))
        # overwrite producer for new errors that are added
        self._jobReport.setProducer(self.name(),self.version())
        # adding the exit status from athena        
        self._jobReport.addInfo( JobInfo('athCode', str(rc) ) )
        self._jobReport.addInfo( JobInfo('athAcronym', str(ExitCodes.what(rc) ) ) )
        # Alter behaviour of athenaReport depending on the following flags.
        self._jobReport.setIgnoreUnknown( self._ignoreUnknown )
        self._jobReport.setIgnoreAll( self._ignoreAll )
        self._jobReport.setIgnoreErrors( self._ignoreErrors )
        self.dumpReport(self._endReportOptions,self._endReportNotOptions)
        self.writeReports()
        ec = self._jobReport.exitCode()
        print ("Signal handler: Raising SystemExit with exit code %s." % ec)
        raise SystemExit( ec )

    ## @brief Default skeleton job options file name.
    #  @details The filename is based on the name of the transform i.e. <tt> skeleton.[TRANSFORM_NAME].py </tt>
    #  @return String
    def defaultSkeletonFilename(self):
        return 'skeleton.' + self._name + '.py'

    ## Getter function for skeleton job options file name.
    #  @see _skeleton attribute.
    #  @return String
    def skeletonFilename(self):
        return self._skeleton

    ## Check if the skeleton job option has been set.
    #  @return Boolean 
    def hasSkeleton(self):
        return self._skeleton is not None

    ## @brief Getter function for current log file name.
    #  @see _logFilename attribute.
    #  @return String
    def logFilename(self):
        return self._logFilename

    ## @brief File name of file containing run arguments used.
    #  @details The filename is based on the name of the transform i.e. <tt> runargs.[TRANSFORM_NAME].py </tt>
    #  @return String
    def runArgsFilename(self):
        return 'runargs.' + self._name + '.py'

    ## Getter function for name of runargs.RunArguments instance.
    #  @see _runArgsName class attribute.
    #  @return String
    def runArgsName(self):
        return JobTransform._runArgsName

    ## Getter function for object containing the various run arguments associated with the transform.
    #  @see _runArgs attribute.
    #  @return runargs.RunArguments instance
    def runArgs(self):
        return self._runArgs

    ## Getter function for object containing run options associated with the transform.
    #  @see _runOpts attribute.
    #  @return runargs.RunOptions instance
    def runOptions(self):
        return self._runOpts

    ## Getter function for version associated with the transform.
    #  @see _version attribute.
    #  @return String
    def version(self):
        return self._version

    ## Getter function for object containing the properties of the transform.
    #  @see _config attribute.
    #  @return TransformConfig.TransformConfig instance
    def runConfig(self):
        return self._config

    ## Getter function for the output files associated with the transform.
    #  @see _outputFiles attribute.
    #  @return List of basic_trfarg.OutputFileArg instances.
    def outputFiles(self):
        return self._outputFiles
    
    ## Getter function for the input files associated with the transform.
    #  @see _inputFiles attribute.
    #  @return List of basic_trfarg.InputFileArg instances.
    def inputFiles(self):
        return self._inputFiles
    
    ## Getter function for the job report associated with the transform.
    #  @see _jobReport attribute.
    #  @return JobReport.JobReport instance.
    def jobReport(self):
        return self._jobReport

    ## Getter function for the maximum number of output events.
    #  @see _maxEvents attribute.
    #  @return Integer.
    def maxEvents(self):
        return self._maxEvents

    ## Getter function for minimum number of input events.
    #  @see _minEvents attribute.
    #  @return Integer.
    def minEvents(self):
        return self._minEvents
    
    ## @brief Customise printout for job report.
    #  @param options List of items (or a comma-delimited string of items) to print. Defaults to @b None.
    #  @param notOptions List of items (or a comma-delimited string of items) to omit. Defaults to @b None.
    #  @warning @a notOptions items take precedence over @a options items.
    #  @see _jobReportOptions and _jobReportNotOptions attributes.
    #  @return None
    #  @remarks allowed values for @a options and @a notOptions are: 
    #  @li @c '' (empty string) - print everything
    #  @li @c 'Summary' - print the Summary section
    #  @li @c 'Errors'  - print the Error section ( @c FATALs, @c ERRORs and @c WARNINGs)
    #  @li @c 'Environment' - print all shell environment variables
    #  @li @c 'Machine' - print the machine section (CPU, binary version etc.)
    #  @li @c 'AtlasRelease' - print the Atlas release version number
    #  @li @c 'DBRelease' - print database release version number
    #  @li @b [ITEM] - print information on @b [ITEM] 
    def setJobReportOptions(self,options=None,notOptions=None):
        if options is not None:
            # convert string to list
            if type(options).__name__ == 'str':
                self._jobReportOptions = options.split(',')
            else:
                self._jobReportOptions = copy(options)
        if notOptions is not None:
            # convert string to list
            if type(notOptions).__name__ == 'str':
                self._jobReportNotOptions = notOptions.split(',')
            else:
                self._jobReportNotOptions = copy(notOptions)

    ## @brief Print a customised version of the job report based on the @a options and @a notOptions to @a output.
    #  @param options List of items (or a comma-delimited string of items) to print. Defaults to @b None.
    #  @param notOptions List of items (or a comma-delimited string of items) to omit. Defaults to @b None.
    #  @param output File-like object. Defaults to @em sys.stdout.
    #  @see setJobReportOptions() remarks for permitted values for @a options and @a notOptions.
    #  @return None
    def dumpReport(self,options=None,notOptions=None,output=sys.stdout):
        # options it the AND or stored and requested options
        if not options: # catches None, '' and []
            # use default
            dumpOpts = self._jobReportOptions
        else:
            # convert string to list
            if type(options).__name__ == 'str': options = options.split(',')
            if not self._jobReportOptions:
                # allow all
                dumpOpts = options
            elif 'None' in self._jobReportOptions:
                # print nothing
                return  # no point in doing anything else
                dumpOpts = ['None']
            else:
                dumpOpts = [ opt for opt in options if opt in self._jobReportOptions ]
                # if no overlap, print nothing
                if not dumpOpts:
                    return  # no point in doing anything else
                    dumpOpts = ['None']
        # notOptions is the OR of stored and requested options
        if notOptions is None:
            # use default
            dumpNotOpts = self._jobReportNotOptions
        else:
            if type(notOptions).__name__ == 'str': notOptions = notOptions.split(',')
            dumpNotOpts = notOptions
            for opt in self._jobReportNotOptions:
                if opt not in dumpNotOpts: dumpNotOpts.append( opt )
        self._jobReport.dump( dumpOpts, dumpNotOpts, output )

    ## @brief Top-level job report writer.
    #  @details Write various job reports to file, depending on the error code.
    #  @warning All reports apart from the metadata in XML format (@em metadata.xml) are written out 
    #  if the transform exitcode is not 0 (SUCCESS).
    #  @see JobReport.JobReport.write(), JobReport.JobReport.writeAll(), JobReport.JobReport.writeTXT(), 
    #  JobReport.JobReport.writeJobInfoXML() and JobReport.JobReport.writeGPickle() functions.
    #  @return None
    def writeReports(self):
        if self._jobReport.exitCode() == 0:
            self._jobReport.writeAll( writeFinalCopy = self._lastInChain )
        else:
            self._jobReport.write( writeFinalCopy = self._lastInChain )
            self._jobReport.writeTXT( writeFinalCopy = self._lastInChain )
            self._jobReport.writeJobInfoXML( writeFinalCopy = self._lastInChain )
            self._jobReport.writeGPickle( writeFinalCopy = self._lastInChain ) 
            # do not write metadata.xml

    ## @brief Setter function for the minimum and maximum number of output events.
    #  @param minEvents Minimal number of output events. If @b None is given, there is no change. 
    #  If set to @b 0, minimum event checking is disabled. 
    #  @param maxEvents Maximum number of output events. Defaults to @b None (i.e. no change).
    #  If set to @b 0, maximum event checking is disabled.
    #  @see _maxEvents and _minEvents attributes.
    #  @return None
    def setMinMaxEvents(self,minEvents,maxEvents=None):
        try:
            if minEvents < 0 or maxEvents < 0 or maxEvents < minEvents:
                raise Exception
        except Exception:
            self.logger().info( "%s should be greater or equal to %s", maxEvents, minEvents )
            return
        self._minEvents = minEvents
        self.logger().info("Setting minimum number of output events to %d", minEvents)
        self._maxEvents = maxEvents
        self.logger().info("Setting maximum number of output events to %d", maxEvents)

    ## @brief Ensure that the exit code is not affected by the presence of unknown errors.
    #  @details Aligns the transform exit code with that of @c athena.py (@em only if the latter was successful) 
    #  even if errors were detected by the transform.
    #  @warning Deprecated. Use enableMaskAllErrors() instead.
    #  @see _ignoreUnknown and _ignoreErrors attributes.
    #  @return None
    def enableIgnoreUnknownErrors( self ):
        self._ignoreUnknown = True
        self._ignoreErrors = True
        self.logger().warning( "--ignoreunknown is deprecated. Using --ignoreerrors=True instead." )

    ## @brief Ignore @em all errors during execution of the transform.
    #  @details Allow transform to proceed as far as possible. All validation routines are omitted as well.
    #  @see _ignoreAll attribute.
    #  @return None
    def enableIgnoreAllErrors( self ):
        VALIDATION_DICT[ 'ALL'] = False # remove all validation
        self._ignoreAll = True

    ## @brief Enable the tcmalloc memory allocation mechanism.
    #  @warning Deprecated. A default memory allocation mechamism is no longer set. 
    #  The default should be (and is now) dictated solely by the application (i.e. @c athena.py).
    #  @return None
    def enableTCMalloc( self ):
        self.logger().warning( "tcmalloc is the current default. Ignoring --tcmalloc option." )

    ## @brief Add the @c --leakcheckexecute option to @c athena.py 
    #  @details Ignored if @c --tcmalloc option found in the @em _athenaOptions list.
    #  @see addAthenaOption() function used to set the @c --leak-check-execute option.
    #  @return None  
    def enableLeakCheckExecute( self ):
        if "--tcmalloc" in self._athenaOptions:
            self.logger().warning( "--leakcheckexecute cannot be used with --tcmalloc. Ignoring --leakcheckexecute option." )
        else:
            self.addAthenaOption( "--leak-check-execute" )

    ## @brief Set the RSS memory limit.
    #  @param val The value to set the RSS memory limit to.
    #  @see _genericOptions attribute which will contain the RSS option.
    #  @return None
    def setRSS( self, val ):
        if val is None:
            return
        self._genericOptions.append( "--rss=%s" % val )

    ## @brief Set the VMEM limit.
    #  @param val The value to set the VMEM limit to.
    #  @see _genericOptions attribute which will contain the VMEM option.
    #  @return None
    def setVMEM( self, val ):
        if val is None:
            return
        self._genericOptions.append( "--vmem=%s" % val )

    ## @brief Include more metadata to retrieve from the log file created.
    #  @details Callback function associated with the command line option @c --extrametadatadict.
    #  Add user-defined dictionary entries to existing @em _extraMetadataDict dictionary.
    #  @see JobReport._extraMetadataDict attribute.
    #  @remarks file provided must contain the _extraMetadataDict dictionary. Refer to extraMetadata.py.
    #  @return None
    def updateExtrasMetadataDict( self, fname ):
        try:
            execfile( fname )
        except IOError:
            self.logger().warning( "Error reading file %s containing extra metadata.", fname )
        else:
            _extraMetadataDict.update( locals()['extraMetadataDict'] )

    ## @brief Set transform to run in test mode.
    #  @details Use with @c --omitvalidation option to omit specified checks from all areas (i.e. not just within validation routines). 
    #  @see omitValidation() function and trfutil.TRF_SETTING dictionary.
    #  @return None
    def enableTest( self ):
        self.logger().info( 'Running in test mode.' )
        TRF_SETTING[ 'testrun' ] = True
        TRF_SETTING[ 'validationRetry' ] = 0
        TRF_SETTING[ 'TRFRetry' ] = 0

    ## @brief Set the pre-execute command string.
    #  @details Setter function for the @em _PECommand attribute. The string of code is executed before athena.py is run.
    #  @param peCommandStr This string is passed to the @c athena.py @c -c option.
    #  @see _PECommand attribute.
    #  @return None
    def setPECommand( self, peCommandStr ):
        self._PECommand = peCommandStr

    ## Setter function that allows the transform to execute routines based on the assumption that Monte Carlo inputs are used.
    #  @param val A boolean value is expected.
    #  @see _mcInput attribute.
    #  @return None
    def enableMCInput( self, val ):
        self._mcInput = val

    ## Setter function to force the transform to access the SQLite database (rather than the ORACLE database).
    #  @param val A boolean value is expected.
    #  @see _useSQLite attribute.
    #  @return None
    def enableSQLite( self, val ):
        self._useSQLite = val

    ## @brief Allow various validation routines to be omitted.
    #  @param omitStr Valid strings are based on the keys of the trfutil.VALIDATION_DICT dictionary. Multiple values can be comma-delimited (without spaces). Use 'ALL' to omit all validation routines, use 'NONE' to remove all validation ommissions.
    #  @see trfutil.VALIDATION_DICT dictionary.
    #  @return None
    def omitValidation( self, omitStr ):
        omitList = omitStr.split( ',' )
        if 'NONE' in omitList: # remove all validation ommissions
            for oEntry in VALIDATION_DICT:
                VALIDATION_DICT[ oEntry ] = None
        else:
            for oEntry in omitList:
                VALIDATION_DICT[ oEntry ] = False

    def uploadToAMI(self, percentage):
        import random
        random.seed()
        if random.random() < float(percentage):
            self._exportToAmi = True
            print ('trf.py will send to ami. ')
        else:
            self._exportToAmi = False
            print ('trf.py will NOT send to ami. ')
    
    
    ## @brief Ensure that the exit code is not affected by the presence of unknown errors.
    #  @details Aligns the transform exit code with that of @c athena.py (@em only if the latter was successful) 
    #  even if errors were detected by the transform.
    #  @param val Boolean value expected. For backward-compatibility, also accepts 'ALL' as a synonym for 'True'.
    #  @see _ignoreUnknown and _ignoreErrors attributes and the JobReport.JobReport.exitCode() function.
    #  @remarks All masked errors are still recorded, just not taken into account when generating the exit code.
    #  @return None
    def enableMaskAllErrors( self, val ):
        try:
            val = val.upper()
        except Exception:
            pass
        else:
            if val == 'ALL':
                val = True
            else:
                val = False
                self.logger().info( "Unknown value. --ignoreerrors=[True|False]. Setting to False." )
        self._ignoreErrors = val or self._ignoreUnknown
        if self._ignoreErrors:
            self._ignoreUnknown = True

    ## Add user-defined ignore error filters.
    #  @param fname Name of file containing the filter. The file format must be the same as @c share/atlas_error_ignore.db.
    #  @see share/atlas_error_ignore.db.
    #  @return None
    def updateIgnoreFilters( self, fname ):
        AtlasErrorCodes.readIgnorePatterns() # read default file in release
        AtlasErrorCodes.readIgnorePatterns( fname ) # add user file

    ## Enable new format for jobReport.gpickle and metadata.xml.
    #  @param val Boolean value expected.
    #  @see JobReport.JobReport.useNewMetadataFormat attribute.
    #  @return None
    def useNewMeta( self, val ):
        self._jobReport.useNewMetadataFormat = val

    ## @brief Set the Athena message level and Python logging level.
    #  @details Value for level has to be one of "ALL", "VERBOSE", "INFO", "WARNING", "ERROR", "DEBUG", "FATAL".
    #  @param level Logging level as a String
    #  @warning Deprecated.
    def _setMessageLevel(self,level):
        if level is None:
            return
        if ( level in [ "ALL", "VERBOSE", "INFO", "WARNING", "ERROR", "DEBUG", "FATAL" ] ):
            TransformLogger.setLoggerLevel(self,level)
            self.addAthenaOption( "-l %s" % level )

    ## Setter function for job transform parent.
    #  @param parentTrf Job transform parent instance to be associated with.
    #  @remarks The tranform inherits particular runtime options (as specified by @em _sharedRunOpts) from its parent.
    #  @see _sharedRunOpts attribute.
    #  @warning Not sure of the purpose of this function since sub-transforms in composite transforms 
    #  are given a full set of relevant options. This function may be removed in the future.
    #  @return None
    def setParent(self,parentTrf):
        # transfer shared command line options from parent to child
        parentOpts = parentTrf.runOptions()
        for n in self._sharedRunOpts:
            try:
                setattr(self._runOpts,n,getattr(parentOpts,n))
            except Exception:
                pass

    ## Add or replace options at the command line for @c athena.py
    #  @param option A single string of all command line options to be passed to @c athena.py. Multiple options should be space-delimited.
    #  @remarks If @a option is already present in the athena options list, its value will be overwritten.
    #  @see _athenaOptions attribute.
    #  @return None
    def addAthenaOption(self,option):
        option = option.strip()
        if not option:
            return
        if option in ( '--stdcmalloc', '--leak-check-execute' ):
            try:
                self._athenaOptions.remove( '--tcmalloc' )
            except ValueError:
                pass
            else:
                self.logger().warning( '%s cannot be used with the default --tcmalloc option. --tcmalloc option removed.', option )
        # remove old option if needed
        firstWord = option.split()[0]
        for opt in self._athenaOptions:
            if opt.split()[0] == firstWord:
                self._athenaOptions.remove(opt)
                break
        # add it to the list
        self._athenaOptions.append( option )
        
    ## Reset skeleton template 
    #  @remarks This function serves to invalidate the existing skeleton template forcing it to be 
    #  recreated on demand to ensure the latest version is used.
    #  @see _skeletonTemplate attribute.
    #  @return None
    def updateFullTemplates(self):
        # Invalidate full templates, so they get recreated when needed
        self._skeletonTemplate = ''

    ## Adds template entries to an existing template.
    #  @param name Name of template to add new template entry strings to.
    #  @param *vargs List of template entry strings to be added.
    #  @remarks Each time a template is added/updated, updateFullTemplates() is called to ensure that 
    #  the top level template (i.e. @em _skeletonTemplate) is refreshed.
    #  @see updateFullTemplates() function.
    #  @return None  
    def addTemplates(self, name, *vargs):
        # count the number of non-None arguments
        name = '_%sTemplate' % (name)
        val = getattr(self, name)
        for arg in vargs: val += str(arg) + os.linesep
        setattr(self, name, val)
        self.updateFullTemplates()

    ## Associates authors to the transform
    #  @param authorList Python list of trfutil.Author instances. 
    #  Other values accepted include a single trfutil.Author instance and a comma-delimited string of author names.
    #  @exception trferr.TransformDefinitionError is raised when @em authorList is not recognised.
    #  @return None
    def addAuthors(self, authorList):
        if not authorList: return
        # make sure I got a list
        authorsAdd = authorList
        if isinstance(authorList,Author):
            authorsAdd = [ authorList ]
        else:
            if isinstance(authorList, str):
                authorsAdd = [ Author(a) for a in authorList.split(',') ]
            elif isinstance(authorList, list):
                pass
            else:
                raise TransformDefinitionError('Author type %s not supported' % type(authorList))
        self._authors += authorsAdd
        # update authors template
        authStrings = [ str(a) for a in authorsAdd ]
        firstLine  = "# Authors: "
        otherLines = "#" + ' '*(len(firstLine)-1)
        sep = os.linesep + otherLines
        i = 0
        if not self._authorsTemplate:
            self.addTemplates( 'authors', "%s%s" % (firstLine,authStrings[i]) )
            i += 1
        if i < len(authorsAdd):
            self.addTemplates( 'authors', "%s%s" % (otherLines,sep.join(authStrings[i:])) )

    ## Add a command line argument definition.
    #  @param arg An Argument class (or any of its sub-classes) instance.
    #  @param default Assign a default value to the argument to be added. 
    #  A @em None value makes the argument @em optional. Defaults to @em None.
    #  @remarks Arguments added are assigned positional numbers (for command line invocation)
    #  in the order they were added.
    #  @warning This function is not designed to be used directly. Use add() instead.
    #  @warning Add all @em non-optional arguments before @em optional ones.
    #  @exception trferr.TransformDefinitionError is raised when the argument is not @em fully-specified 
    #  (i.e. a intermediate class of Argument or even the Argument class itself that is not designed to 
    #  be instantiated.).
    #  @exception trferr.TransformDefinitionError is raised if an @em optional argument (i.e. an argument 
    #  that does not require a value as it has been assigned a default one) is proceeded by a @em non-optional argument.
    #  @exception trferr.TransformArgumentError is raised when a duplicate argument is given.
    #  @return None
    def _addArgument(self, arg, default=None):
        # ensure usage of fully specified arguments
        if not arg.isFullArgument():
            raise TransformDefinitionError( 'Class %s is not a fully specified Argument class.' % arg.__class__.__name__ )
        name = arg.name()
        key = name.lower()
        #check that I have not two arguments with the same name
        if key in self._namedArgs:
            raise TransformArgumentError( 'Duplicate argument name: %s' % name )
        # add to named and positional lists
        self._namedArgs[key] = arg
        self._positionalArgs.append( arg )
        arg.setPosition( len(self._positionalArgs) )
        # handle default
        if default is not None:
            arg.setDefault(default)
            self.setArgument(key,default)
            # argument is optional
            self._optionalArgs[key] = arg
        if not arg.hasDefault():
            # argument in not optional
            # check that all optional arguments are at the end
            if len(self._optionalArgs) > 0:
                raise TransformDefinitionError( 'Non-optional argument (%s) after optional argument(s)' % name )
            self._requiredArgs[key] = arg

    ## Add arguments with the preRunAction attribute.
    #  @param arg basic_trfarg.Argument instance.
    #  @param prepend If true, add action to the start of the list, instead of appending it (default false)
    #  @warning This function is not designed to be used directly.
    #  @return None
    def _addPreRunAction( self, arg, prepend = False ):
        if prepend:
            self._preRunActions.insert(0, arg)
        else:
            self._preRunActions.append( arg )

    ## Add arguments with the postRunAction attribute.
    #  @param arg basic_trfarg.Argument instance.
    #  @param prepend If true, add action to the start of the list, instead of appending it (default false)
    #  @warning This function is not designed to be used directly.
    #  @return None
    def _addPostRunAction( self, arg, prepend = False ):
        if prepend:
            self._postRunActions.insert(0, arg)
        else:
            self._postRunActions.append( arg )

    ## Add a PreJobOptionsFile argument.
    #  @param jo trfutil.PreJobOptionsFile instance.
    #  @warning This function is not designed to be used directly. Use add() instead.
    #  @return None
    def _addPreJobOptionsFile( self, jo ):
        self._preJobOptionsFiles.append( jo )

    ## Add a PostJobOptionsFile argument.
    #  @param jo trfutil.PostJobOptionsFile instance.
    #  @warning This function is not designed to be used directly. Use add() instead.
    #  @return None
    def _addPostJobOptionsFile( self, jo ):
        self._postJobOptionsFiles.append( jo )

    ## Add a TransformLogger argument.
    #  @param log TransformLogger instance.
    #  @warning This function is not designed to be used directly. Use add() instead.
    #  @return None
    def _addLogger( self, log ):
        log.setLoggerParentName( self.loggerName() )
        self._loggers.append( log )

    ## Add an output file argument.
    #  @param outfile An basic_trfarg.OutputFileArg instance.
    #  @warning This function is not designed to be used directly. Use add() instead.
    #  @return None
    def _addOutputFile(self,outfile):
        if outfile not in self._outputFiles: 
            self._outputFiles.append( outfile )

    ## Add an input file argument.
    #  @param infile An basic_trfarg.InputFileArg instance.
    #  @warning This function is not designed to be used directly. Use add() instead.
    #  @return None
    def _addInputFile(self,infile):
        if infile not in self._inputFiles: 
            self._inputFiles.append( infile )

    ## Add an argument as part of the transform customisation procedure.
    #  @param arg1 basic_trfarg.Argument instance to be added.
    #  @param *vargs Optional list of parameter values specific to the argument to be added.
    #  @param **kwargs Optional dictionary of parameter key/value pairs specific to the argument to be added.
    #  @exception trferr.TransformDefinitionError is raised if @em arg1 is an full_trfarg.OptionArg not recognised by the transform.
    #  @warning Optional parameters @em vargs and/or @em kwargs will be passed directly to corresponding argument 
    #  creating functions WITHOUT FURTHER CHECKS so an exception will be raised if an unrecognised parameter is used.
    #  @return None
    def add(self, arg1, *vargs, **kwargs):
        # substitute OptionArg (placeholder argument) by the real option
        if isinstance(arg1,OptionArg):  
            try:
                arg1 = self._optionArgs[arg1.name()]
            except KeyError:
                raise TransformDefinitionError( "Option \"%s\" not supported (requested with OptionArg)" % (arg1.name()) )
        # add the config object to the config argument
        if isinstance(arg1,JobConfigArg): arg1.setConfig(self._config)
        # add object to various places
        if isinstance(arg1,Argument):
            self._addArgument(arg1,*vargs,**kwargs)
            if arg1.metaType().startswith('output'): self._addOutputFile( arg1 )
            if arg1.metaType().startswith('input') and arg1.name().startswith('input'): self._addInputFile( arg1 )
        if isinstance(arg1,Author): self.addAuthors(arg1,*vargs,**kwargs)
        if hasattr(arg1,'preRunAction'): self._addPreRunAction(arg1)
        if hasattr(arg1,'postRunAction'): self._addPostRunAction(arg1)
        if isinstance(arg1,PreJobOptionsFile): self._addPreJobOptionsFile(arg1)
        if isinstance(arg1,PostJobOptionsFile): self._addPostJobOptionsFile(arg1)
        if isinstance(arg1,TransformLogger): self._addLogger(arg1)

    ## @brief Generate the filename for the job option file responsible for Athena messaging service.
    #  @details The filename is based on the name of the transform i.e. <tt> [TRANSFORM_NAME]_messageSvc_jobOptions.py </tt>
    #  @return String
    def messageSvcFilename(self):
        return self.name() + '_messageSvc_jobOptions.py'

    ## @brief Generate the contents of the Athena messaging service job options file.
    #  @details @em LoggedMessageSvc is the default service. 
    #  The fallback (if the package is not available) is the standard @em MessageSvc.
    #  This function creates the required job options file.
    #  @return None
    def _makeMessageSvcJobOptions(self):
        with open( self.messageSvcFilename(), 'w'  ) as joFile:
            jo = [  CommentLine("Replacing MessageSvc with LoggedMessageSvc").bigComment() ,
                    "try:",
                    "   from AthenaServices import SummarySvc",
                    "except:",
                    "   printfunc ('Could not import AthenaServices.SummarySvc')",
                    "   try:",
                    "      from AthenaCommon.AppMgr import ServiceMgr, theApp",
                    "      import AthenaCommon.ConfigurableDb as ConfDb",
                    "      from AthenaServices.AthenaServicesConf import AthenaSummarySvc",
                    "      from AthenaCommon.OldStyleConfig import Service",
                    "   except:",
                    "      printfunc ('Could not import required modules to enable use of LoggedMessageSvc.')",
                    "   else:",
                    "      printfunc ('Using AthenaServices...')",
                    "      from AthenaServices.AthenaServicesConf import AthenaSummarySvc",
                    "      AthenaSummarySvc.SummaryFile = 'AthenaSummary_%s.txt'" % self.name(),
                    "      theApp.CreateSvc += ['AthenaSummarySvc']",
                    "      AthenaSummarySvc = Service('AthenaSummarySvc')",
                    "      ServiceMgr += AthenaSummarySvc",
                    "      theApp.MessageSvcType = 'LoggedMessageSvc'",
                    "      try:",
                    "         oldMsgSvcName = ServiceMgr.MessageSvc.name()",
                    "      except AttributeError:",
                    "         oldMsgSvcName = 'MessageSvc'",
                    "         oldMsgSvcItems = []",
                    "      else:",
                    "         oldMsgSvcItems = ServiceMgr.MessageSvc.getValuedProperties().items()",
                    "         del ServiceMgr.MessageSvc",
                    "      try:",
                    "         del ServiceMgr.allConfigurables[ oldMsgSvcName ]",
                    "      except KeyError:",
                    "         pass",
                    "      newMsgSvc = ConfDb.getConfigurable( theApp.MessageSvcType )( oldMsgSvcName )",
                    "      for name, value in oldMsgSvcItems:",
                    "         setattr( newMsgSvc, name, value )",
                    "      ServiceMgr += newMsgSvc",
                    "      MessageSvc = ServiceMgr.MessageSvc",
                    "else:",
                    "   printfunc ('Using AthenaServices.SummarySvc...')",
                    "   from AthenaServices.AthenaServicesConf import AthenaSummarySvc",
                    "   AthenaSummarySvc.SummaryFile = 'AthenaSummary_%s.txt'" % self.name(),
                    "   SummarySvc.useAthenaSummarySvc()" ]
            joFile.write( os.linesep.join(jo) + os.linesep )

    ## Setter function for option arguments.
    #  @param optName Name of the option argument concerned. 
    #  @param optValue The value to set the option argument to.
    #  @exception trferr.TransformArgumentError is raised if @em optName is not recognised.
    #  @see _optionArgs attribute.
    #  @return None
    def setOptionArg(self,optName,optValue):
        key=optName.lower()
        try:
            self._optionArgs[key].setValue(optValue)
        except KeyError:
            raise TransformArgumentError( "Option %s not supported" % (optName,) )

    ## Check if @em optName is a recognised option argument.
    #  @param optName Name of the option argument concerned.
    #  @see _optionArgs attribute.
    #  @return Boolean
    def isOptionArg(self,optName):
        return optName.lower() in self._optionArgs

    ## Setter function for pre-defined arguments.
    #  @param argName Name of the argument concerned. 
    #  @param argValue The value to set the argument to.
    #  @exception trferr.TransformArgumentError is raise if @em argName is not recognised.
    #  @remarks Both the associated argument and the corresponding run argument entry is updated.
    #  @see _namedArgs and _runArgs attributes.
    #  @return None
    def setArgument(self,argName,argValue):
        key=argName.lower()
        try:
            arg = self._namedArgs[key]
            arg.setValue(argValue) # get back the python type of the value
            setattr(self._runArgs, arg.name(), arg.value())
        except KeyError:
            raise TransformArgumentError( '%s does not have an argument named %s' % (self.name(), argName) )

    ## @brief List the arguments of this transform.
    #  @details Print list of arguments (in positional order) of this transform instance.
    #  @return None
    def ls(self):
        for arg in self._positionalArgs:
            self.logger().info( '%s=%s # %s', arg.name(), arg.value(), arg.getHelp() )

    ## Getter function for the job transform name.
    #  @see _name attribute.
    #  @return String
    def name(self):
        return self._name

    ## Getter function for the job transform file name.
    #  @see _filename attribute.
    #  @return String
    def filename(self):
        return self._filename

    ## @brief Getter function for run argument template.
    #  @details Run argument template is generated on demand from individual templates 
    #  associated with all arguments of the current transform. 
    #  The final run argument template will also include other miscellaneous templates.
    #  @remarks A <tt> maxEvents = -1 </tt> entry is added to the run argument template 
    #  if the @em maxevents argument was omitted at the command line.
    #  This allows new-style transforms based on Reco_trf.py to optionally specify
    #  the @em maxevents argument.
    #  @see _runArgsArgumentsTemplate attribute.
    #  @return String
    def getRunArgsTemplate(self):
        self._runArgsArgumentsTemplate = ''
        for arg in self._positionalArgs:
            self.addTemplates( 'runArgsArguments',  arg.runArgsComment(),  arg.runArgsTemplate(self.runArgsName()) )
        # This allows Reco_trf.py and related transforms to omit specifying maxEvents argument.
        if 'maxevents' not in self._namedArgs:
            self.addTemplates( 'runArgsArguments',  '\n# Explicitly added. MaxEvents argument not provided at commandline.',  '%s.maxEvents = -1' % self.runArgsName() )
        lines = [ self._runArgsHeaderTemplate,
                  self._authorsTemplate,
                  CommentLine.hashLine(),
                  self._runArgsCodeTemplate,
                  self._runArgsArgumentsTemplate ]
        return os.linesep.join( lines ) 

    ## Getter function for the skeleton template.
    #  @remarks The template is only generated if it does not already exist.
    #  @see _skeletonTemplate attribute
    #  @return String
    def getSkeletonTemplate(self):
        if not self._skeletonTemplate:
            lines = [ self._skeletonHeaderTemplate,
                      self._authorsTemplate,
                      CommentLine.hashLine(),
                      self._skeletonCodeTemplate ]
            self._skeletonTemplate = os.linesep.join( lines ) 
        return self._skeletonTemplate

    ## Getter function for an argument based on its name.
    #  @param name Name of the argument concerned.
    #  @remarks The @em name used is case-insensitive.
    #  @exception trferr.TransformArgumentError is raised if the @em name given is not recognised.
    #  @return basic_trfarg.Argument instance
    def getArgument(self,name):
        key = name.lower()
        try:
            return self._namedArgs[key]
        except KeyError:
            raise TransformArgumentError("JobTransform %s does not have argument %s" % (self._name,name) )

    ## @brief Getter function for argument of a given type.
    #  @details The first argument of which the argumentType() = typename
    #  (i.e. className without the ending 'Arg') is retrieved.
    #  @param typename basic_trfarg.Argument class name without the suffix @em Arg.
    #  @see basic_trfarg.Argument.argumentType() function.
    #  @return basic_trfarg.Argument instance (or None if unsuccessful)
    def getArgumentOfType(self,typename):
        for arg in self._positionalArgs:
            if arg.argumentType() == typename: return arg
        self.logger().warning( "Argument of %s type not found. Returning None.", typename )
        return None

    ## Getter function for the list of arguments in positional order.
    #  @see _positionalArgs attribute.
    #  @return List of basic_trfarg.Argument instances.
    def argumentList(self):
        return self._positionalArgs

    ## Retrieve current arguments as a dictionary with the argument name and its original value as the key and value, respectively.
    #  @return Dictionary <tt> { arg1.name() : arg1.originalValue(), ... } </tt>
    def argumentOriginalDict(self):
        d = { }
        for arg in self._positionalArgs:
            d[arg.name()] = arg.originalValue()
        return d

    ## Retrieve current arguments as a dictionary with the argument name and its current value as the key and value, respectively.
    #  @return Dictionary instance <tt> { arg1.name() : arg1.value(), ... } </tt>
    def argumentValueDict(self): 
        d = { }
        for arg in self._positionalArgs:
            d[arg.name()] = arg.value()
        return d

    ## Write all current run arguments to file
    #  @details The run argument template is populated with @em current argument values and written out to file.
    #  @remarks A copy of the the @em original argument values is also written to a corresponding pickle file.
    #  @return None
    def writeRunArgs(self):
        filename = self.runArgsFilename()
        self.logger().info( 'Writing runArgs to file \"%s\"', filename )
        with open( filename, 'w' ) as f:
            f.write( self.getRunArgsTemplate() % self.argumentValueDict() )
        filename = os.path.splitext(filename)[0] + '.gpickle'
        self.writeArgsToPickle(filename)
        
    ## Write the skeleton job options file
    #  @warning Not in use. Automatically raises trferr.TransformDefinitionError.
    def writeSkeleton(self):
        raise TransformDefinitionError( "Auto-generation of skeleton jobOptions file not yet supported." +
                                        " It must be specified in the constructor of %s" % self.__class__.__name__  )
        filename = self.skeletonFilename()
        self.logger().info( 'Writing skeleton to file \"%s\"', filename )
        with open( filename, 'w' ) as f:
            f.write( self.getSkeletonTemplate() % self.argumentValueDict() )

    ## Getter function for help text associated with the transform.
    #  @see _help attribute.
    #  @return String
    def getHelp(self):
        return self._help

    ## Generate short usage message associated with the transform.
    #  @return String
    def getUsage(self):
        cmd = self.filename()
        use = 'usage: %s [options]' % cmd
        for arg in self._positionalArgs:
            if arg.isOptional():
                use += ' [%s]' % arg.name().lower()
            else:
                use += ' <%s>' % arg.name().lower()
        use += os.linesep
        use += '       Arguments should be given as name=value.'
        use += os.linesep
        use += '       Option -h,--help: get detailed help'
        return use

    ## Retrieve pre-formatted help messages for all run options available to the transform.
    #  @return String
    def getFullOptionsHelp(self):
        return 'Options:' + os.linesep + self.runOptions().__str__('   ')

    ## Retrieve pre-formatted help messages for all arguments in the transform.
    #  @return String
    def getFullArgsHelp(self):
        help = [ 'Arguments:' ]
        help += [ '  ' + arg.fullHelp() for arg in self._positionalArgs ]
        return os.linesep.join( help )

    ## Retrieve complete transform help message.
    #  @return String
    def getFullHelp(self):
        use = [ 'JobTransform %s version %s'  % (self.name(),self.version()) ,
                self.getHelp(),
                self.getUsage(),
                self.getFullOptionsHelp(),
                self.getFullArgsHelp() ]
        return os.linesep.join(use)

    ## Print short usage message to @c sys.stdout
    #  @return None
    def usage(self):
        print (self.getUsage())

    ## Print full help message to @c sys.stdout
    #  @return None
    def help(self):
        print (self.getFullHelp())

    ## Add all arguments to _positionalArgs, ensures proper help is provided
    #   @details Placeholder, to be implemented in task specific transforms.
    #   @details Included here for backwards compatibility with older trf.
    def addAllArgs(self):
        return

    ## Print full help message to @c sys.stdout and exit program.
    #  @return None
    def printHelpAndExit(self):
        self.addAllArgs()
        self.help()
        sys.exit()

    ## Convert a Python exception caught by the transform to an appropriate AtlasErrorCodes.ErrorInfo instance.
    #  @param e A Python Exception instance.
    #  @remarks An unknown exception is treated as a AtlasErrorCodes.FATAL error with an @b TRF_EXC error acronym.
    #  @return AtlasErrorCodes.ErrorInfo instance
    def _handleException(self,e):
        err = trferr.errorHandler.handleException(e)
        if err is not None:
            err.who = 'JobTransform.%s' % self.name()
        else:
            err = AtlasErrorCodes.ErrorInfo( acronym='TRF_EXC', severity = AtlasErrorCodes.FATAL, message='%s: %s' % (e.__class__.__name__ ,e.args) )
        return err

    ## @brief Ensure the existence of the skeleton job options file.
    #  @details (It is intended that) The skeleton jobOptions file is auto-generated if it has not been specified.
    #  @warning An exception will be raised if the skeleton job options file has not been specified as the 
    #  automatic skeleton job options generator (i.e. the writeSkeleton() function) has not been implemented yet.
    #  @exception trferr.TransformEnvironmentError is raised if the skeleton job options file cannot be located.
    #  @see writeSkeleton() function to further understand warning.
    #  @return None
    def ensureSkeleton(self):
        fn = self._skeleton
        if fn is None:
            # auto-generate skeleton jobOptions file
            self._skeleton = self.defaultSkeletonFilename()
            self.writeSkeleton()
            fn = self._skeleton
        # get joboptions search path
        # try to find the file in the search path
        if not find_joboptions( fn ):
            raise TransformEnvironmentError( 'Skeleton file %s not found in %s' % (fn,trfconsts.JOBOPTIONSPATH) )

    ## Read transform arguments from pickle file and populate the current transform's arguments with its corresponding values.
    #  @remarks Only supported arguments will be used whilst all others will be ignored.
    def readArgsFromPickle(self,filename):
        with open( filename ) as argFile:
            argDict = pickle.load(argFile)
        for n,v in argDict.items():
            try:
                self.setArgument(n,v)
            except TransformArgumentError:
                pass

    ## Write the dictionary of all the original argument values to file.
    #  @see argumentOriginalDict() function.
    def writeArgsToPickle(self,filename):
        fileutil.remove(filename)
        with open( filename, 'w' ) as argFile:
            pickle.dump(self.argumentOriginalDict(), argFile)

    ## @brief Process list of arguments including the options.
    #  @details Transform arguments and options are populated with information from the @em argList provided.
    #  @param argList List of arguments (Strings) at the command line.
    #  @exception trferr.TransformArgumentError is raised when positional arguments are specified
    #  after named arguments. 
    #  The same exception is also raised when too many positional arguments are given.
    #  @see setOptionArg() and setArgument() functions.
    #  @return None
    def processArgs(self, argList):
        #Print usage when no arguments are given
        if len(argList) == 0:
            self.usage()
            sys.exit()
        args = self._runOpts.extractOptions(argList)
        # process arguments
        posArgs = self._positionalArgs
        nArgs = len(args)
        # fill the dictionary with all given arguments
        lastNamed=''
        pos = 0
        for i in range(nArgs):
            val = args[i]
            equal = val.find('=')
            if equal == -1:
                #positional argument. Not allowed after named argument
                if lastNamed:
                    raise TransformArgumentError(
                        'No positional arguments allowed after named arguments.' +
                        ' Positional argument %d (%s) after named argument %s=%r' %
                        (i+1,val,lastNamed,self.getArgument(lastNamed).value()) )
                try:
                    name = posArgs[pos].name()
                except IndexError:
                    raise TransformArgumentError ('too many arguments: %d (max=%d)' %
                                                  (nArgs, len(posArgs)))
                pos += 1
            else:
                #named argument or option
                name = val[:equal]
                val = val[equal+1:]
                if self.isOptionArg(name):
                    self.setOptionArg(name,val)
                    continue
                else:
                    lastNamed = name
                    pos += 1
            self.setArgument( name, val )

    ## @brief Process the information in the configuration object.
    #  @details Transform arguments and options are populated with information from the @em argList provided.
    #  Configuration auxiliary files are retrieved, min and max events are set, 
    #  max event handling strategy is set and filter efficiency (<=1) taken into account.
    #  @warning This function must be called @em after the pre-run actions of the individual arguments.
    #  @see doPreRunActions() function.
    #  @return None
    def processConfig(self):
        config = self._config
        if not config: return
        # print configuration
        for line in str(config).split(os.linesep):
            self.logger().info( line )
        # get all the auxiliary files
        if config.auxfiles:
            get_files( config.auxfiles, keepDir=False, errorIfNotFound=True )
        # How to handle the case when number of input events is smaller than maxEvents
        self._maxEventsStrategy = config.maxeventsstrategy
        # the the limits on the number of events
        minEvts = config.minevents
        if minEvts > 0:
            self.setMinMaxEvents(minEvts,config.maxeventsfactor*minEvts)
        # compensate number of events to loop for generator efficiency
        eff = config.efficiency
        if eff < 1.0:
            maxEvts = self.getArgumentOfType('MaxEvents')
            if maxEvts and maxEvts != -1:
                oldEvents = maxEvts.value()
                if oldEvents > 0:
                    newEvents = int( math.ceil(oldEvents/eff) )
                    maxEvts.setValue(newEvents)
                    self.logger().info("Processing %d events instead of %d to account for filtering efficiency %f", newEvents, oldEvents, eff )

    ## Dump to file the complete shell command to facilitate the re-execution of the transform.
    #  @remarks The arguments of the transform are written to a separate file as well.
    #  @see writeArgsToPickle() function.
    #  @return None
    def writeLastCommand(self):
        lastname = 'last.' + self._name
        with open( lastname, 'w' ) as lastfile:
            cmd = self.filename()
            for arg in self._positionalArgs:
                cmd += ' %s=%s' % ( arg.name(), arg.originalValue() )
            # allow for overrides when re-executing last command
            cmd += ' $@'
            lastfile.write( 'exec ' + cmd + os.linesep )
        os.chmod(lastname, stat.S_IRWXU | stat.S_IRGRP | stat.S_IXGRP | stat.S_IROTH | stat.S_IXOTH )
        # write last arguments in pickle file
        argsFilename = 'last.runargs.gpickle'
        self.writeArgsToPickle( argsFilename )

    ## Determine the atlas release used.
    #  @remarks If the release can be determined, the @c ATLAS_RELEASE environment variable is set to that value.
    #  @return None
    def _determineAtlasRelease(self):
        self._atlasrelease = get_atlas_release()
        if self._atlasrelease:
            self.logger().info( "Using Atlas Release %s", self._atlasrelease )
            os.environ['ATLAS_RELEASE'] = self._atlasrelease
        else:
            self.logger().warning( "Could not determine Atlas Release" )

    ## @brief Determine the database release used with the @c DBRELEASE environment variable
    #  @details This value is checked against the requested version (i.e. @c DBRELEASE_REQUESTED) 
    #  and the minimum required version (i.e. @c DBRELEASE_REQUIRED )
    #  @return None
    def _determineDBRelease(self):
        # try environment variable DBRELEASE
        self._dbrelease = os.environ.get('DBRELEASE')
        if self._dbrelease:
            self.logger().info( "Got Database Release number %s from environment variable DBRELEASE", self._dbrelease )
        else:
            self.logger().info( "Database Release no longer needed for r19 and beyond" )
            return
        if self._dbrelease == 'current': # no further check required.
            return
        # check consistency with requested version
        dbfound = self._dbrelease
        dbwanted = os.environ.get('DBRELEASE_REQUESTED')
        if dbwanted and dbwanted != dbfound:
            raise TransformEnvironmentError( "Wrong DBRelease version: %s. Requested version: %s" % (dbfound,dbwanted), error='TRF_DBREL_VERSION' )
        # check consistency with required version
        dbneeded = os.environ.get('DBRELEASE_REQUIRED')
        if dbneeded and VersionString(dbfound) < VersionString(dbneeded):
            raise TransformEnvironmentError( "Wrong DBRelease version: %s. Need at least version %s" % (dbfound,dbneeded), error='TRF_DBREL_VERSION' )

    ## @brief Gather information on the environment
    #  @details The information gathered includes information on the Atlas release, database release, 
    #  platform info, cpu info, run directory and execution environment. 
    #  @return None
    def gatherEnvironmentInfo(self):
        # ATLAS offline release number
        self._determineAtlasRelease()
        self._jobReport.addInfo( JobInfo('AtlasRelease', self._atlasrelease) )
        # Database Release number
        self._determineDBRelease()
        self._jobReport.addInfo( JobInfo('DBRelease', self._dbrelease) )
        # What machine are we running on
        machineInfo = JobInfo('Machine')
        # Platform information
        import platform
        joinList = lambda x : '-'.join(x)  # noqa: E731
        asIs = lambda x: x                 # noqa: E731
        platformEnv = { 'architecture'   : joinList ,
                        'dist'           : joinList ,
                        'machine'        : asIs ,
                        'node'           : asIs ,
                        'platform'       : asIs ,
                        'processor'      : asIs ,
                        'python_version' : asIs ,
                        'system'         : asIs  }
        for var,func in platformEnv.items():
            varFunc = getattr(platform,var)
            machineInfo.addContents( JobInfo(var, func( varFunc() ) ) )
        # CPU information from /proc/cpuinfo
        log = self.logger()
        cpumodel=''
        cpucache=''
        modelstring='UNKNOWN'
        fname='/proc/cpuinfo'
        modelRE=re.compile(r'^model name\s+:\s+(\w.+)')
        cacheRE=re.compile(r'^cache size\s+:\s+(\d+ KB)')
        try:
            with open( fname ) as f:
                for line in f:
                    model=modelRE.search(line)
                    if model:
                        cpumodel=model.group(1)
                        log.info( 'Found CPU model: ' + cpumodel )
                    cache=cacheRE.search(line)
                    if cache:
                        cpucache=cache.group(1)
                        log.info( 'Found CPU cache size: ' + cpucache )
                        machineInfo.addContents( JobInfo('cachesize',cpucache) )
                    # stop after 1st pair found - can be multiple cpus
                    if cpumodel and cpucache:
                        modelstring=cpumodel + " " + cpucache
                        break
        except OSError:
            log.warning('Could not determine CPU model (file %s not found)', fname)
        if modelstring == 'UNKNOWN':
            log.warning('CPU model and/or cache size not found')
        machineInfo.addContents( JobInfo('model',modelstring) )
        self._jobReport.addInfo(machineInfo)
        # Working directory
        self._jobReport.addInfo( JobInfo('Workdir',os.getcwd()) )
        # Environment variables
        envInfo = JobInfo('Environment')
        excludeEnv = ( 'YP', 'SSH_.*', '(COLOR|X)?TERM', 'KONSOLE_.*', '.*ROOT', 
                       'VISUAL', 'EDITOR', 'PAGER', 'SHLVL', 'DISPLAY', 'PS[12]', 'MANPATH',
                       'GROUP_DIR', 'MAIL', '.*_HOME', 'WWW_.*', 'GTK_.*', 'ENV',
                       'SAVEPATH', 'NNTPSERVER', '_?HPX_.*', 'LC_.*', 'LANG', 'LS_COLORS',
                       'HEP_ENV', 'KDE_.*', '.*SESSION', 'SESSION.*', 'XMODIFIERS',
                       'X?PRINT.*', 'INPUTRC', 'LESS.*', 'AFSHOME', 'USERPATH', 'IFS', 'LAMHELPFILE',
                       'CLUSTER_DIR', 'ENVIRONMENT', 'GS_LIB', 'ROOTPATH', 'XAUTHORITY'
                       '.*_DCOP', 'DCOP_.*', 'DOTFONTPATH', 'INITIALISED',
                       'SAVEHIST', 'HISTSIZE',
                       'cmt', 'jcmt', 'CVS.*', 'CMTCVSOFFSET', os.sep )
        excludeEnvRE = re.compile( '^%s$' % '|'.join(excludeEnv) )
        for n,v in os.environ.items():
            if not excludeEnvRE.match(n):
                envInfo.addContents( JobInfo(n,v) )
        # add everything to jobReport
        self._jobReport.addInfo(envInfo)
        # print out part of the information
        self.dumpReport(self._startReportOptions,self._startReportNotOptions)

    ## @brief Gather information on the run directory.
    #  @details The information gathered on the run directory includes a full file listing (one line per entry), 
    #  the disk space (in bytes) used and what is still available.
    #  @return Dictionary
    def _getRunDirInfo(self):
        listing = []
        totalSize = 0
        format = "%-10s %10s %24s %s"
        listing.append( format % ("access", "size", "modification_time", "filename") )
        for dirpath,dirnames,filenames in os.walk(os.curdir):
            for f in filenames + dirnames:
                try:
                    fullname = os.path.join(dirpath,f)
                    filestat = fileutil.lstat(fullname)
                    filesize = filestat.st_size
                    filetime = filestat.st_mtime
                    filemode = fileutil.mode_to_string(filestat.st_mode)
                    # prettify the filename printout
                    if os.path.isdir(fullname):
                        fullname += os.sep
                    elif os.path.islink(fullname):
                        target = os.readlink(fullname)
                        fullname += ' -> ' + target
                        filesize = len(target)
                    if fullname.startswith( os.curdir + os.sep ): fullname = fullname[2:]
                    listing.append( format % (filemode, filesize, time.ctime(filetime), fullname) )
                    totalSize += filesize
                except OSError:
                    self.logger().info("lstat failed on %s (probably the file was removed)",f)
                    pass
        statvfs = os.statvfs(os.curdir)
        availSize = statvfs.f_bavail * statvfs.f_bsize
        return { 'RunDirListing'       : os.linesep.join(listing) ,
                 'RunDirUsedDisk'      : totalSize ,
                 'RunDirAvailableDisk' : availSize }

    ## Switch between old and new metadata formats.
    #  @remarks New metadata format is used when the @c --usenewmeta=True is specified at the command line.
    #  @return None
    def addMetaData( self ):
        if self._jobReport.useNewMetadataFormat:
            self.addMetaData_new()
        else:
            self.addMetaData_old()

    ## @brief Gather metadata information from various sources for the job report.
    #  @details Metadata is extracted from the log file, 
    #  argument instances, configuration, input and output files.
    #  @remarks This new format attributes metadata direct to the entity it refers to (as it should).
    #  Provenance information is preserved i.e. sequence of sub-transforms, input and output files are 
    #  attributed to the (sub-)transforms responsible.
    #  @return None
    def addMetaData_new(self):
        # gather metadata from various sources
        addMeta = {}
        # A copy of JobReport._extraMetadataDict is made to allow for modification 
        # to facilitate loop traversal with deletion.
        tempMDict = _extraMetadataDict.copy()
        # gather metadata from logfile
        logfile = self._logFilename
        if os.path.exists(logfile):
            self.logger().info( "Scanning logfile %s for metadata...", logfile )
            # pattern in logfile:
            #    MetaData: <name> [unit]=<value>
            metaPat = re.compile( r"^MetaData:\s+(.*?)\s*=\s*(.*)$" )
            with open( logfile ) as theLog:
                for line in theLog:
                    line = line.rstrip()
                    match=metaPat.search(line)
                    if match:
                        name=match.group(1).split()[0]  # take first word (second word is optional unit)
                        value=match.group(2)
                        self.logger().info( "Found MetaData: %s=%s", name,value )
                        addMeta[name] = value
                        continue
                    # gather extra metadata from extraMetadata
                    for mData in tempMDict.keys():
                        mDataRE = tempMDict[ mData ]
                        try:
                            addMeta[ mData ] = mDataRE.match( line ).group( 1 )
                        except AttributeError:
                            if not isinstance( mDataRE, re._pattern_type  ): # user-defined metadata
                                addMeta[ mData ] = tempMDict.pop( mData ) # remove once added.
                        else:
                            tempMDict.pop( mData ) # match occured so removing
        # gather metadata from arguments
        for arg in self._positionalArgs:
            addMeta.update( arg.metaData() )
        # gather metadata from jobconfiguration
        if self._config is not None:
            addMeta.update( self._config.metaData() )
        self._jobReport._task.addMetaData( addMeta )
        # Update inputfiles in jobReport
        for f in self._inputFiles:
            fileInfoDict = f.fileInfo()
            if not isinstance( fileInfoDict, dict ):
                if fileInfoDict is None:
                    fileInfoList = []
                else:
                    fileInfoList = [ fileInfoDict ] 
            else:
                fileInfoList = fileInfoDict.values()
            for fInfo in fileInfoList:
                self._jobReport.addInputFile( fInfo )
        # Update outputfiles in jobReport
        for f in self._outputFiles:
            fileInfoDict = f.fileInfo()
            if not isinstance( fileInfoDict, dict ):
                if fileInfoDict is None:
                    fileInfoList = []
                else:
                    fileInfoList = [ fileInfoDict ] 
            else:
                fileInfoList = fileInfoDict.values()
            for fInfo in fileInfoList:
                # fill in missing info from jobReport
                reportInfo = self._jobReport.outputFile(fInfo.filename())
                if reportInfo:
                    for name,value in fInfo.metaData().items():
                        if value is None:
                            reportValue = reportInfo.metaData(name)
                            if reportValue is not None:
                                fInfo.addMetaData( { name:reportValue } )
                # Update outputfiles in jobReport
                self._jobReport.addOutputFile(fInfo)

    ## @brief Gather metadata information from various sources for the job report.
    #  @details Metadata is extracted from the log file, 
    #  argument instances, configuration, input and output files. 
    #  @remarks This old format forces metadata to be associated with output files regardless of the context.
    #  Provenance (as described in addMetaData_new() function) is not preserved.
    #  @return None
    def addMetaData_old(self):
        # gather metadata from various sources
        addMeta = {}
        # a copy of JobReport._extraMetadataDict is made to allow for modification 
        # to facilitate loop traversal with deletion.
        tempMDict = _extraMetadataDict.copy()
        # gather metadata from logfile
        logfile = self._logFilename
        if os.path.exists(logfile):
            self.logger().info( "Scanning logfile %s for metadata...", logfile )
            # pattern in logfile:
            #    MetaData: <name> [unit]=<value>
            metaPat = re.compile( r"^MetaData:\s+(.*?)\s*=\s*(.*)$" )
            with open(logfile) as theLog:            
                for line in theLog:
                    line = line.rstrip()
                    match=metaPat.search(line)
                    if match:
                        name=match.group(1).split()[0]  # take first word (second word is optional unit)
                        value=match.group(2)
                        self.logger().info( "Found MetaData: %s=%s", name, value )
                        addMeta[name] = value
                        continue
                    # gather extra metadata from extraMetadata
                    for mData in tempMDict.keys():
                        mDataRE = tempMDict[ mData ]
                        try:
                            addMeta[ mData ] = mDataRE.match( line ).group( 1 )
                        except AttributeError:
                            if not isinstance( mDataRE, re._pattern_type  ): # user-defined metadata
                                addMeta[ mData ] = tempMDict.pop( mData ) # remove once added.
                        else:
                            tempMDict.pop( mData ) # match occured so removing
        # gather metadata from arguments
        for arg in self._positionalArgs:
            addMeta.update( arg.metaData() )
        # gather metadata from jobconfiguration
        if self._config is not None:
            addMeta.update( self._config.metaData() )
        for f in self._outputFiles:
            info = f.fileInfo()
            if info:
                # This is the original way metadata is stored... forced association with output files
                info.addMetaData( addMeta )
                # fill in missing info from jobReport
                reportInfo = self._jobReport.outputFile(info.filename())
                if reportInfo:
                    for name,value in info.metaData().items():
                        if value is None:
                            reportValue = reportInfo.metaData(name)
                            if reportValue is not None:
                                info.addMetaData( { name:reportValue } )
                # Update outputfiles in jobReport
                self._jobReport.addOutputFile(info)

    ## Depending on the type of input file specified, a local copy is made to optimisation and stability purposes.
    #  @return None
    def preStageInputFiles(self):
        from PyJobTransformsCore.FilePreStager import theFileStagerRobot
        self._addLogger( theFileStagerRobot )
        fileList = []
        for f in self._inputFiles:
            if f:
                value = f.value()
                if type(value).__name__ == 'list':
                    fileList += value
                else:
                    fileList.append(value)
        theFileStagerRobot.addFilesToStagerIfNeeded( fileList )
        filesNotStaged = theFileStagerRobot.waitUntilAllFilesStaged()
        if filesNotStaged:
            problemFiles = ''
            for filename,status in filesNotStaged.items():
                problemFiles += os.linesep + "%s:%s" % (filename,status)
            raise InputFileError("Could not stage following files from tape:%s" % problemFiles )

    ## Check arguments for corresponding values otherwise use defaults.
    #  @exception trferr.TransformArgumentError is raised when an argument is not presented with a value and 
    #  does not possess a default value either.
    #  @return None
    def checkArguments(self):
        # fill in missing arguments with default values
        for arg in self._positionalArgs:
            if not arg.hasValue() and arg.hasDefault():
                self.setArgument( arg.name(), arg.getDefault() )
        # check that all arguments have a value
        noValue = [ arg.name() for arg in self._positionalArgs if not arg.hasValue() ]
        if len(noValue):
            raise TransformArgumentError( 'Argument(s) not set: %s' % (','.join(noValue),) )

    ## @brief Check the number of input events are as expected.
    #  @details The maximum number of events desired/required is determined by querying the @em maxEvents argument. 
    #  If the value is < 1 (i.e. 0 or -1), the check is abandoned. This user provided value is compared 
    #  against the actual number of events found in the @em first of the list of input files and an appropriate 
    #  response is initiated. Command-line options to omit certain validation routines will affect the 
    #  response of this function as well.
    #  @exception trferr.InputFileError is raised when the comparison between the desired number of events value 
    #  and the actual number of events is not as expected.
    #  @return None
    def checkInputEvents(self):
        maxEventsArg = self.getArgumentOfType("MaxEvents")
        try:
            maxEvents = maxEventsArg.value()
            # check first file in input file list only.
            if len(self._inputFiles) > 1:
                self.logger.info('Checking maxEvents against {0} only ({1} are assumed to have the same event count)'.format(self._inputFiles[0].name(), [f.name() for f in self._inputFiles[1:]]))
            inputFile = self._inputFiles[0]
            if VALIDATION_DICT[ 'ALL' ] is False or VALIDATION_DICT[ 'testCountEvents' ] is False:
                raise Exception
        except Exception:
            self.logger().info( "Skipping input file max event check." )
            return
        # Do nothing if all events are to be used (-1) rather than set it to the actual number.
        # Also, if no events (0) are to be used, simply return
        if maxEvents < 1:
            return
        # maxEvents given a value so test that it is appropriate.
        events = inputFile.eventCount()
        if events is None:
            if not TRF_SETTING[ 'testrun' ]:
                self.logger().info( "Skipping input file event check. File does not lend itself to event counting." )
        elif events == 0:
            # cross-check with filesize
            totalSize = inputFile.fileSize()
            if totalSize > 0:
                self.logger().warning("Found 0 events in %s, but total filesize %s. Ignoring event count.",
                                      inputFile.name(), totalSize )
            else:
                raise InputFileError(inputFile.originalValue(),' empty file(s). Argument %s' %
                                     (inputFile.name(),) ,
                                     error='TRF_INFILE_EMPTY')
        elif events < maxEvents:
            if self._maxEventsStrategy =='ABORT':
                raise InputFileError(inputFile.originalValue(),': too few events (%d < %d) in input file' %
                                     (events,maxEvents),
                                     error='TRF_INFILE_TOOFEW')
            elif self._maxEventsStrategy =='INPUTEVENTS':
                maxEventsArg.setValue(events)
                self.logger().warning("Replacing %s=%d by number of events (%d) in input file(s) %s",
                                      maxEventsArg.name(),maxEvents,events,inputFile.originalValue())
            elif self._maxEventsStrategy =='IGNORE':
                self.logger().warning("%s=%d larger than number of events (%d) in input file(s) %s",
                                      maxEventsArg.name(),maxEvents,events,inputFile.originalValue())
            else:
                self.logger().warning("Unknown maxEventsStratety (%s). Ignoring that %s=%d is larger than number of events (%d) in input file(s) %s",
                                      self._maxEventsStrategy,maxEventsArg.name(),maxEvents,events,inputFile.originalValue())
            # Do check on minimum number of input events 
            if VALIDATION_DICT[ 'ALL' ] is False or VALIDATION_DICT[ 'testEventMinMax' ] is False:
                self.logger().info( "Input file event min/max test omitted." ) 
            elif self._minEvents and events < self._minEvents:
                raise InputFileError(inputFile.originalValue(),': too few events (%d < %d) in input file' %
                                     (events,self._minEvents),
                                     error='TRF_INFILE_TOOFEW')
        else:
            self.logger().info( "Input file event check successful." )

    ## @brief Function hook to facilitate specialised event matching routines.
    #  @details This hook is to allow transforms to match the number of 
    #  input and output events as appropriate for the file type in question.
    #  @return None
    def matchEvents( self ):
        pass

    ## @brief Top-level function to execute all available pre-run actions (in specified arguments).
    #  @details Special attention is given to the SQLiteSupport argument: The pre-run action associated with the 
    #  @em SQLiteSupport argument the executed only if the @c --useSQLite=True command-line 
    #  option is used (if the option is omitted, the default is @c False).
    #  @remarks The processConfig() function is also executed to process the configuration object.
    #  @return None
    def doPreRunActions(self):
        self.logger().debug("Going to execute Pre-Run commands...")
        # run arguments specific preRunActions
        for cmd in self._preRunActions:
            if isinstance( cmd, SQLiteSupport ):
                # Unless _useSQLite is explicitly set, it takes the value of _mcInput 
                # as the former takes precedence over the latter when determining whether 
                # SQLiteSupport is enabled.
                if self._useSQLite is None:
                    self._useSQLite = self._mcInput
                    self.logger().info( "%s use of SQLite.", { True : 'Enabling', False : 'Disabling', None : 'Disabling' }[ self._useSQLite ] )
                if not self._useSQLite:
                    continue
            cmd.preRunAction()
        # Process the configuration object
        self.processConfig()
        self.logger().debug("Done executing Pre-Run commands")


    def doUpload(self):
        
        isProd = False
        
        try:
            out = subprocess.Popen(["voms-proxy-info","-fqan"],  stdout=subprocess.PIPE, stderr=subprocess.STDOUT).communicate()[0]
            lines=out.split("\n")
            for l in lines:
                # print (l.strip())
                if l.startswith('/atlas/Role=production'): isProd=True
        except OSError:
            print ("trf.py - Not a prodSys environment.")
                     
        
        if isProd is False and os.getenv('TZAMIPW') is None: print ('Performance data will not get stored in the AMI db.'          )
        elif self._exportToAmi is True and self.name()!='Digi_trf':  # digi is off as it has no AMItag and is fast
        
            #print ('-------', self.name(), '-------')
            
            isMC = False
            isStream=''
            isAMItag=''
            isRun=-1

            import PyUtils.AthFile as athFile 
            # this loop just tries to find runnumber, stream, amitag. should not look at NTUP files as these have not metadata embeded
            for arg in self._positionalArgs: 
                # print (arg.name())
                if  arg.name()=='outputAODFile' or arg.name()=='outputESDFile' :
                    # or arg.name().startswith('outputNTUP') 
                    # arg.name()=='outputEvgenFile' or arg.name()=='outputHitsFile' or # arg.name().startswith('outputRDOFile'):
                    print ('========o= inputFilePeeker ============')
                    print (arg.name(), arg.value())
                    inFile=arg.value()
                                        
                    inputFileSummary={}
                    try:
                        fi = athFile.fopen(inFile)
                        inputFileSummary = fi.fileinfos
                    except Exception as err:
                        print ("Unable to open file:",inFile)
                        print ('caught:\t',err)
                    
                    try:
                        if 'IS_SIMULATION' in inputFileSummary['evt_type']:
                            isMC = True
                        print ('isMC     ', isMC)
                        
                        if 'triggerStreamOfFile' in inputFileSummary['tag_info']:
                            isStream = inputFileSummary['tag_info']['triggerStreamOfFile']
                        print ('isStream ',isStream)
                        
                        if 'AMITag' in inputFileSummary['tag_info']:
                            isAMItag = inputFileSummary['tag_info']['AMITag']
                        print ('isAMItag ',isAMItag)
                        
                        if 'run_number' in  inputFileSummary:
                            isRun    = inputFileSummary['run_number'][0]
                        print ('isRun    ', isRun)
                        
                        if isMC is True:
                            print ('this is MC. Changin stream->procstep and runnumber -> pandaid')
                            isStream=self.name()
                            if inFile[1].isdigit():
                                isRun = inFile[1]
                            else:
                                isRun = 0
                    except Exception:
                        print ("Problem in decoding variables.")
                        print (sys.exc_info()[0])
                        print (sys.exc_info()[1])
                    except Exception:
                        print ("Unexpected error:", sys.exc_info()[0])
                    print ('=====================')
                   
            if isAMItag!='':
                print ('trf.py STARTING UPLOAD the final values -> stream:',isStream,'\trunnumber:',isRun,'\tamitag:',isAMItag)
                
                import PyJobTransforms.performanceDataUploader as pu
                
                uploader = pu.PerformanceUploader(isProd)
                uploader.establishConnection()
                # this loop finds sizes and formats and uploads to AMI for all the files.
                for cmd in self._postRunActions:
                    try:
                        # print ('trf.py _postRunAction ',cmd)
                        fs=[]
                        cmd.getDataForAmi(fs)
                        print ('trf.py returned from getting data on object sizes')
                        if len(fs)==3:
                            if fs[1]>0:
                                try:
                                    uploader.uploadDataSize(fs[0], int(isRun), isStream ,isAMItag, fs[1], fs[2])
                                    print ('trf.py object size data upload DONE')
                                except Exception as exc:
                                    print (exc)
                                except Exception:
                                    print ("Unexpected error:", sys.exc_info()[0])
                    except Exception as e:
                        print ('trf.py WARNING: Could not send size data to AMI ' , e)
                        print (sys.exc_info()[0])
                        print (sys.exc_info()[1])
                    except Exception:
                        print ("Unexpected error:", sys.exc_info()[0])
                
                if self._name=='AtlasG4_trf' or self._name=='Evgen_trf' or self._name=='Digi_trf': 
                    perffile='ntuple.pmon.gz' 
                else:
                    perffile='ntuple_'+self._name+'.pmon.gz'
                    
                if os.access(perffile,os.F_OK):
                    try:
                        print ('trf.py: uploading job performance data to AMI')
                            
                        try:
                            uploader.uploadPerfMonSD(isAMItag, self._name, isStream, int(isRun), perffile)
                        except Exception as exc: 
                            print (exc)
                        except Exception:
                            print ("Unexpected error:", sys.exc_info()[0])
                        
                        print ('trf.py upload of job performance data done!')
                    except Exception as e:
                        print ('trf.py WARNING: Could not send job info to AMI ' , e)
                        print (sys.exc_info()[0])
                        print (sys.exc_info()[1])
                    except Exception:
                        print ("Unexpected error:", sys.exc_info()[0])
                else:
                    print ('there is no perfmon file: ', perffile)
        
        
        
    ## Top-level function to execute all available post-run actions (in specified arguments).
    #  @remarks Event matching is performed unless validation routines are explicitly omitted.
    #  The addMetaData() function is also executed to gather metadata from output files.
    #  @return None
    def doPostRunActions(self):
        self.logger().debug("Going to execute Post-Run commands...")
        
        for cmd in self._postRunActions:
            try:
                cmd.postRunAction()             
            except TransformValidationError as e:
                self.addValidationError( e )
                
        from PyJobTransforms.performanceDataUploader import timelimited
        try:
            timelimited(120, self.doUpload)
        except Exception as exc: 
            print (exc)
        except Exception:
            print ("Unexpected error:", sys.exc_info()[0])
            
        
        if not ( VALIDATION_DICT['ALL'] is False or VALIDATION_DICT[ 'testMatchEvents' ] is False ):
            self.matchEvents()
        else:
            self.logger().info( "Skipping event number matching." )
        # add metadata to output files and update jobReport
        self.addMetaData()
        self.logger().debug("Done executing Post-Run commands")
        
    
    ## @brief The top-level execution routine for a transform.
    #  @details This function consitutes the very core of a transform as it executes all other functions required 
    #  before and after the main Athena job is executed.
    #  @remarks Derived classes may override this function as it is typically done for composite transforms.
    #  @exception trferr.JobOptionsNotFoundError is raised when the top-level job options file cannot be found.
    #  @exception trferr.TransformArgumentError is raised when the required number of events is not within limits.
    #  @return JobReport.JobReport instance containing results from the current job.
    def runJob(self):
        # TEST Error insertion
        # self.addError( acronym = 'ATH_FAILURE', severity = AtlasErrorCodes.FATAL, message = 'TEST ONLY' )
        #
        # start with the pre-run actions
        self.doPreRunActions()
        # gather environment only after preRunActions, as they may change the environment
        self.gatherEnvironmentInfo()
        # Do check on minimum and maximum number of requested output events (only if not running in test mode)
        maxEvents = None
        if VALIDATION_DICT[ 'ALL' ] is False or VALIDATION_DICT[ 'testEventMinMax' ] is False:
            self.logger().info( "Event min/max test omitted." )
        else:
            maxEventsArg = self.getArgumentOfType("MaxEvents")
            try:
                maxEvents = maxEventsArg.value()
            except Exception:
                pass
        if maxEvents and maxEvents != -1:
            # undo any efficiency correction
            if self._config: maxEvents *= self._config.efficiency
            if self._minEvents and maxEvents < self._minEvents:
                raise TransformArgumentError("Too few requested events (%s=%d < %d)" % (maxEventsArg.name(),maxEvents,self._minEvents), error='TRF_ARG_MAXEVT_TOOFEW' )
            if self._maxEvents and maxEvents > self._maxEvents:
                raise TransformArgumentError("Too many requested events (%s=%d > %d)" % (maxEventsArg.name(),maxEvents,self._maxEvents), error='TRF_ARG_MAXEVT_TOOMANY' )
        # pre-stage any files that need it
        self.preStageInputFiles()
        # Check the number of input events compared to maxEvents
        self.checkInputEvents()
        # Prepare for running athena job
        # open the logfile
        self._logFile = fileutil.Tee(self._logFilename,'a')
        # get/check the needed top jobOptions files
        self.ensureSkeleton()
        self.writeRunArgs()
        topOptions = [ self.runArgsFilename() ]
        for jo in self._preJobOptionsFiles:
            fn = jo.filename()
            if fn: topOptions.append( fn ) 
        topOptions += [ self.skeletonFilename() ]
        for jo in self._postJobOptionsFiles:
            fn = jo.filename()
            if fn: topOptions.append( fn )
        # Use new message service i.e. LoggedMessageSvc
        self._makeMessageSvcJobOptions()
        topOptions.append( self.messageSvcFilename() )
        # add the contents of the top joboptions to the logfile
        line = CommentLine('Start of top joboptions').bigComment('=')
        self._logFile.write(line + os.linesep)
        for jo in topOptions:
            fullJo = find_joboptions( jo )
            if not fullJo:
                raise JobOptionsNotFoundError(jo,'Top jobOptions file not found')
            self.logger().info( 'Found top jobOptions %s in %s', jo, strip_suffix(fullJo,jo) )
            with open( fullJo ) as joFile:
                for line in joFile:
                    self._logFile.write(line)
        line = CommentLine('End of top joboptions').bigComment('=')
        self._logFile.write(line + os.linesep)
        # Run the athena job
        trfenv.setup_athena_runtime(trfconsts.athena_py)
        wrapper = os.path.join( trfenv.trfPath, 'athena_wrapper.py' )
        if self._PECommand:
            peCommand = [ '-c', self._PECommand ]
        else:
            peCommand = []
        # peCommand must be the last entry as athena_wrapper.py expects this.
        athenaScriptArgs = [ wrapper, trfenv.athena_exe ] + topOptions + self._athenaOptions + self._genericOptions + peCommand
        # Create a script to allow for the trf to be re-run easily. 
        athenaScript = './%s_runathena' % self.name()
        # Set this to true if we actually want to execute the 'wrapper' script (for asetup, e.g.)
        runViaScript = False
        try:
            with open( athenaScript, 'w' ) as athenaFile:
                # If we have an asetup to add, add it here....
                if 'asetup' in self._namedArgs:
                    self.logger().info('Found asetup arg: %s', self._namedArgs['asetup'].value())
                    athenaFile.write('#! /bin/sh' + os.linesep)
                    athenaFile.write('%s/scripts/asetup.sh %s' % (os.environ['AtlasSetup'], self._namedArgs['asetup'].value()) + os.linesep)
                    runViaScript = True
                athenaFile.write(  ' '.join( athenaScriptArgs ) + os.linesep )
            os.chmod( athenaScript,
                      statconsts.S_IXUSR | statconsts.S_IXGRP | statconsts.S_IXOTH |
                      statconsts.S_IRUSR | statconsts.S_IRGRP | statconsts.S_IROTH |
                      statconsts.S_IWUSR )
        except Exception as e:
            self.logger().warning( 'Encountered an error while trying to create %s. %s', athenaScript, e )
        # the actual execution
        if runViaScript:
            self.logger().info( 'Executing %s: %s', self.name(), athenaScript )
            self._runJobProcess = subprocess.Popen( args = athenaScript, bufsize = 1, shell = False,stdout = subprocess.PIPE, stderr = subprocess.STDOUT )
        else:
            self.logger().info( 'Executing %s: %s', self.name(), ' '.join( athenaScriptArgs ) )
            self._runJobProcess = subprocess.Popen( args = athenaScriptArgs, bufsize = 1, shell = False,stdout = subprocess.PIPE, stderr = subprocess.STDOUT )
        # Poll stdout of the process and write to log file
        while self._runJobProcess.poll() is None:
            line = self._runJobProcess.stdout.readline()
            if line:
                self._logFile.write(line)
        # adding the exit status from athena
        rc = self._runJobProcess.returncode
        self.logger().info( '%s has completed running of Athena with exit code %s.', self.name(), rc )
        if rc < 0:
            # dump dmesg to file when the athena job receives a signal.
            os.system( 'dmesg > dmesg_athena.txt' )
            if rc == -signal.SIGKILL:
                self.logger().error( 'Athena received signal %s = SIGKILL. Athena was killed, job will terminate. ', rc )
            elif rc == -signal.SIGTERM:
                self.logger().error( 'Athena received signal %s = SIGTERM. Athena was terminated, job will terminate. ', rc )
            else:
                # After dicsussion with athena core people, we decided it's best to encode the 
                # signal exit code in a 'shell like' way, adding 128 to it
                self.logger().error( 'Athena received signal %s. Exit code reset to Athena exit code %d.', -rc, 128 + abs(rc) )
                rc = 128 + abs(rc)
        # Add the athena exit codes and acroynm directly into the main job report before 
        # it is lost due to some exception occuring between now and the end of this function. 
        # This ensures that --ignoreerrors=True works properly.
        self._jobReport.addInfo( JobInfo( 'athCode', str( rc ) ) )
        self._jobReport.addInfo( JobInfo( 'athAcronym', str( ExitCodes.what( rc ) ) ) )
        self._logFile.close()
#        trfenv.cleanup_athena_runtime() #------->AT:  does nothing!
        # Create temporary job report to fill in information gathered for this job run. 
        # This temporary job report will be merged with the main job report in the 
        # calling function execute().
        athenaReport = JobReport()
        # overwrite producer for new errors that are added
        athenaReport.setProducer( self.name(), self.version() )
        # Check the logfile for errors
        checker = AthenaLogChecker(self._atlasrelease)
        checker.checkLogFile(self._logFilename,athenaReport,self.logger() )
        # Alter behaviour of athenaReport depending on the following flags.
        athenaReport.setIgnoreUnknown( self._ignoreUnknown )
        athenaReport.setIgnoreAll( self._ignoreAll )
        athenaReport.setIgnoreErrors( self._ignoreErrors )
        athenaReport.addInfo( JobInfo( 'athCode', str( rc ) ) )
        athenaReport.addInfo( JobInfo( 'athAcronym', str( ExitCodes.what( rc ) ) ) )
        # do post-processing if successful running
        status = athenaReport.exitCode()
        if status == 0:
            self.doPostRunActions()
            # do check on number of events in output files
            if VALIDATION_DICT[ 'ALL' ] is False or VALIDATION_DICT[ 'testEventMinMax' ] is False:
                self.logger().info( "Output file event min/max test omitted." )
            else:
                for f in self._outputFiles:
                    info = f.fileInfo()
                    events = info and info.metaData("events")
                    if events:
                        if self._minEvents and events < self._minEvents:
                            raise TransformArgumentError("Too few events in file %s (%d < %d)" % (info.filename(),events,self._minEvents),error='TRF_OUTFILE_TOOFEW' )
                        if self._maxEvents and events > self._maxEvents:
                            raise TransformArgumentError("Too many events in file %s (%d > %d)" % (info.filename(),events,self._maxEvents),error='TRF_OUTFILE_TOOMANY' )
        return athenaReport

    ## This @em execute() function allows derived transforms to specify specialised @em runJob() functions whilst 
    #  ensuring that the resulting job report objects are correctly handled as the routines executed are independent 
    #  of any transform specialisation.
    #  @warning Derived transforms must not override this function.
    #  @return JobReport.JobReport instance
    def execute(self):
        self.logger().info( 'Using %s', trfenv.trfPath )
        try:
            #clean up old stuff
            fileutil.remove(self._logFilename)
            fileutil.remove(self._stderrFilename)
            fileutil.remove(self._stdoutFilename)
            for f in JobReport.defaultFiles: fileutil.remove(f)
            # check the arguments
            self.checkArguments()
            # write the last command to file
            self.writeLastCommand()
            # set up runtime environment
            trfenv.setup_runtime()
            # run the sub-job
            self._jobReport.addReport( self.runJob(), 'MERGE' )
        # Catch all exceptions
        except Exception as e:
            self.logger().error( "During execution of %s, exception caught: %s", self.name(), e )
            self._jobReport.addError( self._handleException(e) )
        # run the error diagnoser on all errors
        errorDocter = TransformErrorDiagnoser()
        for error in self._jobReport.errors():
            errorDocter.diagnoseError(error)
        # Alter behaviour of _jobReport depending on the following flags.
        self._jobReport.setIgnoreUnknown( self._ignoreUnknown )
        self._jobReport.setIgnoreAll( self._ignoreAll )
        self._jobReport.setIgnoreErrors( self._ignoreErrors )
        errorcode = self._jobReport.errorCode()
        exitcode  = self._jobReport.exitCode()
        self.logger().info( "JobTransform completed for %s with error code %s (exit code %d)", self.name(),errorcode,exitcode )
        dirInfo = self._getRunDirInfo()
        # in case of ERROR, add workdir contents
        printListing = errorcode and 'KEY_INTERRUPT' not in self._jobReport.errorAcronym()
        for n,v in dirInfo.items():
            if printListing or n != 'RunDirListing': self._jobReport.addInfo( JobInfo(n,v) )
        return self._jobReport

    ## Execute transformation using the command line arguments and writes final reports to file.
    #  @return JobReport.JobReport instance
    def exeSysArgs(self):
        # process argument list
        try:
            self.processArgs(sys.argv[1:])
        except SystemExit as e:
            if not hasattr(e,'args') or not e.args or e.args[0] == 0:
                # normal system exit call without any error (in printHelpAndExit())
                self.logger().error("Got zero SystemExit exception")
                return self._jobReport
            else:
                self.logger().error("Got SystemExit exception with code %s", e.args[0])
                self._jobReport.addError( self._handleException(e) )
        except Exception as e:
            self._jobReport.addError( self._handleException(e) )
        else:
            # Run timing routines
            vTimer.logger = self.logger()
            _n = self.name()
            vTimer.setResultsFile( 'TransformTimer_%s.pickle' % _n )
            vTimer.start( _n )
            # execute with filled arguments
            self.execute()
            vTimer.stop( _n )
            vTimer.writeResultsToFile( 'TransformTimer_%s.pickle' % _n )
        # print out part of the information
        self.dumpReport(self._endReportOptions,self._endReportNotOptions)
        # write reports depending on status
        self.writeReports()
        return self._jobReport

    ## Run transformation using arguments from a RunArguments object and writes final reports to file.
    #  @return JobReport.JobReport instance
    def exeRunArgs(self, runArgs):
        # fill arguments
        try:
            for arg in self._positionalArgs:
                name = arg.name()
                if hasattr(runArgs,name):
                    self.setArgument( name, getattr(runArgs,name) )
        except Exception as e:
            self._jobReport.addError( self._handleException(e) )
        else:
            # Run timing routines
            vTimer.logger = self.logger()
            _n = self.name()
            vTimer.setResultsFile( 'TransformTimer_%s.pickle' % _n )
            vTimer.start( _n )
            # execute with filled arguments
            self.execute()
            vTimer.stop( _n )
            vTimer.writeResultsToFile( 'TransformTimer_%s.pickle' % _n )
        # print out part of the information
        self.dumpReport(self._endReportOptions,self._endReportNotOptions)
        # write all reports
        self.writeReports()
        return self._jobReport

    ## @brief Transforms a dictionary into a list of @c ['KEY=VALUE'].
    #  @details @em RunOptions are represented in the dictionary as @c dic['--ignoreunknown']=''. Only the key is added to the list.
    #  @remarks Generic convenience function used internally only.
    #  @return List of Strings
    def dicToList( self, dic ):
        _l = []
        for key, value in dic.items():
            if value == '':
                _l.append( key )
            else:
                _l.append( key + '=' + str( value ) )
        return _l

    ## Run transformation using arguments from a dictionary and writes final reports to file.
    #  @return JobReport.JobReport instance
    def exeArgDict(self, argDict):
        # First look for run options
        argList=self.dicToList(argDict)
        self._runOpts.extractOptions(argList)
        # Now fill regular arguments
        try:
            for arg in self._positionalArgs:
                name = arg.name()
                if name in argDict:
                    self.setArgument( name, argDict[name] )
        except Exception as e:
            self._jobReport.addError( self._handleException(e) )
        else:
            # Run timing routines
            vTimer.logger = self.logger()
            _n = self.name()
            vTimer.setResultsFile( 'TransformTimer_%s.pickle' % _n )
            vTimer.start( _n )
            # execute with filled arguments
            self.execute()
            vTimer.stop( _n )
            vTimer.writeResultsToFile( 'TransformTimer_%s.pickle' % _n )
        # print out part of the information
        self.dumpReport(self._endReportOptions,self._endReportNotOptions)
        # write all reports
        self.writeReports()
        return self._jobReport

