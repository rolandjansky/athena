# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import os, sys, re, shutil, glob, time, signal, pickle
import bz2
import functools
import tarfile
import uuid
from builtins import range

from subprocess import Popen, PIPE, STDOUT
from threading import Thread
try:
    TRF_Thread_stop = Thread._Thread__stop
except AttributeError:  # __stop does not exist in Python 3.0
    TRF_Thread_stop = Thread._stop

from .envutil import find_executable, find_file_env, find_files_env
from PyJobTransformsCore import trfconsts, trfenv, fileutil
from PyJobTransformsCore.trferr import TransformValidationError, TransformThreadError, TransformThreadTimeout, JobOptionsNotFoundError, FileError
from PyJobTransformsCore.TransformLogger import TransformLogger
from PyJobTransformsCore.VTimer import vTimer
from PyUtils import RootUtils

try:
    import PyDumper.SgDumpLib as sdl
except ImportError:
    print ("Unable to import PyDumper.SgDumpLib.")
    sdl = None

VALIDATION_DICT = { 'ALL' : None, 'testIfEmpty' : None, 'testIfNoEvents' : None, 'testIfExists' : None, 'testIfCorrupt' : None, 'testCountEvents' : None, 'extraValidation' : None, 'testMatchEvents' : None, 'testEventMinMax' : None , 'stopOnEventCountNone' : None, 'continueOnZeroEventCount' :None}

TRF_SETTING = { 'testrun' : False, 'validationTimeout' : 600, 'validationRetry' : 2, 'validationSleepInterval' : 10, 'TRFTimeout' : 600, 'TRFRetry' : 2, 'TRFSleepInterval' : 10 }

LFN_VER_DICT = {}

_PFNPat = re.compile( r'^(?P<url>\S*?)(?P<lfn>[A-Za-z0-9\.\-\_]+?)(?P<ver>\.\d+)?$' )

_defaultSignalHandlerDict = {}

class TrfAlarm(Exception):
    '''Exception to break out of blocking calls when an alarm is raised'''
    pass 

def alarmHandler(signum, frame):
    '''Signal handler for SIGALRM - raise alarm exception'''
    raise TrfAlarm

## ItemInList, AddValidItemToList & OverlapLists: two basic helper functions for lists of strings
def ItemInList(item,aList):
    if not isinstance(aList, list):
        raise TypeError("ItemInList() does not support aList of type %s"%type(aList))

    isInList=False
    for i in aList:
        if i==item:
            isInList=True
    return isInList

def OverlapLists(List1, List2):
    for i in range(0,List1.__len__()):
        if List1[i] in List2:
            return True
    return False

def AddValidItemToList(item,aList):
    #Recursive loop if item is a list
    if isinstance(item,list):
        for i in item:
            AddValidItemToList(i,aList)
    #Add to list if item is a string and not already in the list
    elif isinstance(item, str):
        if not ItemInList(item,aList):
            aList += [item]
    else:
        raise TypeError("AddValidItemToList() does not support item of type %s"%type(item))
    return 

## @brief Install common handler for various signals.
#  @details All existing signal handlers are saved in the @em _trfSignalHandlerDict dictionary to allow for them to be restored.
#  @param handler Common signal handler for all signals concerned.
#  @warning Signal handling in Python is not 100% implemented. Certain signal (even those listed below) cannot be caught.
#  @return None
def setTrfSignalHandler( handler ):
    xDict = {}
    if not _defaultSignalHandlerDict: # default handlers have not been saved
        xDict = _defaultSignalHandlerDict
    for s in [ 'SIGABRT', 'SIGFPE', 'SIGBUS', 'SIGHUP', 'SIGILL', 'SIGINT', 'SIGIO', 'SIGPIPE', 'SIGQUIT', 'SIGSEGV', 'SIGSYS', 'SIGTERM', 'SIGXCPU', 'SIGXFSZ' ]:
        try:
            xDict[ s ] =  signal.signal( getattr( signal, s ), handler )
        except Exception as e:
            print ("Unable to attach custom signal handler to %s: %s" % ( s, e ))
            continue
    return xDict

## @brief Restore signal handlers to the default ones
#  @details Handlers are restored from @em _defaultSignalHandlerDict dictionary.
#  @warning Signal handling in Python is not 100% implemented. Certain signal (even those listed below) cannot be caught.
#  @return None
def setDefaultSignalHandlers():
    if _defaultSignalHandlerDict:
        currentTRFSignalHandler = signal.getsignal( signal.SIGTERM )
    else:
        currentTRFSignalHandler = signal.SIG_DFL
    currentTRFSignalHandler = signal.getsignal( signal.SIGTERM )
    for s in [ 'SIGABRT', 'SIGFPE', 'SIGBUS', 'SIGHUP', 'SIGILL', 'SIGINT', 'SIGIO', 'SIGPIPE', 'SIGQUIT', 'SIGSEGV', 'SIGSYS', 'SIGTERM', 'SIGXCPU', 'SIGXFSZ' ]:
        try:
            signal.signal( getattr( signal, s ), _defaultSignalHandlerDict.get( s, signal.SIG_DFL ) )
        except Exception as e:
            print ("Unable to attach custom signal handler to %s: %s" % ( s, e ))
            continue
    return currentTRFSignalHandler

## @brief List all processes and parents and form a dictionary where the 
#  parent key lists all child PIDs
def getAncestry():
    psCmd = ['ps', 'a', '-o', 'pid,ppid', '-m']

    try:
        print ('Executing %s' % psCmd)
        p = Popen(psCmd, stdout=PIPE, stderr=PIPE)
        stdout = p.communicate()[0]
        psPID = p.pid
    except OSError as e:
        print ('Failed to execute "ps" to get process ancestry: %s' % repr(e))
        raise
    
    childDict = {}
    for line in stdout.split('\n'):
        try:
            (pid, ppid) = line.split()
            pid = int(pid)
            ppid = int(ppid)
            # Ignore the ps process
            if pid == psPID:
                continue
            if ppid in childDict:
                childDict[ppid].append(pid)
            else:
                childDict[ppid] = [pid]
        except ValueError:
            # Not a nice line
            pass
    return childDict

## @brief Find all the children of a particular PID (calls itself recursively to descend into each leaf)
#  @note  The list of child PIDs is reversed, so the grandchildren are listed before the children, etc.
#         so signaling left to right is correct 
def listChildren(psTree = None, parent = os.getpid()):
    '''Take a psTree dictionary and list all children'''
    if psTree is None:
        psTree = getAncestry()
    
    children = []
    if parent in psTree:
        children.extend(psTree[parent])
        for child in psTree[parent]:
            children.extend(listChildren(psTree, child))
    children.reverse()
    return children


## @brief Kill all PIDs 
def infanticide(childPIDs, sleepTime = 3):
    print ('Will kill these processes: %s' % childPIDs)
    for pid in childPIDs:
        try:
            os.kill(pid, signal.SIGINT)
        except OSError:
            pass
        
    time.sleep(sleepTime)
        
    for pid in childPIDs:
        try:
            os.kill(pid, signal.SIGKILL)
        except OSError:
            pass


## @brief Decorator to dump a stack trace when hit by SIGUSR
def sigUsrStackTrace(func):
    import signal
    import traceback
    
    class SigUsr1(Exception):
        pass
    
    def sigHandler(signum, frame):
        print ('Handling signal %d in sigHandler' % signum)
        raise SigUsr1
    
    def signal_wrapper(*args, **kwargs):
        signal.signal(signal.SIGUSR1, sigHandler)
        
        try:
            return func(*args, **kwargs)
        
        except SigUsr1:
            print ('Transform received SIGUSR1. Exiting now with stack trace...')
            traceback.print_exc()
            myChildren = listChildren()
            if myChildren:
                print ('Will now try to kill child PIDs: %s' % myChildren)
                infanticide(myChildren)
            sys.exit(128 + signal.SIGUSR1)  
            
    functools.update_wrapper(signal_wrapper, func)
    return signal_wrapper



# In case things get stuck here, implement the SIGUSR stack trace wrapper
@sigUsrStackTrace
def timelimited_exec1( tl_func, tl_timeout = TRF_SETTING[ 'TRFTimeout' ], tl_retry = TRF_SETTING[ 'TRFRetry' ], tl_interval = TRF_SETTING[ 'TRFSleepInterval' ], **tl_func_kwargs ):
    if tl_timeout <= 0:
        raise ValueError( "tl_timeout parameter must be a positive number of seconds." )
    if tl_retry <= 0:
        raise ValueError( "tl_retry parameter must be a positive number." )
    while tl_retry >= 0:
        signal.signal(signal.SIGALRM, alarmHandler)
        signal.alarm(int(tl_timeout))
        shortCmd = tl_func.replace('\n', '\\n')
        if len(shortCmd) > 80:
            shortCmd = shortCmd[:77] + '...'
        print ('Calling "%s" with timeout %ds' % (shortCmd, tl_timeout))
        p = Popen(tl_func,shell=True,stdout=PIPE,stderr=STDOUT, preexec_fn = lambda : os.setpgid(0,0))
        stdout = stderr = '' # Make sure these variables are valid.
        try:
            (stdout, stderr) = p.communicate()
            signal.alarm(0)
            rc = p.returncode
        except TrfAlarm:
            # Timeout on command happened
            print ('Time limited exec command expired')
            # Kill the subshell and all spawned children 
            myChildren = listChildren()
            infanticide(myChildren)
            p.poll()
            if p.returncode is None:
                # Error - set some fallback value for rc
                rc = -signal.SIGALRM
            else:
                rc = p.returncode
        if rc == 0:
            break
        if tl_retry == 0: # success or no more retries.
            print ("Maximum retry attempts exhausted!")
            break
        print ('Retrying %d more time(s).' % tl_retry)
        tl_retry -= 1
        print ('Waiting %d secs before retry.' % tl_interval)
        time.sleep( tl_interval )
        tl_timeout *= 1.5
        print ('Increasing timeout duration to %d.' % tl_timeout)
    return rc, stdout

## Execute a function, allowing it to run for i secs before raising exception.
#  j retries permitted with sleep interval k secs.
def timelimited_exec( tl_func, tl_func_args = (), tl_pre_func = lambda:None, tl_post_func = lambda:None, tl_timeout = TRF_SETTING[ 'TRFTimeout' ], tl_retry = TRF_SETTING[ 'TRFRetry' ], tl_interval = TRF_SETTING[ 'TRFSleepInterval' ], **tl_func_kwargs ):
    class TOThread( Thread ):
        def __init__( self ):
            Thread.__init__( self )
            self.returnVal = None
            self.error = None

        def run( self ):
            try:
                try:
                    tl_pre_func()
                except Exception as err_pre:
                    print ("%s failed: %s" % ( repr( tl_pre_func ), err_pre.message ) )
                self.returnVal = tl_func( *tl_func_args, **tl_func_kwargs )
                try:
                    tl_post_func()
                except Exception as err_post:
                    print ("%s failed: %s" % ( repr( tl_post_func ), err_post.message ) )
            except Exception as err_func:
                self.error = err_func

        def stop( self ):
            self.error = TransformThreadTimeout( '%s took more than the permitted %d secs.' % ( repr( tl_func ), tl_timeout ) )
            try:
                tl_post_func()
            except Exception as err_post:
                print ("%s failed in stop(): %s" % ( repr( tl_post_func ), err_post.message ) )
            TRF_Thread_stop( self )

    if tl_timeout < 0:
        raise ValueError( "tl_timeout parameter must be a positive number of seconds." )
    if tl_retry < 0:
        raise ValueError( "tl_retry parameter must be a positive number." )
#    print ('Disabling transform signal handler.')
    currentTRFSignalHandler = setDefaultSignalHandlers()
    while tl_retry >= 0:
        tl_thread = TOThread()
        tl_thread.start()
        tl_thread.join( tl_timeout )
        if tl_thread.isAlive():
            print ('%s took more than the permitted %d secs.' % ( repr( tl_func ), tl_timeout ))
            try:
                tl_thread.stop()
            except Exception as err:
                if tl_retry == 0:
#                    print ('Re-enabling transform signal handler.')
                    setTrfSignalHandler( currentTRFSignalHandler )
                    raise TransformThreadError( 'Exception caught whilst attempting to stop %s: %s' % ( repr( tl_func ), err ) )
                else:
                    print ('Exception caught whilst attempting to stop %s.' % repr( tl_func ))
                    tl_thread.error = TransformThreadError( 'Exception caught whilst attempting to stop %s: %s' % ( repr( tl_func ), err ) )
        # Thread has stopped
        if tl_thread.error is None:
            x = tl_thread.returnVal
            del tl_thread
#            print ('Re-enabling transform signal handler.')
            setTrfSignalHandler( currentTRFSignalHandler )
            return x
        # An error was detected
        if tl_retry == 0: # No more retry
            x = tl_thread.error
            del tl_thread
#            print ('Re-enabling transform signal handler.')
            setTrfSignalHandler( currentTRFSignalHandler )
            raise x
        else: # Continue retrying
            print ('Retrying %d more time(s).' % tl_retry)
            tl_retry -= 1
            del tl_thread
        print ('Waiting %d secs before retry.' % tl_interval)
        time.sleep( tl_interval )
        tl_timeout *= 1.5
#    print ('Re-enabling transform signal handler.')
    setTrfSignalHandler( currentTRFSignalHandler )

def validGUID(testString):
    if re.match(r'[\dA-Fa-f]{8}-[\dA-Fa-f]{4}-[\dA-Fa-f]{4}-[\dA-Fa-f]{4}-[\dA-Fa-f]{12}$', testString):
        return True
    else:
        return False
    

def getGUIDfromPFC(filename):
    '''Retrieve a file GUID from the PFC. Returns a tuple with error code and guid (if found)'''
    guid = None
    catalog = os.environ.get('POOL_CATALOG')
    if not catalog:
        catalog = RootTTreeFile.defaultCatalog
        os.environ['POOL_CATALOG'] = catalog
    poolcmd = ['FClistGUID']
    if not find_executable(poolcmd[0]):
        raise EnvironmentError('Pool utility %s not found in PATH' % poolcmd[0] )
    if filename.startswith('LFN:'):
        poolcmd.extend(['-l', filename[4:]])
    else:
        poolcmd.extend(['-p', filename])
    print ('Using %s for GUID retrieval' % poolcmd)
    p = Popen(poolcmd, shell=False, stdout=PIPE, stderr=PIPE, close_fds=True)
    (stdout, stderr) = p.communicate()
    for line in stdout.split(os.linesep):
        line = line.strip()
        if validGUID(line):
            guid = line
            break
    if p.returncode != 0:
        print ("GUID retrieval failed: %s" % stderr)
        return (1, None)
    if guid is None:
        print ('Did not find GUID in catalog %s (usually harmless)' % catalog)
        return (0, None)
    print ("GUID retrieval: %s (%s) found in %s" % ( guid, filename, catalog ))
    return (0, guid)


def addGUIDtoPFC(filename, guid, type = 'ROOT_All'):
    '''Insert a GUID into the PFC. Returns standard error code'''
    catalog = os.environ.get('POOL_CATALOG')
    if not catalog:
        catalog = RootTTreeFile.defaultCatalog
        os.environ['POOL_CATALOG'] = catalog
    poolcmd = ('FCregisterPFN', '-p', filename, '-t', type, '-g', guid)
    if not find_executable(poolcmd[0]):
        raise EnvironmentError('Pool utility %s not found in PATH' % poolcmd[0] )
    print (poolcmd)
    p = Popen(poolcmd, shell=False, stdout=PIPE, stderr=PIPE, close_fds=True)
    (stdout, stderr) = p.communicate()
    if p.returncode != 0:
        print ("GUID insertion failed: %s" % stderr)
        return 1
    print ("Added GUID %s for file %s to %s" % ( guid, filename, catalog ))
    return 0


def StringToList(cmd):
    valList=None
    if isinstance(cmd,list):
        valList=cmd
    else:
        try:
            valList=cmd.split(',,')
        except Exception:
            raise ValueError("StringToList cannot interpret '%s' as a list."%str(cmd))
    return valList

def ntup_entries(fname, tree_names):
    """Count events in ROOT-based files."""
    if TRF_SETTING[ 'testrun' ] and ( VALIDATION_DICT[ 'ALL' ] is False or VALIDATION_DICT[ 'testCountEvents' ] is False ):
        print ('Test run in progress. Event count (ROOT-based) disabled.')
        return None
    #work with string or list of strings
    if not isinstance(tree_names,list):
        tree_names=[tree_names]
    if len( tree_names ) == 0:
        return None
    cmdSnippet = os.linesep.join( [
        "from __future__ import print_function",
        "import PyUtils.RootUtils as ru",
        "import pickle",
        "root = ru.import_root()",
        "tree_names = %s" % tree_names,
        "nevts = None",
        "f = root.TFile.Open( '%s', 'READ' )" % fname,
        "if isinstance( f, root.TFile ) and f.IsOpen():",
        "    for tname in tree_names:",
        "        t = f.Get( tname )",
        "        if isinstance( t, root.TTree ):",
        "            nevts = t.GetEntriesFast()",
        "        del t",
        "        if nevts:",
        "            break",
        "    f.Close()",
        "del f",
        "if nevts is None:",
        "    exit( 1 )",
        "print ('<nevts>%s</nevts>' % pickle.dumps( nevts ))",
        "exit( 0 )" ] ) + os.linesep
    rc, stdout = timelimited_exec1( 'python -c "%s"' % cmdSnippet )
    if rc == 0:
        found = re.search( "<nevts>(.*?)</nevts>", stdout, re.DOTALL )
        if found:
            result = pickle.loads( found.group( 1 ) )
            return result
    print (stdout)
    return None

def ElementToExec(cmd):
    #this method is used by preExec, postExec and rec.UserExecs
    #it handles special characters not allowed by production systems as followed:
    # blank space   --> @
    # double-quote  --> `
    #in addition corrects erroneous over-use of quotes by job transform users
    if cmd.find(',,') > -1:
        print ("WARNING symbol ,, is not expected in StringToExec. There is probably a mistake in the configuration.",cmd)
    if cmd.startswith("'") and cmd.endswith("'"):
        print ("WARNING - suspicious exec syntax. Removing superfluous quotation signs.")
        print ("original:", cmd)
        cmd=cmd.strip("'")
        print ("new:",cmd)
    if cmd.startswith('"') and cmd.endswith('"'):
        print ("WARNING - suspicious exec syntax. Removing superfluous quotation signs.")
        print ("original:", cmd)
        cmd=cmd.strip('"')
        print ("new:",cmd)
    newCmd=cmd.replace('@',' ')
    newCmd=newCmd.replace('`','"')
    if cmd!=newCmd:
        print ("INFO StringToExec changed original: %s  to new: %s"%(cmd,newCmd))
    return newCmd

def StringToExec(cmd):
    #first split create a list from string splitted by ,,
    #then interpret each element to overcome the limitations of the production systems
    valList=StringToList(cmd)
    valOut=[]  
    for iElement in valList:
        newElement=ElementToExec(iElement)
        valOut.append(newElement)
    return valOut

def strip_suffix( aString, aSuffix ):
    """ Remove aSuffix from the end of aString (if present)."""
    if not aSuffix: return aString
    if aString.endswith( aSuffix ): aString = aString[:-len(aSuffix)]
    return aString

def strip_prefix( aString, aPrefix ):
    """Remove aPrefix from the beginning of aString (if present)."""
    if not aPrefix: return aString
    if aString.startswith( aPrefix ): aString = aString[len(aPrefix):]
    return aString

def remove_filename_extension( aString ):
    """Remove everything from <aString> starting from the last . (dot) after
    the last path separator ('/' or '\') if any. If <aString> does not have a dot
    or if it ends with a path separator, then nothing is removed."""
    slash = aString.rfind(os.sep)
    dot = aString.rfind(os.extsep,slash+1)
    if slash == len(aString) - 1: return aString
    if dot == -1: return aString
    return aString[:dot]

def tail( fileOrName, nLines ):
    """Return as a string, the list <nLines> lines of file <fileOrName>.
    If <filename> is a string, it is considered a filename. If it is
    File object, it will be read from starting at the point where it is,
    and will not be closed."""
    logLines = []
    closeFile = False
    if type(fileOrName).__name__ == 'str':
        logFile = open(fileOrName,'r')
        closeFile = True
    else:
        logFile = fileOrName
    for line in logFile:
        logLines.append(line.rstrip())
        if len(logLines) > nLines:
            logLines.pop(0) # remove first line
    if closeFile:
        logFile.close()
    return os.linesep.join(logLines)

def examine_sh_exit_status(status):
    """Return a string describing the exit status, if any it to be given.
    Otherwise return None"""
    sigNum = None
    if status > 128:
        sigNum = status - 128
    elif status < 0:
        sigNum = -status
    if sigNum is not None:
        import signal
        for sigName in dir(signal):
            if sigName.startswith('SIG') and not sigName.startswith('SIG_'):
                sigValue = getattr(signal,sigName)
                if sigValue == sigNum:
                    return sigName
    return None

def find_datafile( filename ):
    return find_file_env( os.path.expanduser( os.path.expandvars(filename) ),
                          trfconsts.DATAPATH )

def find_joboptions( jobOptionsFile, depth=0 ):
    return find_file_env( os.path.expanduser( os.path.expandvars( jobOptionsFile ) ),
                          trfconsts.JOBOPTIONSPATH, sep=',', depth=depth )

def get_atlas_release(project='AtlasProduction'):
    """Determine the Atlas Release used"""
    atlasRelease = None
    #
    # guess from directory of this file
    #
    # make a list of patterns to match
    nightlyBugFind      = os.sep + os.path.join('nightlies','bugfix',project,r'rel_(?P<nightly>[0-6])') + os.sep
    nightlyBugReplace   = r'bug_\g<nightly>'
    nightlyDevFind      = os.sep + os.path.join('nightlies','dev',project,r'rel_(?P<nightly>[0-6])') + os.sep
    nightlyDevReplace   = r'dev_\g<nightly>'
    nightlyBuildFind    = os.sep + os.path.join('nightlies','projects',project,r'rel_(?P<nightly>[0-6])') + os.sep
    nightlyBuildReplace = r'rel_\g<nightly>'
    releaseFind         = os.sep + os.path.join(project,r'(?P<release>[0-9][0-9]?(?:\.[0-9][0-9]?){2,3})') + os.sep
    releaseReplace      = r'\g<release>'
    patterns = { nightlyBugFind   : nightlyBugReplace ,
                 nightlyDevFind   : nightlyDevReplace ,
                 nightlyBuildFind : nightlyBuildReplace ,
                 releaseFind      : releaseReplace }
    dir = os.path.dirname( os.path.realpath(__file__) )
    for find,replace in patterns.items():
        match = re.search(find,dir)
        if match:
            atlasRelease = match.expand(replace)
            print ("Got Atlas Release number %s from directory name of file %s" % (atlasRelease,__file__))
            return atlasRelease
    # 
    # If not found, try some environment variables
    #
    for env in ['ATLAS_RELEASE','AtlasVersion']:
        atlasRelease = os.environ.get(env)
        if atlasRelease:
            print ("Got Atlas Release number %s from environment variable %s" % (atlasRelease,env))
            return atlasRelease
    return None

def linkPresent( filename, findBrokenLink = True ):
    """Check if the given filename path contains a symlink. This function also checks all it's parent directories. 
    If findBrokenLink is True, the first path component that is a symlink and is broken is returned. 
    If findBrokenLink is False, the first symlink in the path in returned."""
    if os.path.isabs( filename ):
        initial_p = ""
        p = os.path.commonprefix( [ os.getcwd(), filename ] )
        filename = filename[ len( p ) + 1: ] # name filename a relative path wrt p.
    else:
        p = os.getcwd()
        initial_p = p + os.sep
    intermediateList = filename.split( os.sep )
    while intermediateList:
        p = os.sep.join( [ p, intermediateList.pop(0) ] )
        if os.path.islink( p ):
            if findBrokenLink and os.path.exists( p ):
                continue
            return p.lstrip( initial_p )
    return ''

def isOnLocalFileSystem( filename ):
    """Check in the filename at every directory level to determine if any of its parent directories have different mount points."""
    filename = os.path.realpath( filename )
    cPrefix = os.path.commonprefix( [ os.getcwd(), filename ] )
    intermediateDirList = os.path.dirname( filename[ len( cPrefix ): ] ).split( os.sep )
    cPrefix = cPrefix.rstrip( os.sep )
    while intermediateDirList:
        cPrefix = os.sep.join( [ cPrefix, intermediateDirList.pop(0) ] )
        if os.path.ismount( cPrefix ):
            return False
    return True

def get_files( listOfFiles, fromWhere='data', doCopy='ifNotLocal', errorIfNotFound=True, keepDir=True, depth=0, sep=os.pathsep ):
    """Copy or symlink a list of files given from a search path given in an environment variable.
    <listOfFiles> is either as a python list of strings, or as a comma separated list in one string.
                  Each entry in the list can contain wildcards (a la unix commands)
    <fromWhere>: name of the environment variable containing the search paths. Available shortcuts: 'data' -> 'DATAPATH'.
    <doCopy>='Always': copy the file
            ='Never' : symlink the file
            ='ifNotLocal': symlink the file if the original is in a subdirectory of the run directory, otherwise copy.
    <errorIfNotFound>=True: An EnvironmentError exception is raised if any filename in the list is not found.
                            If the filename contains wildcards, it is considered not found if no files are found at all.
                     =False: If a file is not found, only a warning is printed
    <keepDir>=True : copy the file into a local subdirectory if the requested filename includes its directory part
              False: copy the file in the current directory with the requested filename while ignoring its directory part
              If a filename in <listOfFiles> contains an absolute path, the directory part is ignored in the copied filename.
    """
    if doCopy not in [ 'Always', 'Never', 'ifNotLocal' ]:
        print ("doCopy value of %s not recognised. Resetting it to 'ifNotLocal'" % doCopy)
        doCopy = 'ifNotLocal'
    fromWhereShorts = { 'data' : 'DATAPATH' }
    # split a comma separated list of files given in a string into a python list
    if isinstance( listOfFiles, str ):
        listOfFiles = listOfFiles.split(',')
    # origin of files
    fromPath = fromWhereShorts.get( fromWhere, fromWhere )
    # copy or symlink
    if doCopy == 'Always':
        copy_func = shutil.copyfile
    elif doCopy == 'Never':
        copy_func = os.symlink
    else: # doCopy == ifNotLocal
        copy_func = None
    fileDict = {}
    # Check if srcFileList contains entries from different directories
    realDirList = []
    symLinkList = []
    # First iteration of listOfFiles to create realDirList.
    # Store created lists for subsequent iteration without having to recreate the lists again.
    for filename in listOfFiles:
        filename = os.path.expanduser( os.path.expandvars( os.path.normpath( filename ) ) )
        srcFileList = find_files_env( filename, fromPath, sep=sep, depth=depth )
        fileDict[ filename ] = srcFileList
        if os.path.isabs( filename ):
            continue
        srcFilePrefix = os.path.dirname( os.path.commonprefix( srcFileList ) ) #.rstrip( os.sep )
        dirName = os.path.dirname( filename )
        # Test if dirName is present at the right end of srcFilePrefix. 
        # If this is not the case, this dirName should not be symlinked but a physical dirName s
        # hould be created instead (later).
        try:
            realDirList.append( symLinkList.pop( symLinkList.index( dirName ) ) )
        except ValueError:
            pass
        else:
            continue
        print ("srcFilePrefix = %s, dirName = %s" % ( srcFilePrefix, dirName ))
        try:
            srcFilePrefix.rindex( dirName, len( srcFilePrefix ) - len( dirName ) )
        except ValueError:
            realDirList.append( dirName )
        else:
            symLinkList.append( dirName )
    del symLinkList
    # Main iteration
    for filename, srcFileList in fileDict.items():
        if not srcFileList:
            if errorIfNotFound:
                raise EnvironmentError('Auxiliary file %s not found in %s' % (filename,fromPath) )
            else:
                print ("WARNING: auxiliary file %s not found in %s" % (filename,fromPath))
        parentDirLinked = None
        for srcFile in srcFileList:
            # Test if the parent directory of the current entry in srcFileList has been symlinked. 
            # If so, skip this entry. Assumes that the find_files_env() returns a sorted list of files.
            if os.path.dirname( srcFile ) == parentDirLinked:
                print ("%s has been symlinked." % parentDirLinked)
                continue
            # determine name of destination file
            if keepDir and not os.path.isabs( filename ):
                subdir = os.path.dirname( filename )      # e.g. subdir = geomDB
                targetFile = os.path.abspath( os.path.join( subdir, os.path.basename( srcFile ) ) ) # e.g. targetFile = $CWD/geomDB/actualfile.db
                if subdir == os.getcwd(): 
                    subdir = ''
            else: # keepDir == False or given filename is an absolute path. 
                targetFile = os.path.abspath( os.path.basename( srcFile ) ) # e.g. targetFile = $CWD/actualfile.db
                subdir = ''
            # associate copy_func with either shutil.copyfile or os.symlink
            if doCopy == 'ifNotLocal':
                if isOnLocalFileSystem( srcFile ):
                    copy_func = os.symlink
                else: # remote file
                    print ("%s is on a different mount point as $CWD." % srcFile)
                    copy_func = shutil.copyfile
            realDirRequired = False
            if copy_func is os.symlink:
                # redefine targetFile and srcFile to link subdirectory instead. 
                # However, if subdir needs to be a real directory due to multiple distinct contributing 
                # source directories, the reassignment is not done.
                if subdir:
                    if subdir in realDirList: 
                        print ("%s found in realDirList: %s" % ( subdir, realDirList ))
                        realDirRequired = True
                    else:
#                        print ("Reassigning targetFile from %s to %s." % ( targetFile, subdir ))
                        targetFile = subdir
                        srcFile = os.path.dirname( srcFile ).rstrip( os.sep )
                        # set flag to Stop iterating over srcFileList since parent dir has (will be) symlinked.
                        parentDirLinked = srcFile
            # create the directory to contain the required srcFiles
            if copy_func is shutil.copyfile or realDirRequired:
                if subdir and not os.path.isdir( subdir ):
                    brokenLink = linkPresent( subdir, findBrokenLink = True )
                    if brokenLink:
                        try:
                            print ("Attempting to remove broken symlink %s" % brokenLink)
                            os.remove( brokenLink )
                        except OSError as x:
                            raise EnvironmentError( 'Unable to create the directory %s as the broken symlink %s cannot be removed: %s' % ( subdir, brokenLink, x ) )
#                        targetFile = 
                    os.makedirs( subdir )
            # Check existence of targetFile (side effect of an exception when running os.path.samefile).
            try:
                isSameFile = os.path.samefile( srcFile, targetFile )
            except OSError: # good. targetFile does not exist.
#                print ("%s does not exist. %s" % ( targetFile, x ))
                if os.path.islink( targetFile ): # broken symlink
                    try:
                        print ("*Attempting to remove %s" % targetFile)
                        os.remove( targetFile )
                    except OSError as x:
                        raise EnvironmentError( 'Unable to remove broken symlink %s: %s' % ( targetFile, x ) )
                try:
                    copy_func( srcFile, targetFile )
                except OSError:
                    # try copying instead.
                    shutil.copyfile( srcFile, targetFile )
                continue
            # Test if srcFile and targetFile are one and the same
            # NB: If the requested file is already in $CWD, the boolean flag isSameFile is always going 
            # to be True since a valid, existing file/symlink is given priority. This is so even if the 
            # existing file is a symlink and resides on a different mount point. The responsibility lies 
            # on user to be aware of the potential problems of accessing files over different mount points.
            if isSameFile:                
                print ("%s is the same as %s. No further action." % ( srcFile, targetFile ))
                continue # do nothing
            print ("%s is not the same as %s" % ( srcFile, targetFile ))
            # This point is reached in only two cases:
            # 1) An absolute filename was used.
            #    targetFile is not what we want as since the function was called to get a file with an absolute filename. 
            #    Attempt to remove the existing file before performing the copy.
            try:
                print ("**Attempting to remove %s" % targetFile)
                os.remove( targetFile ) # remove files and symlinks
            except Exception: # dst file is a directory
                for _root, _dirs, _files in os.walk( targetFile , topdown = False ):
                    for name in _files:
                        os.remove( os.path.join( _root, name ) )
                    for name in _dirs:
                        os.rmdir( os.path.join( _root, name ) )
                os.rmdir( targetFile )
            try:
                copy_func( srcFile, targetFile )
            except OSError:
                # try copying instead.
                shutil.copyfile( srcFile, targetFile )


TRACEBACK_FILENAME = 0
TRACEBACK_LINENUMBER = 1
TRACEBACK_FUNCTION = 2
TRACEBACK_TEXT = 3

def find_in_stack( whatRE, where = TRACEBACK_TEXT, stackList = None ):
    if stackList is None:
        import traceback
        stackList = traceback.extract_tb(sys.exc_info()[2])
    for s in stackList:
        if re.search( whatRE, s[where] ):
            return s
    return None

def load_module( module_filename ):
    """Extension of dynamic import. Load as module a python file with any path in the filename.
    File does not have to be in PYTHONPATH"""
    # mold the filename
    module_filename = os.path.normpath(module_filename)
    # remove .py suffix
    if module_filename.endswith('.py'): module_filename = module_filename[:-3]
    # explicit current dir
    if module_filename.startswith( os.curdir + os.sep ): module_filename = module_filename[2:]
    # updir somewhere
    if module_filename.startswith( os.pardir + os.sep ): module_filename = os.path.abspath( module_filename )
    # first try to load as is
    try:
        if os.path.isabs(module_filename): raise ImportError
        dotted = module_filename.replace(os.sep,'.')
#        print ("load_module(): trying import %s" % dotted)
        module = __import__( dotted )
        modlist = dotted.split('.')
        if len(modlist) > 1:
            for submod in modlist[1:]:
                module = getattr(module,submod) 
    except ImportError:
        # add path to system
        moddir,modname = os.path.split( module_filename )
        absdir = os.path.abspath( moddir )
        if moddir != os.curdir:
            # add module directory to search path
            inserted = False
            if absdir not in sys.path:
                sys.path.insert( 1, absdir )
                inserted = True
#                print ("load_module(): new path: %s" % sys.path)
#            print ("load_module(): trying import %s" % modname)
            module = __import__( modname )
            # cleanup search path
            if inserted: sys.path.remove( absdir )
        else:
            module = __import__( modname )
    return module

def load_transforms( trf_py, nameRE = r".*" ):
    """Return a list of JobTransform objects found in python file <trf_py>,
    whose names match the regular expression <nameRE>. Default is all objects in file."""
    # import transformation as module
    module = load_module( trf_py )
    trfs = [ ]
    for t in dir(module):
        obj = getattr(module,t)
        if isinstance(obj,module.JobTransform) and re.search( nameRE, obj.name() ):
            trfs.append( obj )
    return trfs

def load_errorhandlers( handler_py, nameRE = r".*" ):
    """Load a list of TransformErrorHandler objects found in python file handler_py,
    whose names (=class names) match the regular expression <nameRE>. Default is all objects in file."""
    # import errorhandler as module
    module = load_module( handler_py )
    handlers = [ ]
    for t in dir(module):
        obj = getattr(module,t)
        if isinstance(obj,module.TransformErrorHandler) and re.search( nameRE, obj.name() ):
            handlers.append( obj )
    return handlers

def genLFN_PFN():
    """Generates mapping between LFN and PFN. Assumes LFN and PFN are related by filename."""
    if not os.environ.get('POOL_CATALOG'):
        os.environ['POOL_CATALOG'] = PoolDataFile.defaultCatalog
    # Look inside the PoolFileCatalog
    listPFNcmd = 'FClistPFN'
    if not find_executable( listPFNcmd ):
        raise Exception( 'Required PFN extraction utility not found.' )
    # Retrieve PFNs to populate LFN_VER_DICT
    p = Popen(listPFNcmd,shell=True,stdout=PIPE,stderr=PIPE,close_fds=True)
    while p.poll() is None:
        p.stderr.readline()
    if p.returncode != 0:
        raise Exception( 'PFN retrieval failed. LFN/PFN map cannot be generated.' )
    for line in p.stdout:
        line = line.strip()
        if line:
            PFNPatResult = _PFNPat.search( line )
            if PFNPatResult:
                LFN_VER_DICT[ PFNPatResult.group('lfn') ] = PFNPatResult.group('ver')

def getCorrectedFilename( fn, forceLFN = None ):
    if forceLFN is None:
        forceLFN = fn.startswith( 'LFN:' )
        if forceLFN:
            fn = fn[4:]
    # Check local
    if not forceLFN:
        if fileutil.exists( fn ):
            return fn
        foundOtherVersion = fileutil.exists_suffix_number( fn + '.' )
        if foundOtherVersion:
            return foundOtherVersion
    # Check PFC
    if not LFN_VER_DICT:
        genLFN_PFN()
    PFNPatResult = _PFNPat.search( fn )
    if not PFNPatResult:
        raise Exception( 'Invalid filename: %s' % fn )
    # Retrieve version from LFN_VER_DICT
    try:
        ver = LFN_VER_DICT[ PFNPatResult.group('lfn') ]
    except KeyError:
        raise Exception( '%s not found.' % fn )
    # If version already provided in filename, check if it matches that in PFC.
    if PFNPatResult.group('ver') and ver != PFNPatResult.group('ver'):
        raise Exception( '%s does not match the version found in the PFC.' % fn )
    if ver is None: # PFC does not contain version info for fn.
        ver = ''
    return 'LFN:' + PFNPatResult.group('url') + PFNPatResult.group('lfn') + ver

def expandParallelVectorNotation( valIn ):
    """A parallel Vector Notation decoder. Adapted from code by Brinick Simmons."""
    if not valIn.count( '[' ):
        return [ valIn ]
    if valIn.count( '[' ) != valIn.count( ']' ):
        raise Exception( 'Mismatched brackets.')
    pieces = re.findall( r'\[[\S]+\]', valIn ) # get the bracket sections
    if not pieces:
        return [ valIn ]
    if False in [ not re.findall( r'\[|\]', p[ 1:-1 ] ) for p in pieces ]:
        raise Exception( 'Nested brackets detected.' )
    ppieces = [ [ i.strip() for i in p[ 1:-1 ].split( ',' ) ] for p in pieces ]
    for i in map( None, *ppieces ):
        try:
            if None in i:
                raise Exception( 'Mismatched number of entries in bracket sets.' )
        except TypeError:
            # the entries in the list are not tuples.
            # This implies a single set of brackets
            # i.e. it is not possible to have mismatched entries
            # in brackets e.g. [(1,2,3),(5,6)]
            break
    for bracket in pieces:
        valIn = valIn.replace( bracket, '%s' )
    return [ valIn % j for j in zip( *ppieces ) ]

def expandStringToList( valIn, ensureExistence = None ):
    """Comma separated list of filenames. Each filename in the list can be coded
    with the following syntax:
    
    [DATASET#]DATASET.ABC.DEF._000[1,2,3,4,5].root
    DATASET[##].ABC.DEF._000[1,2,3,4,5].root
    DATASET[##].ABC.DEF._000[1-5].root ---> Note: [N-M] where N<=M, 0<=N<=9, 0<=M<=9. This is handled by glob()
    DATASET[##].ABC.DEF.*.root         ---> This is handled by glob()

    '#','##' are used to highlight the dataset so as to reflect it in the job report and metadata.xml files."""
    listSep = ','
    if ensureExistence is None:
        ensureExistence = True
    # split the comma separated list (outside brackets)
    sepList = []
    n = len(valIn)
    i = 0
    start = 0
    while i < n:
        c = valIn[i]
        if c == '[':
            i = valIn.find( ']', i + 1 )
            if i == -1:
                raise Exception( 'Mismatched brackets.' )
        elif c == listSep:
            if i == n - 1 or i - start < 1: # listSep found at last position or problem with position counters
                raise Exception( 'Incorrect syntax. Check position of list separators used.' )
            sepList.append( valIn[start:i] )
            start = i + 1
        i += 1
    # add the last entry in the list
    if start < n:
        sepList.append( valIn[start:] )
    all = []
    from PyJobTransformsCore.basic_trfarg import DataFileArg
    for val in sepList:
        forceLFN = val.startswith( 'LFN:' )
        add = []
        ds = ''
        # remove optional LFN: prefix
        if forceLFN:
            val = val[4:]
        # detect dataset
        if '#' in val:
            # retrieve dataset and strip dataset from val.
            ds, val = DataFileArg.extractDataset( val, omitFromName = True )
        if ds:
            ds += '#'
        # Attempt to use glob to allow for wildcard expansion.
        add.extend( glob.glob( val ) )
        if not add: # glob did not succeed. Proceed with PVN.
            add.extend( expandParallelVectorNotation( val ) )
        # stitch filename components together 
        dirName = os.path.dirname( val )
        if ensureExistence:
            if dirName: # filename includes directory path as well.
                add = [ os.sep.join( [ dirName, ds + os.path.basename( getCorrectedFilename( fname, forceLFN ) ) ] ) for fname in add ]
            else:
                add = [ ds + getCorrectedFilename( fname, forceLFN ) for fname in add ]
        elif ds:
            if dirName:
                add = [ os.sep.join( [ dirName, ds + os.path.basename( fname ) ] ) for fname in add ]
            else:
                add = [ ds + fname for fname in add ]
        all += add
    return all

def getCachedFileInfo( filename, infoKey ):
    # try extract information from AthFile cache.
    if isinstance( filename, list ):
        fList = filename
    else:
        fList = [ filename ]
    cmdSnippet = os.linesep.join( [ \
        "from __future__ import print_function",
        "from PyUtils import AthFile",
        "from sys import exit",
        "import pickle",
        "infoVal = None",
        "fileList = %s" % fList,
        "infoKey = '%s'" % infoKey,
        "try:",
        "    athFileObjList = AthFile.fopen( fileList )",
        "except Exception as e:",
        "    exit( 'AthFile access failed: %s' % e )",
        "try:",
        "    infoVal = [ athFileObj.fileinfos[ infoKey ] for athFileObj in athFileObjList ]",
        "except KeyError as e:",
        "    exit( 'Requested infoKey %s missing.' % infoKey )",
        "print ('<infoVal>%s</infoVal>' % pickle.dumps( infoVal ))",
        "exit( 0 )" ] ) + os.linesep
    rc, stdout = timelimited_exec1( 'python -c "%s"' % cmdSnippet, tl_timeout = TRF_SETTING[ 'TRFTimeout' ] * len( fList ) )
    if rc == 0:
        found = re.search( "<infoVal>(.*?)</infoVal>", stdout, re.DOTALL )
        if found:
            resultList = pickle.loads( found.group( 1 ) )
            if isinstance( filename, list ):
                return resultList
            try:
                return resultList[0]
            except Exception:
                pass
    print (stdout)
    return None

def corruptionTestROOT( filename, file_type ):
    if isinstance( filename, list ):
        raise TypeError( 'filename parameter takes a single file and not a list.' )

    from PyJobTransformsCore.ValidateRootFile import checkPFCorruption
    return checkPFCorruption(filename)


def corruptionTestBS( filename, file_type,logger):
    #First try AtlListBSEvents -c %filename:
    cmd = 'AtlListBSEvents -c %s ' % filename
    p = Popen(cmd,shell=True,stdout=PIPE,stderr=PIPE,close_fds=True)
    while p.poll() is None:
      line = p.stdout.readline()
      if line:
        logger.info("AtlListBSEvents Report: %s", line.strip())
    rc = p.returncode
    if rc == 0:
      return rc
    #AltListBSEvents.exe failed, fall back to PyDumper
    else:
      logger.info("AtlListBSEvents failed to validate %s, Using the (slower) PyDumper method ", filename)
      cmdSnippet = os.linesep.join( [
          "from sys import exit",
          "import os",
          "try:",
          "    import PyDumper.SgDumpLib as sdl",
          "    sc, out = sdl.run_sg_dump( files = [ '%s' ], output = os.devnull, pyalg_cls = 'PyDumper.PyComps:DataProxyLoader', use_recex_links = False, file_type = '%s' )" % ( filename, file_type ),
          "except Exception as e:",
          "    exit( 'Validation routine error: %s' % e )",
          "exit( sc )" ] ) + os.linesep
      rc, stdout = timelimited_exec1( 'python -c "%s"' % cmdSnippet )
      if rc != 0:
          print (stdout)
      return rc


class FileType:
    """Utility class for file manipulation that supports various file access systems
    (local files and rfio files).
    It holds the filename, and it can hold optionally the file type, contents and suffix."""
    defaultContents = None
    defaultType = None

    def __init__(self,type=defaultType,contents=defaultContents):
        """Total filename expected: <dir>/<bare>.<contents>.<type>
        type: file type without the dot (root,pool.root,tar,tar.gz,txt,...)
              If None, then any type is allowed, and becomes equal
              to the part after the last . (dot) in the filename.
        contents: file contents (evgen,simu,digi,esd,aod,...)
        bare: anything (except /)

        Optional file attempt numbers are automatically added at the
        end of the type and suffix."""
        #
        # Set the type
        #
        self.__type = type
        if type is None:
            typeRE = '[^.]+'
        else:
            typeRE = type
        # Include the possibility of an attempt number at the end
        self.__typeRE = re.compile( r"\.(?P<type>%s)(\.[0-9]+)?$" % typeRE )
        #
        # Set the contents
        #
        self.__contents = contents
        if contents is None:
            contentsRE = '[^.]+'
        else:
            # make contents case insensitive
            contentsRE = ''
            for c in contents:
                contentsRE += r'[%s%s]' % (c.lower(),c.upper())
        self.__contentsRE = re.compile( r'\.(?P<contents>%s)' % (contentsRE,) +
                                        self.__typeRE.pattern )
        # to get the base name: without type
        self.__baseRE = re.compile( r'(?P<base>.*)' + self.__typeRE.pattern )
        # to get the bare name: without type and contents
        self.__bareRE = re.compile( r'(?P<bare>.*)' + self.__contentsRE.pattern )
        self.__guid = None

    def getGUID(self,filename):
        if TRF_SETTING[ 'testrun' ]:
            return None
        guid = getCachedFileInfo( filename, 'file_guid' )
        if guid is not None:
            return guid
        guid = str(uuid.uuid4()).upper()
        print ("GUID retrieval: %s (%s) generated with uuid.uuid4()" % ( guid, filename ))
        return guid
            
    def hasType(self):
        return self.__type is not None

    def type(self,filename=None):
        if filename is None:
            return self.__type
        else:
            match = self.__typeRE.search( os.path.basename(filename) )
            if match: return match.group('type')
        return ""

    def hasContents(self):
        return self.__contents is not None

    def contents(self,filename=None):
        if filename is None:
            return self.__contents
        else:
            match = self.__contentsRE.search( os.path.basename(filename) )
            if match: return match.group('contents')
        return ""
    
    def validationType( self ):
        """Check if the contents is one recognised by PyDumper.SgDumpLib.run_sg_dump()."""
        vType = self.contents()
        if vType not in ( 'rdo', 'bs', 'esd', 'aod', 'dpd' ):
            return 'any'
        return vType

    def checkType(self,filename):
        """Check if filename ends with .<type>"""
        return not self.hasType() or self.__typeRE.search( filename )

    def checkContents(self,filename):
        """Check if filename ends with <suffix>"""
        return not self.hasContents() or self.__contentsRE.search( filename )

    def baseFilename(self,filename):
        """Return filename without the path and type"""
        match = self.__baseRE.search( os.path.basename(filename) )
        if match:
            return match.group('base')
        else:
            return filename

    def bareFilename(self,filename):
        """Return filename without the path, contents and type"""
        match = self.__bareRE.search( os.path.basename(filename) )
        if match:
            return match.group('bare')
        else:
            return self.baseFilename(filename)

    def getMetaData(self,filename):
        """Return a dictionary with metadata name:value pairs.
        If a value is None, it could not be determined at this level,
        and needs to be determined at a higher level."""
        return { 'fileType' : self.defaultContents }

    def eventCount( self, arg ):
        """Return number of events in file of argument arg.
        Return None if event count is not applicable to file type."""
        return None


class BSFile( FileType ):
    """ByteStream data file"""
    defaultType = 'data'
    defaultContents = 'bs'
    
    def __init__( self, contents = defaultContents, type = defaultType ):
        FileType.__init__( self, type, contents )

    def eventCount( self, arg ):
        """Count the events of InputFile or OutputFile argument <arg>.
        If the number of events can not be determined, return None"""
        try:
            logger = arg.logger()
            fileList = arg.value()
        except Exception as e:
            print ("Event count failed for %s: %s" % ( arg, e ))
            return None
        if TRF_SETTING[ 'testrun' ] and ( VALIDATION_DICT[ 'ALL' ] is False or VALIDATION_DICT[ 'testCountEvents' ] is False ):
            logger.info( 'Test run in progress. Event count (AthFile-based) disabled.' )
            return None
        resultList = getCachedFileInfo( fileList, 'nentries' )
        if resultList is None or not isinstance( resultList, list ):
            return resultList
        # resultList is indeed a list
        try:
            return sum( resultList )
        except TypeError:
            # probably 'N/A' detected in resultList. Should be fixed with AthenaPython-00-04-19
            return None

    def validateFile( self, arg, **validationDict ):
        """Validate the file. Use SgDumpLib.run_sg_dump() or event count routine."""
#        ### TEST ONLY ###
#        raise TransformValidationError( arg.value(), 'Test validation error (%s)' % arg.name(), 'TRF_OUTFILE_NOTFOUND' )
        try:
            logger = arg.logger()
            fName = arg.value()
            argName = arg.name()
        except Exception as e:
            print ("Could not validate file associated with %s: %s" % ( arg, e ))
            return
        if VALIDATION_DICT[ 'ALL' ] is False:
            logger.info( "Skipping all validation routines." )
            return
        # Defined default validation values
        vDict = { 'testIfEmpty' : True, 'testIfNoEvents' : True, 'testIfExists' : True, 'testIfCorrupt' : True, 'testCountEvents' : True, 'extraValidation' : None, 'testMatchEvents' : True, 'testEventMinMax' : True, 'stopOnEventCountNone' : True, 'continueOnZeroEventCount' : True}
        # apply modifications to validation values from subclasses
        vDict.update( validationDict )
        # Check if any validation tests have been disabled at the command line and apply to vDict
        for vTestName, vTestEnabled in VALIDATION_DICT.items():
            if vTestEnabled is not None:
                vDict[ vTestName ] = vTestEnabled
        #Make sure filename is not a list
        if isinstance(fName,list):
          fName = fName[0] 
        if not fileutil.exists( fName ):
            if vDict[ 'testIfExists' ]:
                raise TransformValidationError( fName, 'failed validation. File not created. Argument %s' % argName, 'TRF_OUTFILE_NOTFOUND' )
            else:
                logger.info( "Ignoring missing %s.", fName )
                return
        if fileutil.getsize( fName ) == 0:
            if vDict[ 'testIfEmpty' ]:
                raise TransformValidationError( fName, 'failed validation. Empty file. Argument %s' % argName, 'TRF_OUTFILE_EMPTY' )
            else:
                logger.info( "Ignoring empty %s.", fName )
                return
        # Check if sdl can cope with the file type
        if self.validationType() == 'any':
            vDict[ 'testIfCorrupt' ] = False
        if vDict[ 'testIfCorrupt' ] and sdl is not None:
            logger.info( "Checking %s for corruption.", fName )
            vTimer.start( '%s validation' % argName )
            sc = corruptionTestBS( fName, self.validationType(),logger )
            vTimer.stop( '%s validation' % argName )
            if sc < 0:
                logger.warning( "Execution of corruption test failed [%s].", sc )
            elif sc > 0:
                raise TransformValidationError( fName, 'failed validation [%s]. File corrupt. Argument %s' % ( sc, argName ) )
        if vDict[ 'testCountEvents' ]:
            logger.info( "Attempting to validate %s using event count routine.", fName )
            vTimer.start( '%s validation' % argName )
            eCount = arg.eventCount()
            vTimer.stop( '%s validation' % argName )
            if eCount == 0:
                if vDict[ 'testIfNoEvents' ]:
                  if not vDict[ 'continueOnZeroEventCount' ]:  
                    raise TransformValidationError( fName, 'failed validation. File contains no events. Argument %s' % argName, 'TRF_OUTFILE_TOOFEW' )
                  else:
                    logger.info(" WARNING - 0 events in %s, proceeding with empty file. ", fName)
                else:
                    logger.info( "Ignoring 0 events in %s.", fName )
                    return
            elif eCount is None:
                if vDict[ 'stopOnEventCountNone' ]:
                  raise TransformValidationError( fName, 'failed validation. Events could not be counted Argument %s' % argName, 'TRF_OUTFILE_NEVENTFAIL' )
                else:
                  logger.info( "No event count for file %s (corrupt or unreachable). Proceeding anyways.", fName )
        if callable( vDict[ 'extraValidation' ] ):
            vTimer.start()
            extraValidationResult = None
            try:
                extraValidationResult = timelimited_exec( tl_func = vDict[ 'extraValidation' ], tl_func_args = ( fName, ), tl_pre_func = lambda:None, tl_post_func = lambda:None, tl_timeout = TRF_SETTING[ 'validationTimeout' ], tl_retry = TRF_SETTING[ 'validationRetry' ], tl_interval = TRF_SETTING[ 'validationSleepInterval' ] )
            except TransformThreadTimeout:
                logger.warning( 'Extra validation routine timed out.' )
            except TransformThreadError as e:
                logger.warning( 'Thread running extra validation routine failed to stop.\n%s', e )
            except Exception as e:
                logger.warning( 'Extra validation routine error.\n%s', e )
            vTimer.stop()
            if not extraValidationResult:
                raise TransformValidationError( fName, 'failed additional validation. Argument %s' % argName, 'TRF_OUTFILE' )
#        # Fail if sdl check fails, does not fail over to event count routine.
#        if sdl is None:
#            eCount = arg.eventCount()
#            if eCount == 0:
#                raise TransformValidationError( fName, 'failed validation. File contains no events. Argument %s' % argName, 'TRF_OUTFILE_TOOFEW' )
#            elif eCount == None:
#                raise TransformValidationError( fName, 'failed validation. Events could not be counted Argument %s' % argName, 'TRF_OUTFILE_NOTREAD' )
#        else:
#            sc, out = sdl.run_sg_dump( files = [ fName ], output = os.devnull, pyalg_cls = 'PyDumper.PyComps:DataProxyLoader', use_recex_links = False, file_type = self.validationType(), msg = logger )
#            if sc != 0:
#                raise TransformValidationError( fName, 'failed validation [%s]. File corrupt. Argument %s' % ( sc, argName ) )
        logger.info( "%s successfully validated.", fName )
        return

    def getGUID(self,filename):
        if TRF_SETTING[ 'testrun' ]:
            return None
        guid = getCachedFileInfo( filename, 'file_guid' )
        if guid is None:
            raise FileError( filename, "File %s GUID not present in BS file.", filename )
        return guid

    def getMetaData(self,filename):
        return { 'fileType' : self.defaultContents }


class RootTTreeFile( FileType ):
    """A root file containing TTree's"""
    defaultType = 'root'
    defaultContents = None
    # Borrowed from PoolDataFile
    defaultCatalogFilename = 'PoolFileCatalog.xml'
    defaultCatalog = 'xmlcatalog_file:' + defaultCatalogFilename
    
    def __init__(self,contents=defaultContents,type=defaultType):
        FileType.__init__(self,type,contents)
        self.__rootMacroFile = os.path.join(trfenv.trfPackagePath,'share','CountEvents.cc')

    def eventCount( self, arg ):
        """Count events in Root TTree file using root"""
        return None

    def getGUID( self, filename ):
        if TRF_SETTING[ 'testrun' ]:
            return None
        if not fileutil.exists( filename ):
            print ("GUID retrieval failed: %s not found." % filename)
            return None
        # Use FClistGUID
        rc, guid = getGUIDfromPFC(filename)
        if guid is not None:
            return guid
        if rc != 0:
            print ('Warning: Problem with PFC')
        
        # ROOT type files have no internal GUID, so if it wasn't in the PFC then
        # we just make one up...
        guid = str(uuid.uuid4())
        print ('Generated GUID %s for %s' % (guid, filename))
        # Do not register GUIDs for now - FC behaving badly!
        rc = addGUIDtoPFC(filename, guid, type = 'ROOT_All')
        if rc != 0:
            print ('Warning: Failed to add new GUID to PFC')

        return guid


class PoolDataFile( RootTTreeFile ):
    defaultType = 'pool.root'
    defaultCatalogFilename = 'PoolFileCatalog.xml'
    defaultCatalog = 'xmlcatalog_file:' + defaultCatalogFilename
    defaultMessageLevel = 5
    defaultContents = None

    def __init__(self,contents=defaultContents,type=defaultType):
        RootTTreeFile.__init__(self,contents,type)
        
    def getGUID(self, filename):
        # Use FClistGUID
        rc, guid = getGUIDfromPFC(filename)
        if guid is not None:
            return guid
        if rc != 0:
            print ('Warning: Problem with PFC')

        # Use pool_extractFileIdentifier.py
        poolcmd = ['pool_extractFileIdentifier.py', filename]
        print ("GUID retrieval: Attempting to use %s." % poolcmd)
        if not find_executable(poolcmd[0]):
            raise EnvironmentError('Pool utility %s not found in PATH' % poolcmd[0] )
        p = Popen(poolcmd,shell=False,stdout=PIPE,stderr=PIPE,close_fds=True)
        (stdout, stderr) = p.communicate()
        for line in stdout:
            words = line.split(os.linesep)
            try:
                if filename in words[1]:
                    guid = words[0]
                    break
            except Exception:
                continue
        if p.returncode != 0:
            print ("GUID retrieval failed: %s" % stderr)
            guid = None
        if guid:
            print ("GUID retrieval: Using embedded value %s (%s)" % (guid,filename))
            return guid


    def validateFile( self, arg, **validationDict ):
        """Validate the file. Use SgDumpLib.run_sg_dump() or event count routine."""
#        ### TEST ONLY ###
#        raise TransformValidationError( arg.value(), 'Test validation error (%s)' % arg.name(), 'TRF_OUTFILE_NOTFOUND' )
        try:
            logger = arg.logger()
            fName = arg.value()
            argName = arg.name()
        except Exception as e:
            print ("Could not validate file associated with %s: %s" % ( arg, e ))
            return
        if VALIDATION_DICT[ 'ALL' ] is False:
            logger.info( "Skipping all validation routines." )
            return
        # Defined default validation values
        vDict = { 'testIfEmpty' : True, 'testIfNoEvents' : True, 'testIfExists' : True, 'testIfCorrupt' : True, 'testCountEvents' : True, 'extraValidation' : None, 'testMatchEvents' : True, 'testEventMinMax' : True , 'stopOnEventCountNone' : True, 'continueOnZeroEventCount' : True}
        # apply modifications to validation values from subclasses
        vDict.update( validationDict )
        # Check if any validation tests have been disabled at the command line and apply to vDict
        for vTestName, vTestEnabled in VALIDATION_DICT.items():
            if vTestEnabled is not None:
                vDict[ vTestName ] = vTestEnabled
        if not fileutil.exists( fName ):
            if vDict[ 'testIfExists' ]:
                raise TransformValidationError( fName, 'failed validation. File not created. Argument %s' % argName, 'TRF_OUTFILE_NOTFOUND' )
            else:
                logger.info( "Ignoring missing %s.", fName )
                return
        if fileutil.getsize( fName ) == 0:
            if vDict[ 'testIfEmpty' ]:
                raise TransformValidationError( fName, 'failed validation. Empty file. Argument %s' % argName, 'TRF_OUTFILE_EMPTY' )
            else:
                logger.info( "Ignoring empty %s.", fName )
                return
        # Check if sdl can cope with the file type
        if self.validationType() == 'any':
            vDict[ 'testIfCorrupt' ] = False
        if vDict[ 'testIfCorrupt' ]:
            logger.info( "Checking %s for corruption.", fName )
            vTimer.start( '%s validation' % argName )
            sc = corruptionTestROOT( fName, self.validationType() )
            vTimer.stop( '%s validation' % argName )
            if sc<0:
                raise TransformValidationError( fName, 'failed validation [%s]. File corrupt. Argument %s' % ( sc, argName ) )
        if vDict[ 'testCountEvents' ]:
            logger.info( "Attempting to validate %s using event count routine.", fName )
            vTimer.start( '%s validation' % argName )
            eCount = arg.eventCount()
            vTimer.stop( '%s validation' % argName )
            if eCount == 0:
                if vDict[ 'testIfNoEvents' ]:
                  if not vDict[ 'continueOnZeroEventCount' ]:  
                    raise TransformValidationError( fName, 'failed validation. File contains no events. Argument %s' % argName, 'TRF_OUTFILE_TOOFEW' )
                  else:
                    logger.info(" WARNING - 0 events in %s, proceeding with empty file. ", fName)
                else:
                    logger.info( "Ignoring 0 events in %s.", fName )
                    return
            elif eCount is None:
                if vDict[ 'stopOnEventCountNone' ]:
                  raise TransformValidationError( fName, 'failed validation. Events could not be counted Argument %s' % argName, 'TRF_OUTFILE_NEVENTFAIL' )
                else:
                  logger.info( "No event count for file %s (corrupt or unreachable). Proceeding anyways.", fName )
        if callable( vDict[ 'extraValidation' ] ):
            vTimer.start()
            extraValidationResult = None
            try:
                extraValidationResult = timelimited_exec( tl_func = vDict[ 'extraValidation' ], tl_func_args = ( fName, ), tl_pre_func = lambda:None, tl_post_func = lambda:None, tl_timeout = TRF_SETTING[ 'validationTimeout' ], tl_retry = TRF_SETTING[ 'validationRetry' ], tl_interval = TRF_SETTING[ 'validationSleepInterval' ] )
            except TransformThreadTimeout:
                logger.warning( 'Extra validation routine timed out.' )
            except TransformThreadError as e:
                logger.warning( 'Thread running extra validation routine failed to stop.\n%s', e )
            except Exception as e:
                logger.warning( 'Extra validation routine error.\n%s', e )
            vTimer.stop()
            if not extraValidationResult:
                raise TransformValidationError( fName, 'failed additional validation. Argument %s' % argName, 'TRF_OUTFILE' )
#        # Fail if sdl check fails, does not fail over to event count routine.
#        if sdl is None:
#            eCount = arg.eventCount()
#            if eCount == 0:
#                raise TransformValidationError( fName, 'failed validation. File contains no events. Argument %s' % argName, 'TRF_OUTFILE_TOOFEW' )
#            elif eCount == None:
#                raise TransformValidationError( fName, 'failed validation. Events could not be counted Argument %s' % argName, 'TRF_OUTFILE_NOTREAD' )
#        else:
#            sc, out = sdl.run_sg_dump( files = [ fName ], output = os.devnull, pyalg_cls = 'PyDumper.PyComps:DataProxyLoader', use_recex_links = False, msg = logger )
#            if sc != 0:
#                raise TransformValidationError( fName, 'failed validation [%s]. File corrupt. Argument %s' % ( sc, argName ) )

        logger.info( "%s successfully validated.", fName )
        return
    
    def writeSize(self, arg):
        fName = arg.value()
        print ('trfutil.py  Checking object sizes in file ', fName )
        if  isinstance(self, AODFile) or isinstance(self,ESDFile) or isinstance(self,DPDFile) or isinstance(self,EvgenFile) or isinstance(self,HitsFile):
            ne=0
            collectionSize={}
            try:
                # print ('trfutil.py checkFile running...')
                import PyUtils.PoolFile as PF
                # PF.PoolOpts.FAST_MODE = True
                poolFile = PF.PoolFile( fName , False)
                poolFile.checkFile( )
                ne=poolFile.dataHeader.nEntries
                if ne>0:
                    for collection in poolFile.data:
                        collectionSize[collection.name] = collection.diskSize / ne
            except Exception as e:
                print ("## Caught exception [%s] !!" % str(e.__class__))
                print ("## What:",e)
                print (sys.exc_info()[0])
                print (sys.exc_info()[1])
                return
                
            # fName+='.size'
            # try:
            #     with open( fName, 'w' ) as sizeFile:
            #         pickle.dump(ne,sizeFile)
            #         pickle.dump(collectionSize,sizeFile)
            # except Exception as e:
            #     print ('trfutil.py WARNING: Could not write consumption info to file ', fName , e)
            #     print (sys.exc_info()[0])
            #     print (sys.exc_info()[1])
            #     return

            #returns number_of_events and tuple of sizes
            return [ne, collectionSize]	
        else:
            print ('not needed for file of this type')
                
        
    def eventCount( self, arg ):
        """Count the events of InputFile or OutputFile argument <arg>.
        If the number of events can not be determined, return None"""
        try:
            logger = arg.logger()
            fileList = arg.value()
        except Exception as e:
            print ("Event count failed for %s: %s" % ( arg, e ))
            return None
        if TRF_SETTING[ 'testrun' ] and ( VALIDATION_DICT[ 'ALL' ] is False or VALIDATION_DICT[ 'testCountEvents' ] is False ):
            logger.info( 'Test run in progress. Event count (AthFile-based) disabled.' )
            return None
        resultList = getCachedFileInfo( fileList, 'nentries' )
        if resultList is None or not isinstance( resultList, list ):
            return resultList
        # resultList is indeed a list
        try:
            return sum( resultList )
        except TypeError:
            # probably 'N/A' detected in resultList. Should be fixed with AthenaPython-00-04-19
            return None


    @staticmethod
    def setMessageLevel(level,logger=None):
        newLevel = str(level)
        oldLevel = os.environ.get('POOL_OUTMSG_LEVEL')
        if oldLevel != newLevel:
            os.environ['POOL_OUTMSG_LEVEL'] = newLevel
            if logger:
                logger.info( "Setting POOL message level to %d.", level )


class EvgenFile( PoolDataFile ):
    defaultContents = 'evgen'
    def __init__(self,contents=defaultContents):
        PoolDataFile.__init__(self,contents)


class HitsFile( PoolDataFile ):
    defaultContents = 'hits'
    def __init__(self,contents=defaultContents):
        PoolDataFile.__init__(self,contents)


class RDOFile( PoolDataFile ):
    defaultContents = 'rdo'
    def __init__(self,contents=defaultContents):
        PoolDataFile.__init__(self,contents)


class ESDFile( PoolDataFile ):
    defaultContents = 'esd'
    def __init__(self,contents=defaultContents):
        PoolDataFile.__init__(self,contents)
   

class AODFile( PoolDataFile ):
    defaultContents = 'aod'
    def __init__(self,contents=defaultContents):
        PoolDataFile.__init__(self,contents)


class TAGFile( RootTTreeFile ):
    defaultContents = 'tag'
    def __init__(self,contents=defaultContents):
        RootTTreeFile.__init__(self,contents)
        
    def getGUID(self, filename):
        # Use FClistGUID, then fallback to AthFile (N.B. tag files have funny names in the PFC!)
        rc, guid = getGUIDfromPFC("RootCollection||PFN:" + filename)
        if guid is not None:
            return guid
        if rc != 0:
            print ('Warning: Problem with PFC')
            
        print ("GUID retrieval: Now attempting to use AthFile to retrieve GUID from %s." % filename)
        guid = getCachedFileInfo( filename, 'file_guid' )
        if guid is None:
            print ("ERROR: GUID retrieval failed.")
        return guid


    def eventCount( self, arg ):
        """Return number of events in file of argument arg.
        Return None if event count is not applicable to file type."""
        try:
            fName = arg.value()
        except Exception as e:
            print ("Event count failed for %s: %s" % ( arg, e ))
            return None
        return ntup_entries( fname = fName, tree_names = "POOLCollectionTree" )


class SANFile( RootTTreeFile ):
    defaultContents = 'san'
    def __init__(self,contents=defaultContents):
        RootTTreeFile.__init__(self,contents)


class HistogramFile( FileType ):
    defaultContents = 'hist'
    defaultType = 'root'
    def __init__(self,contents=defaultContents,type=defaultType):
        FileType.__init__(self,type,contents)


class FTKIPFile(FileType):
    defaultContents = 'ftkip'
    def __init__(self, contents=defaultContents):
        FileType.__init__(self, contents)
        
    def eventCount(self, arg):
        '''Count ftk i/p events using the ^F event flag'''        
        fname = arg.value()
        try:
            eventCount = 0
            f = bz2.BZ2File(fname, 'r')
            for line in f:
                if line.startswith('F'):
                    eventCount += 1
            return eventCount
        except OSError as e:
            print ("Got OSError: %s" % str(e))
            return None
        except IOError as e:
            print ("Got IOError: %s" % str(e))
            return None


class JiveXMLTGZFile(FileType):
    defaultContents = 'XML.tar.gz'
    def __init__(self, contents=defaultContents):
        FileType.__init__(self, contents)
        
    def eventCount(self, arg):
        '''Count events by seeing how many JiveXML files we have in the tarball'''
        fname = arg.value()
        try:
            f = tarfile.open(fname, 'r:*')
            n = f.getnames()
            f.close()
            return len(n)
        except tarfile.TarError as e:
            print ('Error determining the number of events in %s: %s' % (fname, e))
            return None

## Helper function to make JiveXML tarball from all JiveXML_*.xml in the cwd
def jiveXMLtgz(fname):
    # This should really be somewhere else, not in the actual trf code. Demo only!
    jiveXMLfiles = glob.glob('JiveXML_*.xml')
    if len(jiveXMLfiles) == 0:
        raise RuntimeError ('No JiveXML files to merge')
    print ("Will tar.gz %d JiveXML files: %s" % (len(jiveXMLfiles), jiveXMLfiles))
    try:
        jXMLtargz = tarfile.open(fname, 'w:gz')
        for jXMLfile in jiveXMLfiles:
            jXMLtargz.add(jXMLfile)
        jXMLtargz.close()
    except tarfile.TarError as e:
        raise RuntimeError ('Error while trying to create Jive XML tag.gz file %s: %s' % (fname, e))


class NtupleFile( RootTTreeFile ):
    defaultContents = 'ntup'
    defaultType = 'root'
    def __init__(self,contents=defaultContents,type=defaultType, tree_names = None):
        RootTTreeFile.__init__(self,type,contents)
        if tree_names is None:
            tree_names = []
        self.tree_names = tree_names
    
    def eventCount( self, arg ):
        try:
            fName = arg.value()
        except Exception as e:
            print ("Event count failed for %s: %s" % ( arg, e ))
            return None
        if not isinstance(fName, list):
            fName=[fName]
        nentries=0
        for file in fName:
            n=ntup_entries(fname=file, tree_names=self.tree_names)
            try:
                nentries += n
            except TypeError:
                return None
        return nentries

    def validateFile( self, arg, **validationDict ):
        try:
            logger = arg.logger()
            fName = arg.value()
            argName = arg.name()
        except Exception as e:
            print ("Could not validate file associated with %s: %s" % ( arg, e ))
            return
        if VALIDATION_DICT[ 'ALL' ] is False:
            logger.info( "Skipping all validation routines." )
            return
        # Defined default validation values
        vDict = { 'testIfEmpty' : True, 'testIfNoEvents' : True, 'testIfExists' : True, 'testIfCorrupt' : True, 'testCountEvents' : True, 'extraValidation' : None, 'testMatchEvents' : True, 'testEventMinMax' : True , 'stopOnEventCountNone' : True, 'continueOnZeroEventCount' : True}
        # apply modifications to validation values from subclasses
        vDict.update( validationDict )
        # Check if any validation tests have been disabled at the command line and apply to vDict
        for vTestName, vTestEnabled in VALIDATION_DICT.items():
            if vTestEnabled is not None:
                vDict[ vTestName ] = vTestEnabled
        if not fileutil.exists( fName ):
            if vDict[ 'testIfExists' ]:
                raise TransformValidationError( fName, 'failed validation. File not created. Argument %s' % argName, 'TRF_OUTFILE_NOTFOUND' )
            else:
                logger.info( "Ignoring missing %s.", fName )
                return
        if fileutil.getsize( fName ) == 0:
            if vDict[ 'testIfEmpty' ]:
                raise TransformValidationError( fName, 'failed validation. Empty file. Argument %s' % argName, 'TRF_OUTFILE_EMPTY' )
            else:
                logger.info( "Ignoring empty %s.", fName )
                return

        if vDict[ 'testIfCorrupt' ]:
            logger.info( "Checking %s for corruption.", fName )
            vTimer.start( '%s validation' % argName )
            from PyJobTransformsCore.trfValidateRootFile import checkFile as checkNTUPFile
            sc = checkNTUPFile(fileName=fName, type='basketWise', requireTree=False, msg=logger)
            vTimer.stop( '%s validation' % argName )
            if sc!=0:
                raise TransformValidationError( fName, 'failed validation [%s]. File corrupt. Argument %s' % ( sc, argName ) )
    
            
        if vDict[ 'testCountEvents' ] and self.tree_names:
            logger.info( "Attempting to validate %s using event count routine.", fName )
            vTimer.start( '%s validation' % argName )
            eCount = arg.eventCount()
            vTimer.stop( '%s validation' % argName )
            if eCount == 0:
                if vDict[ 'testIfNoEvents' ]:
                  if not vDict[ 'continueOnZeroEventCount' ]:  
                    raise TransformValidationError( fName, 'failed validation. File contains no events. Argument %s' % argName, 'TRF_OUTFILE_TOOFEW' )
                  else:
                    logger.info(" WARNING - 0 events in %s, proceeding with empty file. ", fName)
                else:
                    logger.info( "Ignoring 0 events in %s.", fName )
                    return
            elif eCount is None:
                if vDict[ 'stopOnEventCountNone' ]:
                  raise TransformValidationError( fName, 'failed validation. Events could not be counted Argument %s' % argName, 'TRF_OUTFILE_NEVENTFAIL' )
                else:
                  logger.info( "No event count for file %s (corrupt or unreachable). Proceeding anyways.", fName )
        else:
            logger.info( "Event counting not tested for %s.", fName )
        logger.info( "%s successfully validated.", fName )
        return



class MonitorHistFile( RootTTreeFile ):
    defaultContents = 'monitor_hist'
    defaultType = 'root'
    def __init__(self,contents=defaultContents,type=defaultType):
        RootTTreeFile.__init__(self,type,contents)

    def eventCount( self, arg ):
        try:
            logger = arg.logger()
            fName = arg.value()
        except Exception as e:
            print ("Event count failed for %s: %s" % ( arg, e ))
            return None
        if TRF_SETTING[ 'testrun' ] and ( VALIDATION_DICT[ 'ALL' ] is False or VALIDATION_DICT[ 'testCountEvents' ] is False ):
            logger.info( 'Test run in progress. Event count (ROOT-based) disabled.' )
            return None
        ROOT = RootUtils.import_root(batch=True)
        f = None
        try:
            f = timelimited_exec( tl_func = ROOT.TFile.Open, tl_func_args = ( fName, ), tl_pre_func = lambda:None, tl_post_func = lambda:None, tl_timeout = TRF_SETTING[ 'TRFTimeout' ], tl_retry = TRF_SETTING[ 'TRFRetry' ], tl_interval = TRF_SETTING[ 'TRFSleepInterval' ] )
        except TransformThreadTimeout:
            logger.warning( 'ROOT file opening timed out.' )
        except TransformThreadError as e:
            logger.warning( 'Thread for ROOT file opening failed to stop.\n%s', e )
        except Exception as e:
            logger.warning( 'ROOT file open error.\n%s', e )
        if not f:
            logger.warning("Could not open file [%s].", fName)
            return None
        # assume the first key starting with run_ gives the run number
        rundir = None
        keys = f.GetListOfKeys()
        for key in keys:
            if key.GetName()[:4] == 'run_' and key.GetName() != 'run_multiple':
                rundir = key.GetName()
        if rundir is None:
            logger.warning( 'Unable to find run directory.' )
            f.Close()
            return None
        logger.debug( 'Using run directory %s.', rundir )
        hpath = '%s/GLOBAL/DQTDataFlow/events_lb' % rundir
        h = f.Get(hpath)
        if not isinstance( h, ROOT.TH1 ):
            logger.warning( 'Unable to retrieve %s.', hpath )
            f.Close()
            return None
        try:
            nBinsX = h.GetNbinsX()
        except Exception:
            f.Close()
            logger.warning( 'Unable to retrieve number of events.' )
            return None            
        nev = 0
        for i in range(1, nBinsX):
            if h[i] < 0:
                # should not happen
                logger.warning( 'Negative number of events for step %s.', h.GetXaxis().GetBinLabel(i) )
                f.Close()
                return None
            if h[i] == 0:
                continue
            # h is not zero
            if nev == 0:
                nev = h[i]
            else:
                if nev != h[i]:
                    # mismatch in number of events between different steps!
                    logger.warning( 'Mismatch in events per step; most recent step seen is %s.', h.GetXaxis().GetBinLabel(i) )
                    f.Close()
                    return None
        f.Close()
        return nev

    def validateFile( self, arg, **validationDict ):
        """Validate the file. """
        try:
            logger = arg.logger()
            fName = arg.value()
            argName = arg.name()
        except Exception as e:
            print ("Could not validate file associated with %s: %s" % ( arg, e ))
            return
        if VALIDATION_DICT[ 'ALL' ] is False:
            logger.info( "Skipping all validation routines." )
            return
        # Defined default validation values
        vDict = { 'testIfEmpty' : True, 'testIfNoEvents' : True, 'testIfExists' : True, 'testIfCorrupt' : True, 'testCountEvents' : True, 'extraValidation' : None, 'testMatchEvents' : True, 'testEventMinMax' : True , 'stopOnEventCountNone' : True, 'continueOnZeroEventCount' : True}
        # apply modifications to validation values from subclasses
        vDict.update( validationDict )
        # Check if any validation tests have been disabled at the command line and apply to vDict
        for vTestName, vTestEnabled in VALIDATION_DICT.items():
            if vTestEnabled is not None:
                vDict[ vTestName ] = vTestEnabled
        if not fileutil.exists( fName ):
            if vDict[ 'testIfExists' ]:
                raise TransformValidationError( fName, 'failed validation. File not created. Argument %s' % argName, 'TRF_OUTFILE_NOTFOUND' )
            else:
                logger.info( "Ignoring missing %s.", fName )
                return
        if fileutil.getsize( fName ) == 0:
            if vDict[ 'testIfEmpty' ]:
                raise TransformValidationError( fName, 'failed validation. Empty file. Argument %s' % argName, 'TRF_OUTFILE_EMPTY' )
            else:
                logger.info( "Ignoring empty %s.", fName )
                return
#         if vDict[ 'testIfCorrupt' ]:
#             logger.info( "Checking %s for corruption.", fName )
#             vTimer.start( '%s validation' % argName )
#             sc = corruptionTestROOT( fName, self.validationType() )
#             vTimer.stop( '%s validation' % argName )
#             if sc<0:
#                 raise TransformValidationError( fName, 'failed validation [%s]. File corrupt. Argument %s' % ( sc, argName ) )
        if vDict[ 'testCountEvents' ] and 'HIST_' not in fName:
            logger.info( "Attempting to validate %s using event count routine.", fName )
            vTimer.start( '%s validation' % argName )
            eCount = arg.eventCount()
            vTimer.stop( '%s validation' % argName )
            if eCount == 0:
                if vDict[ 'testIfNoEvents' ]:
                  if not vDict[ 'continueOnZeroEventCount' ]:  
                    raise TransformValidationError( fName, 'failed validation. File contains no events. Argument %s' % argName, 'TRF_OUTFILE_TOOFEW' )
                  else:
                    logger.info(" WARNING - 0 events in %s, proceeding with empty file. ", fName)
                else:
                    logger.info( "Ignoring 0 events in %s.", fName )
                    return
            elif eCount is None:
                if vDict[ 'stopOnEventCountNone' ]:
                  raise TransformValidationError( fName, 'failed validation. Events could not be counted Argument %s' % argName, 'TRF_OUTFILE_NEVENTFAIL' )
                else:
                  logger.info( "No event count for file %s (corrupt or unreachable). Proceeding anyways.", fName )
        elif 'HIST_' in fName:
            logger.info('No event counting validation performed because file %s is of HIST_ subtype', fName)
            
        if callable( vDict[ 'extraValidation' ] ):
            vTimer.start()
            extraValidationResult = None
            try:
                extraValidationResult = timelimited_exec( tl_func = vDict[ 'extraValidation' ], tl_func_args = ( fName, ), tl_pre_func = lambda:None, tl_post_func = lambda:None, tl_timeout = TRF_SETTING[ 'validationTimeout' ], tl_retry = TRF_SETTING[ 'validationRetry' ], tl_interval = TRF_SETTING[ 'validationSleepInterval' ] )
            except TransformThreadTimeout:
                logger.warning( 'Extra validation routine timed out.' )
            except TransformThreadError as e:
                logger.warning( 'Thread running extra validation routine failed to stop.\n%s', e )
            except Exception as e:
                logger.warning( 'Extra validation routine error.\n%s', e )
            vTimer.stop()
            if not extraValidationResult:
                raise TransformValidationError( fName, 'failed additional validation. Argument %s' % argName, 'TRF_OUTFILE' )
        logger.info( "%s successfully validated.", fName )
        return



class DPDFile( PoolDataFile ):
    defaultContents = 'dpd'
    def __init__(self,contents=defaultContents):
        PoolDataFile.__init__(self,contents)

# Use default PoolDataFile validation.
#    def validateFile( self, arg, **validationDict ):
#        #'testIfCorrupt' flag present until there is concensus on the treatment of empty DPD files
#        vDict = { 'testIfNoEvents' : False, 'testIfCorrupt' : False }
#        vDict.update( validationDict )
#        PoolDataFile.validateFile( self, arg, **vDict )


class CommentLine:
    """Utility class to generate python comment lines from a help string"""
    def __init__(self,comment):
        self.__comment = comment

    @staticmethod
    def getLine( char, width = 80 ): return '#' + width * char

    @staticmethod
    def dashLine( width = 80 ): return '#' + width * '-'

    @staticmethod
    def hashLine( width = 80 ): return '#' + width * '#'

    def __str__(self):
        return self.smallComment()

    def smallComment(self):
        return '# ' + self.__comment.replace(os.linesep, os.linesep + '# ')

    def bigComment(self,char='-',width=80):
        line = CommentLine.getLine(char,width)
        return line + os.linesep + \
            self.smallComment() + os.linesep + \
            line


#
# end of class CommentLine
#
class Author:
    def __init__(self,name,email):
        self.__name = name
        self.__email = email

    def __str__(self):
        return '%s <%s>' % (self.__name, self.__email)
    
    def name(self):
        return self.__name

    def email(self):
        return self.__email


class StringNumberList:
    """List of strings only differing by a number they contain, where the list of numbers is coded in a special syntax.
    <prefix>[<list>]<postfix> where '[' and ']' are litteral characters and <list> gives a recipe for a list of
    numbers to be generated (with <n> and <m> integers): 
    <n>,<m> (enumeration) or <n>-<m> (range including m) or any combination of those.
    A python list of strings is generated where the [<list>] is replaced by the actual integers that the list represents.
    The [<list>] part can also be omitted, in which case a list of one string is generated (as given).
    The width of the integers is determined by the integer with the most number of digits in the [<list>], where leading 0's
    are included in the count. The integers in the expanded filenames have leading 0's padded where needed."""

    openBracket = '['
    closeBracket = ']'

    def __init__(self, codedList=None):
        self.set( codedList )

    def set(self, codedString):
        self.__codedString = codedString
        self.__numbers = None
        self.__prefix = None
        self.__suffix = None
        self.__numberList = None
        self.__digits = 0
        # return self for easy chaining of commands
        return self

    def getPrefix(self,openBracket=-1):
        """Get everything up to (but not including) the [.
        If [ is not found, return the full string. In case of error, return None.
        The <openBracket> argument (optional) is there for optimisation and gives
        the position of the '[' in the coded string (or -1 if unknown)."""
        if self.__prefix is not None: return self.__prefix
        valIn = self.__codedString
        if valIn is None: return None #signalling error
        if openBracket != -1:
            assert valIn[openBracket] == StringNumberList.openBracket
            bopen = openBracket
        else:
            bopen = valIn.find(StringNumberList.openBracket)
            if bopen == -1: self.__prefix = valIn
        if bopen > 0:
            self.__prefix = valIn[:bopen]
        else:
            self.__prefix = valIn
        return self.__prefix

    def getSuffix(self,closeBracket=-1):
        """Get everything after the ].
        If ] is not found, return emptry string. In case of error, return None"""
        if self.__suffix is not None: return self.__suffix
        valIn = self.__codedString
        if valIn is None: return None #signalling error
        if closeBracket != -1:
            assert valIn[closeBracket] == StringNumberList.closeBracket
            bclose = closeBracket
        else:
            bclose = valIn.find(StringNumberList.closeBracket)
            if bclose == -1: self.__suffix = ""
        if bclose < len(valIn) - 1:
            self.__suffix = valIn[bclose+1:]
        else:
            self.__suffix = ""
        return self.__suffix

    def getNumbers(self,openBracket=-1,closeBracket=-1):
        """Get the part in between [ and ], including the [].
        If [] part is not found, return empty string. In case of error, return None"""
        if self.__numbers is not None: return self.__numbers
        valIn = self.__codedString
        if valIn is None: return None #signalling error
        if openBracket != -1:
            assert valIn[openBracket] == StringNumberList.openBracket
            bopen = openBracket
        else:
            bopen = valIn.find(StringNumberList.openBracket)
        if closeBracket != -1:
            assert valIn[closeBracket] == StringNumberList.closeBracket
            bclose = closeBracket
        else:
            bclose = valIn.find(StringNumberList.closeBracket,bopen + 1)
        if bopen == -1 and bclose == -1:
            self.__numbers = ""
        elif bopen == -1 or bclose == -1 or bclose < bopen + 2:
            self.__numbers = None
        else:
            self.__numbers = valIn[bopen:bclose+1]
        return self.__numbers

    def getNumberList(self,openBracket=-1,closeBracket=-1):
        """Return a tuple of size 2, containing the list of integers in the first field and
        the number of digits in the second field. The list of integers is the result of
        the decoding of the numbers coded in the [] part of the input string. The number
        of digits is the maximum number of digits used in the numbers in the [] part,
        where leading 0's are included in the count.
        If no '[]' part is found, return an tuple with an empty list. In case of error,
        returns (None,None)."""
        if self.__numberList is not None: return (self.__numberList,self.__digits)
        nums = self.getNumbers(openBracket,closeBracket)
        if nums is None: return (None,None)
        if nums=="": return (list(),0)
        numList = [ ]
        bclose = len(nums)
        posB = 1
        digits = 0
        while posB <= bclose:
            #always start with an digit
            posE = posB
            while posE < bclose and nums[posE].isdigit(): posE += 1
            # require at leaste one digit
            if posE == posB: return (None,None)
            # convert to integer
            digits = max(digits,posE - posB)
            iNum = int(nums[posB:posE])
            charE = nums[posE]
            if charE == StringNumberList.closeBracket: # last number
                numList.append( iNum )
                break
            elif charE == ',':  # single number
                numList.append( iNum )
                posB = posE + 1
                continue
            elif charE == '-':  # next comes end of range
                posB = posE + 1
                posE = posB
                while posE < bclose and nums[posE].isdigit(): posE += 1
                # require at leaste one digit
                if posE == posB: return (None,None)
                # convert to integer    
                digits = max(digits,posE - posB)
                iStop = int(nums[posB:posE])
                if iStop < iNum: return (None,None)
                numList += range(iNum,iStop+1)
                charE = nums[posE]
                if charE == ',':
                    posB = posE + 1
                    continue
                elif charE == StringNumberList.closeBracket:
                    break
                else:
                    return (None,None)
            else: # spurious character
                return (None,None)
        self.__numberList = numList
        self.__digits = digits
        return (self.__numberList,self.__digits)

    def convertStringList(self,codedString):
        return self.set(codedString).getStringList()

    def getStringList(self):
        """Convert coded string <valIn> into an expanded list. If <valIn> contains a syntax error, None is returned."""
        openBracket = self.__codedString.find(StringNumberList.openBracket)
        closeBracket = self.__codedString.find(StringNumberList.closeBracket,openBracket + 1)
        numList,digits = self.getNumberList(openBracket,closeBracket)
        if numList is None: return None
        if len(numList) == 0: return [ self.__codedString ]
        prefix = self.getPrefix(openBracket)
        if prefix is None: return None
        suffix = self.getSuffix(closeBracket)
        if suffix is None: return None
        return [ ('%s%0*d%s'.strip() % (prefix,digits,i,suffix)).strip() for i in numList ]

    
class JobOptionsFile:
    def __init__(self,filename):
        self.__filename = filename

    def filename(self):
        return self.__filename

    def setFilename(self,filename):
        self.__filename = filename

    def preRunAction(self):
        """Check that joboptions file can be found.
        Check is skipped if filename is empty"""
        filename = self.filename()
        if filename and not find_joboptions( filename ):
            raise JobOptionsNotFoundError( filename )


class PreJobOptionsFile(JobOptionsFile):
    def __init__(self,filename):
        JobOptionsFile.__init__(self,filename)

    def preRunAction(self):
        JobOptionsFile.preRunAction(self)


class PostJobOptionsFile(JobOptionsFile):
    def __init__(self,filename):
        JobOptionsFile.__init__(self,filename)

    def preRunAction(self):
        JobOptionsFile.preRunAction(self)


class GetFiles(TransformLogger):
    def __init__(self, listOfFiles, fromWhere='data', doCopy='Never', errorIfNotFound=True, keepDir=True, depth=0):
        TransformLogger.__init__(self)
        self.listOfFiles = listOfFiles
        self.fromWhere = fromWhere
        self.doCopy = doCopy
        self.keepDir = keepDir
        self.depth = depth
        self.errorIfNotFound = errorIfNotFound

    def preRunAction(self):
        get_files(self.listOfFiles, fromWhere=self.fromWhere, doCopy=self.doCopy, errorIfNotFound=self.errorIfNotFound, keepDir=self.keepDir, depth=self.depth)


class SQLiteSupport(GetFiles):
    """Make sure sqlite file access works on all nfs mounted disks by making copies to the run directory"""
    def __init__(self):
        self.__listOfFiles = [ "geomDB/*_sqlite", "sqlite200/*.db", "triggerDB/*.db" ]
        # if CMTSITE is STANDALONE, then files much be found.
        errorIfNotFound = os.environ.get('CMTSITE') == 'STANDALONE'
        GetFiles.__init__(self, self.__listOfFiles, doCopy='ifNotLocal', errorIfNotFound=errorIfNotFound, keepDir=True, )
        # remove any empty directories
        for dir in [ os.path.dirname(f) for f in self.__listOfFiles ]:
            if os.path.isdir(dir) and not os.listdir(dir):
                if os.path.islink(dir):
                    # if I leave the symlink in place CORAL will create an empty database file
                    # and therefore make athena fail...
                    self.logger().info('Removing local symlink of empty sqlite directory: %s', dir)
                    os.remove(dir)
                else:
                    self.logger().info('Removing local copy of empty sqlite directory: %s', dir)
                    os.rmdir(dir)

    def preRunAction(self):
        """Proceed with retrieve files with get_files()."""                
        GetFiles.preRunAction(self)


class ServiceOverride(PostJobOptionsFile,TransformLogger):
    def __init__(self,serviceName,membersDict,environmentDict=None):
        PostJobOptionsFile.__init__(self,"")
        TransformLogger.__init__(self)
        self.__service = serviceName
        self.__members = membersDict
        self.__environDict = environmentDict
##        self.__createdJoFile = None

    def preRunAction(self):
        nMem = len(self.__members)
        if nMem == 0: return
        environDict = { }
        if self.__environDict:
            for env in self.__environDict:
                if env in os.environ:
                    val = os.environ[env]
                    if val:
                        environDict[env] = val
                    else:
                        environDict[env] = self.__environDict[env]
            if not environDict: return
        members = { }
        for mem in self.__members:
            try:
                val = self.__members[mem] % environDict
            except KeyError:
                pass #don't set it if environment is needed but not present
            else:
                members[mem] = val
        if not members: return
        jo = [ "%s = Service( \"%s\" )" % (self.__service,self.__service) ]
        filename = self.__service
        for mem in members:
            val = members[mem]
            jo.append( "%s.%s = %r" % (self.__service, mem, val) )
            filename += '_%s_%s' % (mem,val)
        self.logger().info('Creating jobOptions file %s', filename)
        joFile = open(filename,'w')
        joFile.write( os.linesep.join(jo) + os.linesep )
        joFile.close()
        self.setFilename(filename)
##        self.__createdJoFile = filename
        # only now call baseclass preRunAction()
        PostJobOptionsFile.preRunAction(self)

    def postRunAction(self):
        """Do some cleanup"""
##        if self.__createdJoFile:
##            self.logger().info('Removing jobOptions file %s' % self.__createdJoFile)
##            fileutil.remove(self.__createdJoFile)
##            self.__createdJoFile = None

    def setMember(self,name,value):
        self.__members[name] = value

    def getMember(self,name):
        return self.__members[name]


class VersionString:
    """A class that can be used to extract the version numbers encoded in a string,
    and to do version number comparisons (== != < > etc).
    The encoding has to be provided in the constructor in the form of a compiled
    regular expression. A utility function withSeparator() is available to
    generate the regular expression for version numbers separated by a given
    character. The default pattern is numbers separated by a . (dot), with a
    maximum of 5 numbers (levels)"""
    @staticmethod
    def withSeparator(sep,maxfields):
        """Return compiled regular expression pattern where the
        version is a sequence of numbers separated with <sep>.
        It will match a maximum of <maxfields> consecutive version fields."""
        sep = re.escape(sep)
        if maxfields > 1:
            fields = (r'(?:%s(\d+))?' % sep) * (maxfields-1)
        else:
            fields = ''
        return re.compile(r'(?:^|%s)(\d+)%s(?:%s|$)' % (sep,fields,sep) )

    def __init__(self,version,pattern=None):
        if pattern is None: pattern = VersionString.withSeparator('.',5)
        self.version = version
        match = pattern.search(version)
        if match:
            self.numberList = [int(v) for v in match.groups() if v is not None]
        else:
            self.numberList = []

    def __str__(self):
        return self.version

    def __lt__(self,other,returnOnEqual=False):
        nSelf = len(self.numberList)
        nOther = len(other.numberList)
        n = min(nSelf,nOther)
        for i in range(n):
            numSelf = self.numberList[i]
            numOther = other.numberList[i]
            if numSelf < numOther: return True
            if numSelf > numOther: return False
        else:
            if nSelf < nOther: return True
            if nSelf > nOther: return False
        # they are equal!
        return returnOnEqual

    def __eq__(self,other):
        nSelf = len(self.numberList)
        nOther = len(other.numberList)
        if nSelf != nOther: return False
        for i in range(nSelf):
            if self.numberList[i] != other.numberList[i]: return False
        # they are equal!
        return True

    def __le__(self,other):
        return self.__lt__(other,True)

    def __gt__(self,other):
        return other.__lt__(self)

    def __ge__(self,other):
        return other.__le__(self)

    def __ne__(self,other):
        return not self.__eq__(other)

    def __nonzero__(self):
        return len(self.numberList) > 0

    def dump(self):
        print ('%s -> %s' % (self.version,self.numberList))

