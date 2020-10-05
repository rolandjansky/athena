# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import os, sys, re, time
from PyJobTransformsCore import dummyaccess, rfio
import stat as statconsts

__doc__ = """A set of file utilities that can be used for several file systems (local files, rfio, castor)"""

__all__ = [ ]

defaultRetryMaxTime   = 1.0 # seconds
defaultRetryStartTime = 0.1 # seconds

def retry_function_time( func, args, retryException,
                         retryMaxTime = defaultRetryMaxTime,
                         retryStartTime = defaultRetryMaxTime ):
    """Call function several times if it throws a <retryException>.
    It will wait an increasing amount of time in between tries.
    First waiting time is <retryStartTime>, which is increased by
    a factor of 2 for each retry. It will give up and raise the
    original exception if it still fails after a total retry time of <retryMaxTime>.
    <func>: function to be called
    <args>: tuple with the function arguments, or the single function argument"""
    if type(args) != tuple: args = (args,)
    retryDelay = retryStartTime
    if retryDelay <= 0: retryDelay = 0.1 # avoid infinite loop
    OK = False
    tStart = time.time()
    while not OK:
        try:
            val = func( *args )
            OK = True
        except retryException:
            OK = False
            dt = time.time() - tStart
            argsStr = ', '.join( [ '%r' % a for a in args ] )
            if dt > retryMaxTime:
                print ("%s(%s) Failed" % (func.__name__,argsStr))
                raise
            time.sleep(retryDelay)
            retryDelay *= 2
            print ("Retrying %s(%s)" % (func.__name__,argsStr))

    return val


def retry_file_access( func, args, retryException = OSError,
                       retryMaxTime = defaultRetryMaxTime,
                       retryStartTime = defaultRetryMaxTime ):

    return retry_function_time( func, args, retryException, retryMaxTime, retryStartTime )

class AccessType:
    def __init__(self, name, matchPattern, replaceWith, baseModule, pathModule):
        self.name = name
        self.matchPat = matchPattern
        self.replacePat = replaceWith
        self.baseModule = baseModule
        self.pathModule = pathModule


    def matches(self,filename):
        return re.search( self.matchPat, filename ) is not None


    def cleanUp(self,filename):
        return re.sub( self.matchPat, self.replacePat, filename )


#supported access types
IO_LOCAL  = AccessType('local' , r'(.*)'     , r'\1'     , os  , os.path)
IO_RFIO   = AccessType('rfio'  , r'^rfio:'   , r'rfio:'       , rfio, rfio)
IO_CASTOR = AccessType('castor', r'^(?:rfio:)?/castor/', 'rfio:/castor/', rfio, rfio)
IO_XROOTD = AccessType('xrootd', r'^root:'   , r'root:'  , dummyaccess, dummyaccess )
IO_LFN    = AccessType('lfn'   , r'^LFN:'    , r'LFN:'   , dummyaccess, dummyaccess )
_accessTypes = ( IO_LFN, IO_XROOTD, IO_CASTOR, IO_RFIO, IO_LOCAL ) # IO_LOCAL should be last entry due to regex being very liberal


class Tee:
    """A file utility like unix 'tee'. It writes any output to a file and to screen (stdout by default).
    <option> if it has an 'a', append to logfile file, otherwise overwrite existing file."""
    def __init__(self,filename,options='',screen=sys.stdout):
        if 'a' in options:
            fileMode = 'a'
        else:
            fileMode = 'w'
        self.f = open (filename,fileMode)
        self.screen = screen

    #
    # override file functions
    #
    def write(self,s):
        self.screen.write(s)
        self.f.write(self,s)


    def writelines(self,ls):
        self.screen.writelines(ls)
        self.f.writelines(self,ls)


    def flush(self):
        self.screen.flush()
        self.f.flush()

    

def get_access_type(filename):
    for at in _accessTypes:
        if at.matches( filename ): return at
    return IO_LOCAL


def exists(filename):
    at = get_access_type(filename)
    return retry_file_access( at.pathModule.exists, filename )


def getsize(filename):
    at = get_access_type(filename)
    return retry_file_access( at.pathModule.getsize, filename )


def getmtime(filename):
    at = get_access_type(filename)
    return retry_file_access( at.pathModule.getmtime, filename )


def listdir(filename):
    at = get_access_type(filename)
    return retry_file_access( at.baseModule.listdir, filename )


def stat(filename):
    at = get_access_type(filename)
    return retry_file_access( at.baseModule.stat, filename )


def lstat(filename):
    at = get_access_type(filename)
    return retry_file_access( at.baseModule.lstat, filename )


def access(filename,mode):
    at = get_access_type(filename)
    return retry_file_access( at.baseModule.access, (filename,mode) )


def remove(filename):
    """Remove file <filename> if it exists. Only supported for local files."""
    at = get_access_type(filename)
    if at == IO_LOCAL:
        if exists(filename): retry_file_access( os.remove, filename )
    else:
        print ("WARNING: file %s file %s can not be removed" %
               (at.name, filename))
    

def exists_suffix(filename,suffixRE):
    """Test if a file exists in the same directory as <filename>, with the
    same name, but with an additional suffix given as a regular expression
    in <suffixRE>. It returns a list of all matching suffices, or an empty
    list if no matching filename+suffix was found."""
    sufs = []
    dirname  = os.path.dirname(filename) or os.curdir
    # if directory does not exist, then file will never exist (and prevent crash later on)
    if not os.path.isdir(dirname): return []
    filename = os.path.basename(filename)
    pat = re.compile( '^%s(%s)' % (filename,suffixRE) )
    for f in listdir(dirname):
        if pat.search( f ):
            sufs.append( pat.sub( r'\1', f ) )

    return sufs


def exists_suffix_number(filename):
    """Test if a file exists in the same directory as <filename>, with the same name,
    but a non-negative integer added at the end of the name. It returns the filename
    with the highest number added, or None if no such file exists."""
    sufs = exists_suffix(filename, r'[0-9]+')
    maxnum = -1
    found = None
    for suf in sufs:
        num = int(suf)
        if num > maxnum:
            maxnum = num
            found = filename + suf

    return found


def mode_string(filename):
    return mode_to_string( lstat(filename).st_mode )
    

def mode_to_string(mode):
    """Return the unix like string corresponding to the file access mode (rwxd etc)"""
    modeList = ['-']*10
    # first character (dir/symlink)
    if statconsts.S_ISDIR(mode):
        modeList[0] = 'd'
    elif statconsts.S_ISLNK(mode):
        modeList[0] = 'l'
    # user modes
    if mode & statconsts.S_IRUSR: modeList[1] = 'r'
    if mode & statconsts.S_IWUSR: modeList[2] = 'w'
    if   mode & statconsts.S_ISUID: modeList[3] = 's'
    elif mode & statconsts.S_IXUSR: modeList[3] = 'x'
    # group modes
    if mode & statconsts.S_IRGRP: modeList[4] = 'r'
    if mode & statconsts.S_IWGRP: modeList[5] = 'w'
    if   mode & statconsts.S_ISGID: modeList[6] = 's'
    elif mode & statconsts.S_IXGRP: modeList[6] = 'x'
    # other modes
    if mode & statconsts.S_IROTH: modeList[7] = 'r'
    if mode & statconsts.S_IWOTH: modeList[8] = 'w'
    if mode & statconsts.S_IXOTH: modeList[9] = 'x'

    return ''.join(modeList)
 
