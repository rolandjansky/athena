# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @author: Sebastien Binet <binet@cern.ch>
# @date:   March 2007
#
#
from __future__ import with_statement, print_function

__author__  = "Sebastien Binet <binet@cern.ch>"

import sys
import os
import six

from AthenaCommon.Logging import log

# import xml before ROOT to prevent crashes (LCG_96): ATEAM-597
# should be OK to remove from LCG_97 on
import xml.etree.cElementTree  # noqa: F401

def ROOT6Setup():
   log.info('executing ROOT6Setup')
   if six.PY3:
      import builtins as builtin_mod
   else:
      import __builtin__ as builtin_mod
   oldimporthook = builtin_mod.__import__
   autoload_var_name = 'ROOT6_NamespaceAutoloadHook'
   
   def root6_importhook(name, globals={}, locals={}, fromlist=[], level=-1):
       if six.PY3 and level < 0: level = 0
       m = oldimporthook(name, globals, locals, fromlist, level)
       if m and (m.__name__== 'ROOT' or name[0:4]=='ROOT') \
             and (name!='ROOT' or fromlist is not None): # prevent triggering on just 'import ROOT'; see ATEAM-597
          log.debug('Python import module=%s  fromlist=%s', name, str(fromlist))
          if fromlist:
             #MN: in this case 'm' is the final nested module already, don't walk the full 'name'
             vars = [ '.'.join(['', fl, autoload_var_name]) for fl in fromlist]
          else:
             vars = [ '.'.join([name, autoload_var_name]) ]
          for v in vars:
             try:
                mm = m
                #MN: walk the module chain and try to touch 'autoload_var_name' to trigger ROOT autoloading of namespaces
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
            if six.PY3:
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
    
