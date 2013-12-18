# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os,commands,time
import stat as statconsts
import envutil

__doc__ = """Module with utilities for rfio files"""

class RFIOError(IOError):
    def __init__(self,*vargs):
        IOError.__init__(self,*vargs)

def _remove_prefix(filename,prefix):
    if filename.startswith(prefix): filename = filename[len(prefix):]
    return filename


def rfstat(filename):
    """Return tuple (status,output) of rfstat shell command. Output is a list of strings
    (one entry per line). Raises RFIOError if rfstat command is not found."""
    statcmd = 'rfstat'
    if not envutil.find_executable(statcmd):
        raise RFIOError( '%s not found in PATH' % statcmd )
    cmd = '%s %s' % (statcmd,_remove_prefix(filename,'rfio:'))
    status,output = commands.getstatusoutput( cmd )
    status >>= 8

    return (status, output.split(os.linesep))


def rfdir(dirname):
    """Return tuple (status,output) of rfdir shell command. Output in a list of strings (one entry per line).
    The format is the same as the unix shell command \'ls -la\'
    Raises RFIOError if rfdir command is not found"""
    dircmd = 'rfdir'
    if not envutil.find_executable(dircmd):
        raise RFIOError( '%s not found in PATH' % dircmd )
    cmd = '%s %s' % (dircmd,_remove_prefix(dirname,'rfio:'))
    status,ls_la = commands.getstatusoutput( cmd )
    status >>= 8
    
    return (status, ls_la.split(os.linesep))


def rfstat_dict(filename):
    """Return dictionary with name:value pairs of the rfstat output. Returns None
    if rfstat returns an error code (i.e. file does not exist)."""
    status,output = rfstat(filename)
    if status: return None
    rfdict = { }
    for line in output:
        colon = line.index(':')
        name = line[:colon].strip()
        value = line[colon+1:].strip()
        rfdict[name] = value

    return rfdict
    
    
def rfstat_item(filename,item):
    """Return the contents of <item> in the rfstat output"""
    status,output = rfstat(filename)
    if status: return None
    for line in output:
        if line.startswith( item ):
            colon = line.index(':')
            return line[colon+1:].strip()

    # nothing found
    return None


def listdir(dirname):
    """Return the contents of directory <dirname> in a python list a-la os.listdir(),
    i.e. only the filenames, and not including the current dir (.) and parent dir (..)""" 
    status,ls_la = rfdir(dirname)
    if status: raise RFIOError('Directory %s not found' % dirname)
    dir = [ ]
    for d in ls_la:
        dd = d.split()[-1]
        if dd != os.curdir and dd != os.pardir:
            dir.append( dd )

    return dir
  

def exists(filename):
    status,output = rfstat(filename)
    return status == 0


def getsize(filename):
    """Return size of file <filename> in bytes"""
    return int(rfstat_item(filename,'Size'))


def getmtime(filename):
    return lstat(filename).st_mtime


class StatResult:
    def __init__(self):
        self.st_mode  = 0
        self.st_ino   = 0
        self.st_dev   = 0
        self.st_nlink = 0
        self.st_uid   = 0
        self.st_gid   = 0
        self.st_size  = 0
        self.st_atime = 0
        self.st_mtime = 0
        self.st_ctime = 0

        self.__items = [ i.upper() for i in self.__dict__ if i.startswith('st_') ]


    def __getitem__(self,idx):
        for i in self.__items:
            if idx == getattr(statconsts,i):
                return getattr(self,i.lower())
        return None


    def __setitem__(self,idx,val):
        for i in self.__items:
            if idx == getattr(statconsts,i):
                setattr(self,i.lower(),val)
        

    def __str__(self):
        vals = []
        for i in range(len(self.__items)):
            vals.append( self[i] )
        return str( tuple(vals) )



def _stat_time(time_string):
    t = time.mktime(time.strptime(time_string))
    if os.stat_float_times():
        return float(t)
    else:
        return int(t)
    


def lstat(filename):
    st = StatResult()
    output = rfstat_dict(filename)
    if output is None: raise RFIOError('file %s not found' % filename)
    for name in output:
        value = output[name]
        if name == 'Device':
            st.st_dev = eval('0x' + value)
        elif name.startswith('Inode'):
            st.st_ino = int(value)
        elif name.find('blocks') != -1:
            st.st_blocks = int(value)
        elif name == 'Protection':
            octal = value.split()[-1][1:-1]
            st.st_mode = eval('0' + octal)
        elif name == 'Hard Links':
            st.st_nlink = int(value)
        elif name == 'Uid':
            st.st_uid = int(value.split()[0])
        elif name == 'Gid':
            st.st_gid = int(value.split()[0])
        elif name.startswith('Size'):
            st.st_size = int(value)
        elif name == 'Last access':
            st.st_atime = _stat_time(value)
        elif name == 'Last modify':
            st.st_mtime = _stat_time(value)
        elif name == 'Last stat. mod.':
            st.st_ctime = _stat_time(value)

    return st

# never follow symbolic links, so stat == lstat
stat = lstat


def access(filename,mode):
    if mode == os.F_OK: return exists(filename)

    st = stat(filename)
    filemode = st.st_mode
    uid = st.st_uid
    gid = st.st_gid
    if mode & os.R_OK:
        rOK = ( filemode & statconsts.S_IROTH ) or \
              ( filemode & statconsts.S_IRGRP and os.getgid()  == gid ) or \
              ( filemode & statconsts.S_IRUSR and os.getuid()  == uid ) or \
              ( filemode & statconsts.S_ISGID and os.getegid() == gid ) or \
              ( filemode & statconsts.S_ISUID and os.geteuid() == uid )
    else:
        rOK = True

    if mode & os.W_OK:
        wOK = ( filemode & statconsts.S_IWOTH ) or \
              ( filemode & statconsts.S_IWGRP and os.getgid()  == gid ) or \
              ( filemode & statconsts.S_IWUSR and os.getuid()  == uid ) or \
              ( filemode & statconsts.S_ISGID and os.getegid() == gid ) or \
              ( filemode & statconsts.S_ISUID and os.geteuid() == uid )
    else:
        wOK = True

    if mode & os.X_OK:
        xOK = ( filemode & statconsts.S_IXOTH ) or \
              ( filemode & statconsts.S_IXGRP and os.getgid()  == gid ) or \
              ( filemode & statconsts.S_IXUSR and os.getuid()  == uid ) or \
              ( filemode & statconsts.S_ISGID and os.getegid() == gid ) or \
              ( filemode & statconsts.S_ISUID and os.geteuid() == uid )
    else:
        xOK = True

    return rOK and wOK and xOK

    
