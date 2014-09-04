# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: PyUtils/python/rfio.py
# @purpose: simple ctypes-based python wrapper around libshift (from CASTOR2)
# @author: Sebastien Binet <binet@cern.ch>
# @date:   October 2008

__doc__ = "simple ctypes-based python wrapper around librfio (from CASTOR2)"
__version__ = "$Revision$"
__author__  = "Sebastien Binet <binet@cern.ch>"

import ctypes, os
from . import _rfio as c_api

_lib = c_api._libraries['libshift.so']

STRING = ctypes.c_char_p

### -- exports ----------------------------------------------------------------
__all__ = [
    'c_api', # the raw ctypes-wrapped library
    'access',
    'stat',
    'lstat',
    'open',
    ]


### -- helper methods ---------------------------------------------------------
def _raise_ioerror():
    global c_api
    raise IOError(c_api.rfio_serrno(),c_api.rfio_serror())

### -- data -------------------------------------------------------------------
DEFAULT_BUFFER_SIZE = 8 * 1024  # bytes

### -- classes ----------------------------------------------------------------
def __str__(self):
    return str(tuple([self.tv_sec,self.tv_nsec]))
c_api.timespec.__str__ = __str__
c_api.timespec.__repr__= __str__
del __str__

def __str__(self):
    return str(tuple([
        self.st_mode,  # protection bits
        self.st_ino,   # inode number
        self.st_dev,   # device
        self.st_nlink, # number of hard links
        self.st_uid,   # user ID of owner
        self.st_gid,   # group ID of owner
        self.st_size,  # size of file, in bytes
        self.st_atim,  # time of most recent access
        self.st_mtim,  # time of most recent content modification
        self.st_ctim,  # time of most recent metadata change
        ]))
c_api.stat.__str__ = __str__
c_api.stat.__repr__= __str__
del __str__

##
class File(object):
    """file-like object for files on CASTOR"""

    def __init__ (self, fname, mode='r'):
        super (File, self).__init__ ()
        self.name = fname
        self.mode = mode
        if not mode in ('r','w','a'):
            raise ValueError, "mode should be 'r', 'w' or 'a'"
        global c_api
        self._f = c_api.rfio_fopen (fname, mode)
        if c_api.rfio_serrno():
            _raise_ioerror()
        self.fd = c_api.rfio_fileno (self._f)

    def close (self):
        fd = self.fd
        if fd is not None:
            self.fd = None
            global c_api
            if c_api.rfio_close (fd):
                raise_ioerror()
        return

    def __del__ (self):
        try:
            self.close()
        except:
            pass
        
    def fileno (self):
        if self.fd is not None:
            return self.fd
        raise ValueError ('I/O operation on closed file')

    def flush (self):
        global c_api
        if c_api.rfio_fflush (self._f):
            _raise_ioerror()
        return
    
    def next (self):
        line = self.readline()
        if not line:
            raise StopIteration
        return line

    def tell (self):
        bytes = c_api.rfio_ftell (self._f)
        if bytes < 0:
            _raise_ioerror()
        return bytes

    def read (self, n=-1):
        if n < 0:
            return self.readall()
        buf = c_api.STRING('\0'*n)
        rc = c_api.rfio_fread (buf, 1, n, self._f)
        if rc < 0:
            _raise_ioerror()
        return str(buf.value)

    def readall (self):
        res = str()
        while True:
            data = self.read (DEFAULT_BUFFER_SIZE)
            if not data:
                break
            res += data
        return res

    def readline (self, limit = -1):
        r"""Read and return a line from the stream.

        If limit is specified, at most limit bytes will be read.
        """
        if limit is None:
            limit = -1
        if not isinstance (limit, (int, long)):
            raise TypeError ("limit must be an integer")
        res = str()
        while limit < 0 or len(res) < limit:
            data = self.read (DEFAULT_BUFFER_SIZE)
            if not data:
                break
            res += data
            if res.endswith('\n'):
                break
        return res

    def readinto (self, b):
        nb = c_api.rfio_fread (b, 1, len(b), self._f)
        if nb < 0:
            _raise_ioerror()
        return nb
    
    def __iter__ (self):
        return self

    def readlines (self, hint=None):
        """Return a list of lines from the stream.

        hint can be specified to control the number of lines read: no more
        lines will be read if the total size (in bytes/characters) of all
        lines so far exceeds hint.
        """
        if hint is None:
            hint = -1
        if not isinstance (hint, (int, long)):
            raise TypeError ("hint must be an integer")
        if hint <= 0:
            return list(self)
        n = 0
        lines = []; _append = lines.append
        for line in self:
            _append (line)
            n += len(line)
            if n >= hint:
                break
        return lines

    def seek (self, offset, whence=0):
        rc = c_api.rfio_fseek (self._f, offset, whence)
        if rc < 0:
            _raise_ioerror()
        return

    def write (self, b):
        # FIXME: use buffer protocol...
        buf = c_api.STRING(b)
        nb = c_api.rfio_fwrite (buf, 1, len(buf.value), self._f)
        if nb < 0:
            _raise_ioerror()
        return nb

    def writelines (self, lines):
        for line in lines:
            self.write (line)

    ### Context manager ###
    def __enter__ (self):
        """Context management protocol. Returns self."""
        return self

    def __exit__ (self, *args):
        self.close()
        
    pass # class File

### -- module methods ---------------------------------------------------------
def open (fname, mode='r'):
    return File (fname, mode)

def stat (fname):
    st = c_api.stat()
    if c_api.rfio_stat (fname, st):
        _raise_ioerror()
    return st

def lstat (fname):
    st = c_api.stat()
    if c_api.rfio_lstat (fname, st):
        _raise_ioerror()
    return st

def access (fname, mode):
    """access(path, mode) -> 1 if granted, 0 otherwise"""
    rc = c_api.rfio_access (fname, mode)
    if rc < 0:
        return 0
    return 1

