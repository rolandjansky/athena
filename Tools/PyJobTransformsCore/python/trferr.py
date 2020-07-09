# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

################################################################################
# Exception classes
################################################################################
__all__ = [ 'TransformError', 'TransformDefinitionError', 'TransformArgumentError',
            'TransformEnvironmentError', 'TransformConfigError', 'TransformErrorDiagnoser',
            'TransformValidationError', 'FileError', 'InputFileError', 'OutputFileError', 
            'JobOptionsNotFoundError', 'TransformErrorHandler', 'AthenaLogChecker',
            'TransformThreadTimeout', 'TransformThreadError' ]

import re
import os
from PyJobTransformsCore import fileutil, trfconsts, AtlasErrorCodes, VTimer
from PyJobTransformsCore.TransformLogger import TransformLogger
from AthenaCommon.Include import IncludeError

from future import standard_library
standard_library.install_aliases()

# some constants for shared library loading problems
systemLibs = [ 'libc.so', 'libg2c.so', 'libstdc++.so', 'libshift.so',
               'libpthread.so', 'libm.so', 'libgcc_s.so',
               'libXpm.so', 'libX11.so', 'libXpm.so', 'libX11.so' ]
systemLibsRE = re.compile('|'.join([re.escape(l) for l in systemLibs]))

# severity levels
WARNING=AtlasErrorCodes.WARNING
ERROR=AtlasErrorCodes.ERROR
FATAL=AtlasErrorCodes.FATAL
NO_ERROR=AtlasErrorCodes.NO_ERROR

_libraryRE = r'(?P<library>\S+?)(?:\s|:|$)'
_shLibREs = [ re.compile(r'[cC]ould not load module ' + _libraryRE),
              re.compile(r'[eE]rror (?:in|while) loading shared librar(?:y|ies): ' + _libraryRE ) ]


def examineLoadLibrary(lib):
    """Return tuple (acronym,diagnosis) for library <lib>"""
    from PyJobTransformsCore.envutil import examine_library

    # turn module name into library name
    if not lib.startswith('lib') and not lib.endswith('.so'):
        lib = 'lib' + lib + '.so'
    print ("Examining library " + lib)
    diagLines = []
    errorAcronym = None
    missingSystemLibs = []
    missingOtherLibs = []
    misLibs = examine_library(lib)
    for l in misLibs:
        if systemLibsRE.search(l):
            missingSystemLibs.append(l)
        else:
            missingOtherLibs.append(l)
    if missingSystemLibs:
        if len(missingSystemLibs) == 1: libWord = 'library'
        else:                           libWord = 'libraries'
        diagLines.append( 'Site problem: Missing system %s: %s' % (libWord, ','.join(missingSystemLibs)) )
        if not errorAcronym: errorAcronym = "ATH_SITE_SYSLIBS"

    if missingOtherLibs:
        if len(missingOtherLibs) == 1: libWord = 'library'
        else:                          libWord = 'libraries'
        diagLines.append( 'Can not find %s: %s  Please check software installation.' % (libWord,','.join(missingOtherLibs)) )
        if not errorAcronym: errorAcronym = "ATH_SITE_LIBS"
    return (errorAcronym,os.linesep.join(diagLines))    

def diagnoseAthenaCrash(error):
    """Try to get some more info on an athena crash"""
    haveDiagnosis = False
    diagLines = []
    errorAcronym = None
    stderr = error.stderr
    stdout = error.stdout
    stdall = ''
    if stderr:
        stdall += os.linesep + stderr
    if stdout:
        stdall += os.linesep + stdout
    stdall = stdall.strip()
    for line in stdall.split(os.linesep):
        for libRE in _shLibREs:
            match = libRE.search(line)
            if match:
                lib = match.group('library')
                errorAcronym,diag = examineLoadLibrary(lib)
                if diag: diagLines.append(diag)
                if errorAcronym:
                    error.setCategory(errorAcronym)
                    error.severity = FATAL
                    haveDiagnosis = True
    if diagLines:
        error.diagnosis = os.linesep.join(diagLines)
        error.severity = FATAL
        haveDiagnosis = True
    return haveDiagnosis

def diagnoseGeometryError(error):
    if error.acronym == 'TRF_GEO_MISMATCH':
        error.diagnosis = 'ABORT task and resubmit with the geometryVersion that was used for producing the input file'
        return True
    return False

def diagnoseSharedLibError(error):
    message = error.message
    if not message: return False
    for libRE in _shLibREs:
        match = libRE.search(message)
        if match:
            lib = match.group('library')
            errorAcronym,diag = examineLoadLibrary(lib)
            if diag:
                error.diagnosis = diag
            if errorAcronym:
                error.setCategory(errorAcronym)
                error.severity = FATAL
            return True
    return False


class TransformErrorDiagnoser:
    """Class to diagnose the reasons for errors"""
    def __init__(self):
        self._diagnosers = {}
        # add a bunch of diagnosers
        self.addDiagnoser( '^TRF_ATHENACRASH',      diagnoseAthenaCrash )
        self.addDiagnoser( '^TRF_GEO',              diagnoseGeometryError )
        self.addDiagnoser( '^(ATH|TRF)_?(DLL|MOD)', diagnoseSharedLibError )

    def clear(self):
        """Clear all diagnosers"""
        self._diagnosers.clear()

    def addDiagnoser(self,acronymRE,diag):
        """Add a diagnoser function <diag> to be called if the error acronym matches
        the regular expression <acronymRE>.
        <diag> is a function taking an ErrorInfo as an argument and returning a boolean
        indicating if a diagnosis was found and written into the error object."""
        if type(acronymRE) == str: acronymRE = re.compile(acronymRE)
        self._diagnosers[acronymRE] = diag

    def diagnoseError(self,error):
        """If possible, add a diagnosis to the ErrorInfo <error>.
        Return a boolean indicating whether a diagnosis has been added.
        The first diagnoser that adds a diagnosis (i.e. returns True),
        will be taken and the following ones will not be called."""
        acronym = error.acronym
        #
        # remove POOL and ROOT warning messages from stderr
        #
        if error.stderr:
            stderrNew = []
            stderrChanged = False
            ignorePatterns = [ r"Warning in <TClass::TClass>: no dictionary for class .* is available",
                               r"WARNING: \$POOL_CATALOG is not defined",
                               r"using default `xmlcatalog_file:PoolFileCatalog.xml" ]
            ignoreREs = []
            # some warning messages come asynchonously, therefore remove only the matching part
            for pat in ignorePatterns:
                ignoreREs.append( re.compile(r'(?P<before>^.*?)' + pat + r'(?P<after>.*$)') )
            for line in error.stderr.split(os.linesep):
                newLine = line
                for ignore in ignoreREs:
                    match = ignore.search(newLine)
                    if match:
                        newLine = (match.group('before') or '') + (match.group('after') or '')
                stderrNew.append( newLine )
                if newLine != line: stderrChanged = True
            if stderrChanged:
                error.stderr = os.linesep.join(stderrNew)
        #
        # Try to find a diagnoser for this error
        #
        for acro,diag in self._diagnosers.items():
            if acro.search(acronym):
                print ("Diagnosing error %s with %s" % (acronym,diag.__name__))
                if diag(error): break
        #
        # If no message, then set message to diagnosis (if present) or stderr (if present)
        #
        if not error.message:
            if error.diagnosis: error.message = error.diagnosis
            elif  error.stderr: error.message = error.stderr
        return False


##
# @class AthenaLogChecker
# @brief performs checking for fatals, errors, warnings communicates and error codes in log file produced by Athena application
#
class AthenaLogChecker:
    _eventLoopPat = r'\S*EventLoopMgr\s*  INFO'
    _startOfExecuteRE = re.compile( r'%s.*start of run' % (_eventLoopPat,) )
    _eventNumberRE = re.compile( r'%s.*start of event (?P<event>[0-9]+)' % (_eventLoopPat) )
    _noMoreEventNumberRE = re.compile( r'%s.*Message limit reached' % (_eventLoopPat) )
    _startOfFinaliseRE = re.compile( r'\S+\s*(?:   INFO|  DEBUG|VERBOSE|    ALL).*[Ff]inali[zs]e' )

    ## c'tor
    # @param self "Me, myself and Irene"
    # @param release Atlas software release name 
    def __init__( self, release ):
        self.release = release
        self.reset()
        # read error categories, error patterns and ignore patterns if needed
        if not AtlasErrorCodes.categoriesCount()    : AtlasErrorCodes.readCategories()
        if not AtlasErrorCodes.errorPatternsCount() : AtlasErrorCodes.readErrorPatterns()
        if not AtlasErrorCodes.ignorePatternsCount(): AtlasErrorCodes.readIgnorePatterns()
        self.vTimer = VTimer.VTimer( 'AthenaLogChecker' )

    ## clears all counters
    # @param self "Me, myself and Irene"
    def reset(self):
        """Reset everything for a clean (re)start"""
        self.ignoreCount = 0
        self.errorCount = 0
        self.warningCount = 0
        self.fatalCount = 0
        self.stage = 'initialise'
        self.event = None

    ## increases counter for givern severity 
    # @param self "Me, myself and Irene"
    # @param severity a string, one of ["ERROR", "WARNING", "FATAL"]
    # @param add value for increasing counter, default = 1
    def countError(self,severity,add=1):
        if severity == 'ERROR':
            self.errorCount += add
        elif severity == 'WARNING':
            self.warningCount += add
        elif severity == 'FATAL':
            self.fatalCount += add
                
    ##
    # @param self "Me, myself and Irene"
    # @param filename path to log file to check
    # @param report JobReport instance
    # @param logger a logger instance
    def checkLogFile(self,filename,report,logger=None):
        """Check the logfile <filename> for known errors and add those to JobReport <report>.
        Returns the number of lines checked."""
        self.vTimer.start( '%s log' % report._trf )
        self.reset()
        if logger:
            releaseName = 'ALL'
            if self.release:
                releaseName += ',' + self.release 
            logger.info("Scanning athena logfile %s assuming ATLAS release %s ...",
                        filename, releaseName)
            logger.info("Athena initialise()...")
        logFile = open(filename)
        nLines = 0
        for line in logFile:
            nLines += 1
            line = line.rstrip()
            report.addError( self.processLine( line, logger ) )
        logFile.close()
        if logger:
            logger.info("Done scanning %d lines of file %s. Summary:", nLines, filename)
            logger.info("   Ignored : %d", self.ignoreCount )
            logger.info("   Warnings: %d", self.warningCount )
            logger.info("   Errors  : %d", self.errorCount )
            logger.info("   Fatals  : %d", self.fatalCount )
        self.vTimer.stop( '%s log' % report._trf )
        return nLines

    ##
    # @param self "Me, myself and Irene"
    # @param line line from log to check 
    # @param logger a logger instance or None 
    def processLine(self,line,logger=None):
        """Check line for errors, warnings, fatals and recognised error patterns.
        Return ErrorInfo object if any match is found, and None otherwise"""
        line=line.rstrip()
        # see where we are in the execution
        if self.stage == 'initialise':
            if  AthenaLogChecker._startOfExecuteRE.match(line):
                if logger: logger.info("Athena execute()...")
                self.stage = 'execute'
                return None
        elif self.stage == 'execute':
            if AthenaLogChecker._startOfFinaliseRE.match(line):
                if logger: logger.info("Athena finalise()...")
                self.stage = 'finalise'
                self.event = None
                return None
            match = AthenaLogChecker._eventNumberRE.match(line)
            if match:
                self.event = match.group('event')
                if logger: logger.debug( "Athena event %s" , self.event )
                return None
            if AthenaLogChecker._noMoreEventNumberRE.match(line):
                oldEvent = self.event
                self.event = None
                if logger and oldEvent is not None:
                    logger.debug( "No more event numbers available" )
                return None
        # match ignore patterns
        ignore = AtlasErrorCodes.matchIgnorePattern(line,self.release)
        if ignore:
            if ignore.re.pattern == r'.*?\s+?INFO .+':
                return None
            self.ignoreCount += 1
            if logger:
                logger.debug("ignoring error in line: \"%s\"", line)
                logger.debug("    because it matched: \"%s\"", ignore.re.pattern)
            return None
        # then match known error patterns
        match, err = AtlasErrorCodes.matchErrorPattern(line,self.release)
        if err:
            self.processError(err)
            if logger:
                logger.debug("matched error category %s in line: %s", err.category.acronym, line)
                logger.debug("    because it matched: \"%s\"", match.re.pattern)
            return err
        # finally, perform generic error match
        err = self.extractError(line)
        if err:
            self.processError(err)
            if logger:
                logger.verbose("non-matched error in line: %s", line)
            return err
        return None

    def processError(self,err):
        err.addEvents( self.event )
        err.stage = self.stage
        self.countError(err.severity)

    ## extracs errors, warnings and fatalas from a line 
    # @param self "Me, myself and Irene"
    # @param line string to check
    # @return ErrorInfo instance or None
    def extractError(self,line):
        """For ERROR,WARNING and FATAL messages, return ErrorInfo object
        with who, severity and message field filled. For all other messages
        return None"""
        line=line.rstrip()
        lineREC = re.compile(r"(^\S*\s*(?=WARNING|ERROR|FATAL))(WARNING|ERROR|FATAL)\:?\s+(.+$)")
        match = lineREC.search(line)
        if match:
            who = match.group(1).strip()
            # no more than 2 words in algorithm name
            if ( len( who.split() ) > 2 ): return None
            severity = match.group(2).strip()
            if severity == 'FATAL':
                severity = FATAL
            elif severity == 'ERROR':
                severity = ERROR
            elif severity == 'WARNING':
                severity = WARNING
            message = match.group(3)
            if not who: who = "(Unknown)"
            if message.startswith(':'):
                if len(message) > 1:
                    message = message[1:]
                else:
                    message = ''
            if not message: message = None
            if severity == ERROR or severity == FATAL :
                acronym = 'TRF_UNKNOWN'
            else:
                acronym = None
            # count
            return AtlasErrorCodes.ErrorInfo(acronym=acronym,severity=severity,who=who,message=message)
        else:
            return None


class TransformError( Exception ):
    """Base class for PyJobTransform Exception classes"""
    def __init__(self,message=None,error='TRF_UNKNOWN',**kwargs):
        Exception.__init__(self,error,message)
        self.message = message
        self.error = error
        self.extras = kwargs

    def __str__(self):
        header = '%s: ' % self.error
        msg = header + self.message
        indent = os.linesep + ' '*len(header)
        for n,v in self.extras.items():
            msg += '%s%s=%s' % (indent,n,v)
        return msg

    def setError(self,error):
        self.error = error
        self.args = (self.error,self.message)

    def setMessage(self,message):
        self.message = message
        self.args = (self.error,self.message)


class TransformThreadError( TransformError ):
    def __init__(self,message=None,error='TRF_TIMEOUT',**kwargs):
        TransformError.__init__(self,message,error,**kwargs)


class TransformThreadTimeout( TransformThreadError ):
    pass


class TransformDefinitionError( TransformError ):
    """Exception raised in case of an error in the definition of the transformation"""
    def __init__(self,message=None,error='TRF_DEF',**kwargs):
        TransformError.__init__(self,message,error,**kwargs)


class TransformArgumentError( TransformError ):
    """Exception raised in case of an error in the argument values"""
    def __init__(self,message=None,error='TRF_ARG',**kwargs):
        TransformError.__init__(self,message,error,**kwargs)


class TransformEnvironmentError( TransformError ):
    """Exception raised in case of an error in the run-time environment"""
    def __init__(self,message=None,error='TRF_ENV',**kwargs):
        TransformError.__init__(self,message,error,**kwargs)
        

class TransformConfigError( TransformError ):
    """Exception raised in case of an error in the transform configuration"""
    def __init__(self,message=None,error='TRF_CONFIG',**kwargs):
        TransformError.__init__(self,message,error,**kwargs)


class FileError( TransformArgumentError ):
    def __init__(self,filename,message=None,error='TRF_FILE'):
        TransformArgumentError.__init__(self,message,error) 
        self._filename = filename

    def filename(self):
        return self._filename


class OutputFileError( FileError ):
    def __init__(self,filename,message=None,error='TRF_OUTFILE'):
        mess = "Output file %s" % filename
        if message:
            mess += ' ' + message
        FileError.__init__( self, filename, mess, error )


class InputFileError( FileError ):
    def __init__(self,filename,message=None,error='TRF_INFILE'):
        mess = "Input file %s" % filename
        if message:
            mess += ' ' + message
        FileError.__init__( self, filename, mess, error )


class TransformValidationError( OutputFileError ):
    """Exception raised when transform validation fails"""
    def __init__( self, filename, message=None, error='TRF_OUTFILE_CORRUP' ):
        OutputFileError.__init__( self, filename, message, error )


class JobOptionsNotFoundError( TransformError ):
    """Exception raised in case a joboptions file can not be found"""
    def __init__(self,filename,message=None,error='ATH_JOP_NOTFOUND'):
        mess = "JobOptions file %s not found" % filename
        if message: mess += '. ' + message
        TransformError.__init__(self,mess,error)


class TransformErrorHandler(TransformLogger):
    def __init__(self):
        self._name = self.__class__.__name__
        TransformLogger.__init__(self,self._name)

    def name(self):
        return self._name

    def handleException(self,e):
        """Return a filled ErrorInfo object if the error is handled, or None
        is the error is not handled"""
        # print ("Handling Exception %s %s" % (e.__class__.__name__, e.args))
        #
        # general pre-processing
        #
        # add filename to EnvironmentError for printout
        if isinstance(e,EnvironmentError):
            fn = e.filename
            if fn is not None and fn not in e.args: e.args += (fn,)
        #
        # specific processing
        #
        if isinstance(e,TransformError):      return self.handleTransformError(e)
        elif isinstance(e,IncludeError):      return self.handleIncludeError(e)
        elif isinstance(e,SystemExit):        return self.handleSystemExit(e)
        elif isinstance(e,KeyboardInterrupt): return self.handleKeyboardInterrupt(e)
        elif isinstance(e,RuntimeError):      return self.handleRuntimeError(e)
        elif type(e) in (AttributeError,NameError,TypeError,SyntaxError):
            return self.handlePythonSyntaxError(e)
        elif isinstance(e,Exception):
            if hasattr(e,'args') and type(e.args) == list and e.args:
                args0 = e.args[0]
                # test for some known strings
                if isinstance(args0, str):
                    if args0.find('Failed to load DLL') != -1:
                        return self.handleDllLoadError(e)
        # error was not handled
        return None
                        
    def handleTransformError(self,e):
        err = AtlasErrorCodes.ErrorInfo(acronym=e.error, severity = FATAL, message=e.message)
        # transfer all extras
        for n,v in e.extras.items():
            if hasattr(err,n):
                setattr(err,n,v)
            else:
                err.message += os.linesep + '%s=%s' % (n,v)
        return err

    def handleIncludeError(self,e):
        try:
            joPath = os.environ[trfconsts.JOBOPTIONSPATH]
        except KeyError:
            diag = 'Environment variable %s not defined' % (trfconsts.JOBOPTIONSPATH)
        else:
            diag = 'Not found in %s=%s' % (trfconsts.JOBOPTIONSPATH,joPath)
        return AtlasErrorCodes.ErrorInfo( acronym = 'ATH_JOP_NOTFOUND', severity = FATAL, message = e.args, diagnosis = diag )

    def handleSystemExit(self,e):
        try:
            status = e.args[ 0 ]
        except Exception:
            status = 0
        if status == 0:
            return AtlasErrorCodes.ErrorInfo( acronym = 'OK' )
        return AtlasErrorCodes.ErrorInfo( acronym = 'ATH_FAILURE', severity = FATAL, message = 'SystemExit %s' % status )
        
    def handleKeyboardInterrupt(self,e):
        return AtlasErrorCodes.ErrorInfo( acronym='TRF_KEY_INTERRUPT',
                          severity = FATAL,
                          diagnosis="You pressed the interrupt key on the keyboard (usually CRTL-C)")

    def handlePythonSyntaxError(self,e):
        return AtlasErrorCodes.ErrorInfo( acronym='TRF_PYT_SYNTAX', severity = FATAL, message='%s: %s' % (e.__class__.__name__, e.args) )

    def handleRuntimeError(self,e):
        mess = ''
        if hasattr(e,'args'):
            if type(e.args) == str:
                mess = e.args
            elif type(e.args) in (list,tuple) and type(e.args[0]) == str:
                mess = e.args[0]
        if 'C++ exception' in mess:
            return AtlasErrorCodes.ErrorInfo( acronym='ATH_EXC_CXX', severity = FATAL, message='%s: %s' % (e.__class__.__name__, e.args) )
        return None
    
    def handleDllLoadError(self,e):
        # try to find the guilty one
        import subprocess
        from PyJobTransformsCore.trfutil import TRACEBACK_TEXT, find_in_stack
        from PyJobTransformsCore.envutil import find_library

        mess = None
        diag = None
        dllRE = r"^theApp.Dlls\s*[+]?="
        stack = find_in_stack( dllRE )
        if stack:
            text = stack[TRACEBACK_TEXT]
            dllNameRE = r"([\w\.\-]+)"
            subRE = "%s%s%s%s" % (dllRE,r"\s*\[\s*\"", dllNameRE, r"\"\s*\]")
            dll = re.sub( subRE, r"\1", text )
            lib = 'lib%s.so' % (dll)
            full_lib = find_library(lib)
            mess = 'module %s can not be loaded' % (dll)
            diag = ''
            if not full_lib:
                diag += '%s not found.' % (lib)
            else:
                self.logger().debug( "Found %s. Checking dependencies...", full_lib )
                lddOut = subprocess.getoutput( 'ldd %s' % (full_lib) )
                missLibs = [ ]
                subRE = "%s%s%s" % (r"^\s*",dllNameRE,r"\s+.*not found\s*.*$")
                for line in lddOut:
                    if re.search( "not found", line ):
                        misLib = re.sub( subRE, r"\1", line )
                        missLibs.append( misLib )
                        fullMissLib = find_library(misLib)
                        if fullMissLib:
                            dir = os.path.dirname(fullMissLib)
                            if not fileutil.access( fullMissLib, os.R_OK ):
                                diag += "%s is found in but can not be read from %s." % (fullMissLib,dir)
                                diag += os.linesep
                            else:
                                diag += "%s is found but ldd can not load it from %s." % (misLib,dir)
                                diag += os.linesep
                                if dir.startswith('/afs/'):
                                    diag += "Check your afs cache size, it may be too small."
                if len(missLibs) >= 1:
                    diag += '%s can not be found' % (', '.join(missLibs))
            if diag:
                diag += ' %s=%s' % (trfconsts.LD_LIBRARY_PATH,os.environ[trfconsts.LD_LIBRARY_PATH])
            else:
                diag=None
        return AtlasErrorCodes.ErrorInfo( acronym='ATH_DLL_LOAD', severity = FATAL, message=mess, diagnosis=diag )

#
# end of class TransformErrorHandler:
#
defaultErrorHandler = TransformErrorHandler()
errorHandler = defaultErrorHandler
