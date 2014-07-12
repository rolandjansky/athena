# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## This class is borrowed from flock.py, written by Jordan Callicoat and
## available from an online python cookbook (aspn.activestate.com)
## Updated to use lockf for actual network atomicity by Peter Onyisi
## Using system lock to ensure that lock released even in case of segfault

import os
import socket
import time
import fcntl

class SingleAppInstance(object):
  '''Class to handle creating and removing lockfiles'''
  
  # custom exceptions
  
  class FileLockAcquisitionError(Exception): pass
  class FileLockReleaseError(Exception): pass
  
  # convenience callables for formatting
  addr = lambda self: '%d@%s' % (self.pid, self.host)
  fddr = lambda self: '<%s %s>' % (self.path, self.addr())
  pddr = lambda self, lock: '<%s %s@%s>' %\
                              (self.path, lock['pid'], lock['host'])
  
  def __init__(self, path, debug=None, blocking = False):
    self.pid   = os.getpid()
    self.host  = socket.gethostname()
    self.path  = path
    self.debug = debug # set this to get status messages
    self.locked = False
    self.lockfile = None
    self.blocking = blocking
  
  def acquire(self):
    '''Acquire a lock, returning self if successful, False otherwise'''
    if self.islocked():
      if self.debug:
        lock = self._readlock()
        print 'Previous lock detected: %s' % self.pddr(lock)
      return False
    try:
      fd = os.open(self.path, os.O_RDWR | os.O_CREAT, 0777)
      fh = os.fdopen(fd, 'r+')
      self.lockfile = fh
      fh.seek(0)
      try:
        fcntlflag = fcntl.LOCK_EX
        if not self.blocking:
          fcntlflag |= fcntl.LOCK_NB
        fcntl.lockf(fh, fcntlflag)
        self.locked = True
      except IOError:
        print 'Unable to acquire lock on %s: existing lock %s' % (self.path, fh.read())
        fh.close()
        return False
      fh.write(self.addr())
      fh.truncate()
      fh.flush()
      os.fsync(fh)
      if self.debug:
        modtime = os.stat(self.path)
        outstring = 'Acquired lock: '+ self.fddr() + ' at time '+ time.ctime(modtime.st_mtime)
        print outstring
    except Exception, e:
      if os.path.isfile(self.path):
        try:
          # my guess is this causes problems
          #os.unlink(self.path)
          pass
        except:
          pass
      raise self.FileLockAcquisitionError(
             'Error acquiring lock: %s, reason %s' % (self.fddr(), e))
    return self
  
  def release(self):
    '''Release lock, returning self'''
    if self.ownlock():
      try:
        fh = self.lockfile
        fcntl.lockf(fh, fcntl.LOCK_UN)
        self.locked = False
        self.lockfile.close()
        self.lockfile = None
        if not self.blocking:
          # some space for a small race here unfortunately
          os.unlink(self.path)
        if self.debug:
          outstring  = 'Released lock: ' + self.fddr() +' at time ' +  time.ctime()
          print outstring
      except Exception, e:
        print e
        raise self.FileLockReleaseError(
          'Error releasing lock: %s, reason %s' % (self.fddr(), e))
    return self
  
  def _readlock(self):
    '''Internal method to read lock info'''
    try:
      lock = {}
      fh   = open(self.path)
      data = fh.read().rstrip().split('@')
      fh.close()
      lock['pid'], lock['host'] = data
      return lock
    except:
      return {'pid': 8**10, 'host': ''}
  
  def islocked(self):
    '''Check if we already have a lock'''
    return self.locked
  
  def ownlock(self):
    '''Check if we own the lock'''
    return self.locked
  
  def __del__(self):
    '''Magic method to clean up lock when program exits'''
    self.release()

  def __enter__(self):
    self.acquire()
    return self

  def __exit__(self, typ, value, tb):
    self.release()
    return self
