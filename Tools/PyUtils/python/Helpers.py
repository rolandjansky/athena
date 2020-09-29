# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @author: Sebastien Binet <binet@cern.ch>
# @date:   March 2007
#
#

import sys
import os

from AthenaCommon.Logging import log

def ROOT6Setup(batch=False):
   log.info('executing ROOT6Setup')
   import builtins as builtin_mod
   oldimporthook = builtin_mod.__import__
   autoload_var_name = 'ROOT6_NamespaceAutoloadHook'
   batch_mode = bool(batch)

   def root6_importhook(name, globals={}, locals={}, fromlist=[], level=0):
       nonlocal batch_mode
       isroot = False
       bm = batch_mode
       if name=='ROOT' or (name[0:4]=='ROOT' and name!='ROOT.pythonization'):
          isroot = True
          batch_mode = None  # only set it on first ROOT import

       m = oldimporthook(name, globals, locals, fromlist, level)

       if m and isroot:
          log.debug('Python import module=%s, fromlist=%s', name, fromlist)
          if bm is not None:
             log.debug('Setting ROOT batch mode to %s', bm)
             m.gROOT.SetBatch(bm)

          if fromlist:
             # in this case 'm' is the final nested module already, don't walk the full 'name'
             vars = [ '.'.join(['', fl, autoload_var_name]) for fl in fromlist]
          else:
             vars = [ '.'.join([name, autoload_var_name]) ]

          for v in vars:
             try:
                mm = m
                # walk the module chain and try to touch 'autoload_var_name' to trigger ROOT autoloading of namespaces
                for comp in v.split('.')[1:]:
                   mm = getattr(mm, comp)
             except Exception:
                pass

       return m

   builtin_mod.__import__ = root6_importhook


import re
from tempfile import NamedTemporaryFile
class ShutUp(object):
    """
    A little helper class to keep ROOT silent...
    """
    DefaultFilter = [
        re.compile("Warning in <TClass::TClass>: no dictionary for class."),
        re.compile("Warning in <TEnvRec::ChangeValue>: duplicate entry."),
        re.compile("Error in <TStreamerInfo::BuildOld>:."),
        ]
    def __init__(self, filters = DefaultFilter):
        self._dummy = False # if dummy, we don't really shut-up ROOT...
        if os.environ.get('PYUTILS_SHUTUP_DEBUG', '0') == '1':
            self._dummy = True
            
        self.save_err  = open( '/dev/null', 'w' )
        self.save_out  = open( '/dev/null', 'w' )
        try:
            self.quiet_err = NamedTemporaryFile( suffix = ".msg.log" )
            self.quiet_out = NamedTemporaryFile( suffix = ".msg.log" )
        except OSError:
            # load problem ?
            # retry a bit later... (in 2 seconds)
            import time
            time.sleep(2) #
            try:
                self.quiet_err = NamedTemporaryFile( suffix = ".msg.log" )
                self.quiet_out = NamedTemporaryFile( suffix = ".msg.log" )
            except OSError:
                # then (implicitly) fallback on sys.stderr
                self._dummy = True

        self.filters = filters

        if not self._dummy:
            os.dup2( sys.stderr.fileno(), self.save_err.fileno() )
            os.dup2( sys.stdout.fileno(), self.save_out.fileno() )
        return

    def __del__ (self):
       self.save_err.close()
       self.save_out.close()
       return
    
    def mute(self):
        if not self._dummy:
            os.dup2( self.quiet_err.fileno(), sys.stderr.fileno() )
            os.dup2( self.quiet_out.fileno(), sys.stdout.fileno() )
        return
    
    def unMute(self):
        if not self._dummy:
            os.dup2( self.save_err.fileno(), sys.stderr.fileno() )
            os.dup2( self.save_out.fileno(), sys.stdout.fileno() )
            self.__filterRootMessages(self.quiet_err)
            self.quiet_err.seek(0)
            self.__filterRootMessages(self.quiet_out)
            self.quiet_out.seek(0)
        return

    def __filterRootMessages(self, fd):
        fd.seek(0)
        for l in fd.readlines():
            l = l.decode()
            printOut = True
            for filter in self.filters:
                if re.match(filter, l):
                    printOut = False
            if printOut:
                print ("PyRoot:",l.replace("\n",""))
            pass
        return

    # context-manager protocol
    def __enter__(self):
        return self.mute()
    def __exit__(self,exc_type, exc_val, exc_tb):
        return self.unMute()
    
