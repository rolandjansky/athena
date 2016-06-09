# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @author: Sebastien Binet <binet@cern.ch>
# @date:   March 2007
#
#
from __future__ import with_statement

__version__ = "$Revision$"
__author__  = "Sebastien Binet <binet@cern.ch>"

import sys
import os

from AthenaCommon.Logging import log


def ROOT6Setup():

   def addROOTIncludePaths():
      """
      Fill ROOT include path list for entries for all packages found in CMTPATH
      """

      # Most projects make their headers available to root under
      # root-include-path.  But DetCommon does not.
      if 'CMTPATH' in os.environ:
         import glob
         import PyUtils.RootUtils as ru
         interp = ru.import_root().gInterpreter
         for p in os.environ['CMTPATH'].split(':'):
            if p.find('DetCommon')>=0:
               idir = os.path.join (p, 'InstallArea', 'include')
               for ii in glob.glob (os.path.join (idir, '*')):
                  interp.AddIncludePath (ii)
            

      # Also need to make Geant headers available.
      if 'G4INCLUDE' in os.environ:
          import PyUtils.RootUtils as ru
          interp = ru.import_root().gInterpreter
          interp.AddIncludePath( os.environ['G4INCLUDE'] )



   def cppyyFakeCintex():
      class Cintex:
         def Enable(self):
             pass
      
      _load = cppyy.loadDict
      def loadDict(dict):
         if dict.find('Reflex') >= 0:
            log.debug(" LoadDict: ignoring dict " + dict )
         else:
            log.debug(" LoadDict: loading dict " + dict )
            return _load(dict)
      
      cppyy.Cintex = Cintex()
      cppyy.hasFakeCintex = True
      cppyy.loadDict = loadDict


   def install_root6_importhook():
      import __builtin__
      oldimporthook = __builtin__.__import__
      autoload_var_name = 'ROOT6_NamespaceAutoloadHook'
      
      def root6_importhook(name, globals={}, locals={}, fromlist=[], level=-1):
          if name == 'PyCintex':
             import sys, traceback
             source, line, f, t = traceback.extract_stack( sys._getframe(1) )[-1]
             log.warning( 'PyCintex imported (replace with import cppyy) from: %s:%d'%(source,line) )
          m = oldimporthook(name, globals, locals, fromlist, level)
          if m and (m.__name__== 'ROOT' or name[0:4]=='ROOT'):
             log.debug('Python import module=%s  fromlist=%s'%(name, str(fromlist)))
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
                except:
                   pass
          return m
      
      __builtin__.__import__ = root6_importhook
      

   try:
      import cppyy
      # let cppyy pretend to be PyCintex (and prevent subsequent imports of PyCintex)
      sys.modules['PyCintex'] = PyCintex = cppyy
   except ImportError, e:
    # handle a somewhat common mistake
      import traceback
      traceback.print_exception( sys.exc_type,
          '%s, ROOT version or setup problem?' % str(e), sys.exc_traceback )
      sys.exit( 1 )

   try:
       # test if we have Cintex (ROOT5)
       cppyy.Cintex.Debug
       # if the previous line did not throw exception, then we have ROOT5 with Cintex and Reflex
       cppyy.hasCintex = True
   except AttributeError, e:
       # no Cintex!  do ROOT6 stuff
       # but don't initialize more than once
       if not hasattr(cppyy, 'hasFakeCintex'):
          log.info('executing ROOT6Setup')
          cppyyFakeCintex()
          addROOTIncludePaths()
          install_root6_importhook()

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
            printOut = True
            for filter in self.filters:
                if re.match(filter, l):
                    printOut = False
            if printOut:
                print "PyRoot:",l.replace("\n","")
            pass
        return

    # context-manager protocol
    def __enter__(self):
        return self.mute()
    def __exit__(self,exc_type, exc_val, exc_tb):
        return self.unMute()
    
###
from contextlib import contextmanager
@contextmanager
def restricted_ldenviron(projects=None, msg=None):
    """
    a context helper to limit ROOT automatic loading of dictionaries
    to a given set of cmt-projects (LCGCMT, AtlasCore, ...)
    """

    if projects is None:
        # nothing to do.
        # execute user stuff
        yield
        # end of story
        return

    # Bypass the rest of the function in case CMake is used and not CMT
    import os
    if os.environ.get( 'CMTPATH', '' ) == '':
       yield
       return

    if isinstance(projects, str):
        projects = [p.strip() for p in projects.split() if p.strip() != '']
    if not isinstance(projects, (list, tuple)):
        raise TypeError("projects has to be a list, tuple or space-separated"
                        " string")

    import os, sys
    from PyCmt.Cmt import CmtWrapper
    cmt = CmtWrapper()
    def _get_projects_paths(project_names, cmt=cmt):
        """return the list of paths of a project and its dependencies
        """
        if isinstance(project_names, str):
            project_names = project_names.split()
        projects = []
        for proj_name in project_names:
            projects.extend(cmt.project_deps(proj_name) + [proj_name])
        projects = list(set(projects))
        proj_paths = []
        tree = cmt.project_tree()
        for p in projects:
            path = tree[p].path
            proj_paths.append(path)
        return proj_paths
        
    # select only projects user asked for (and their dependencies)
    usr_projects = _get_projects_paths(projects)
    # get the same thing for all the projects we are currently using
    cur_projects = _get_projects_paths(cmt.projects_dag()[0].name)
    # intersect:
    blacklist = [p for p in cur_projects if p not in usr_projects]
    
    original_env = os.environ.copy()
    orig_ld_path = os.environ.get('LD_LIBRARY_PATH', '')
    orig_bin_path= os.environ.get('PATH', '')
    
    if 0:
        print ":::cmt projects:",usr_projects
        print ":::blacklist:",blacklist

    def _slim_down(orig_path, blacklist=blacklist):
        """helper method to slim down a path by removing every entry which
        starts with some element of the blacklist
        """
        new_path = []
        for d in orig_path.split(os.pathsep):
            # removing every entry which is in the blacklist
            burned = [p for p in blacklist if d.startswith(p)]
            if len(burned) == 0:
                new_path.append(d)
        return os.pathsep.join(new_path)
            
    # slim-down LD_LIBRARY_PATH
    new_ld_path = _slim_down(orig_ld_path)
    # and the PATH (to keep everything consistent)
    new_bin_path= _slim_down(orig_bin_path)
    
    # commit the new values
    os.environ['LD_LIBRARY_PATH'] = new_ld_path
    os.environ['PATH'] = new_bin_path

    # execute user stuff...
    try:
        yield
    finally:
        # restore original environment
        os.environ.update(original_env)
    
        
