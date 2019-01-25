# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# File: AthenaCommon/python/Include.py
# Author: Wim Lavrijsen (WLavrijsen@lbl.gov)

"""Handle Athena options file inclusion. Files are located through the
JOBOPTSEARCHPATH envar and globally executed. If requested, files will be
traced. Note, however, that this option interferes with pdb and trace."""

import os, sys, re, fnmatch
from Utils.unixtools import FindFile


### data ---------------------------------------------------------------------
__version__ = '1.3.0'
__author__  = 'Wim Lavrijsen (WLavrijsen@lbl.gov)'

__all__ = [ 'include', 'marker', 'lineMarker', 'fidMarker',
            'callMarker', 'returnMarker', 'activeMarker', 'silentMarker',
            'tracedMarker' ]

marker       = ' -+-'
__marker__   = ' -+-'
silentMarker = ' '
activeMarker = 'A'
tracedMarker = 'T'
callMarker   = 'C'
returnMarker = 'R'
lineMarker   = '%3d'
fidMarker    = '%2d'

# do not trace any files that fnmatch these names:
excludeTracePattern = [
   '*/GaudiPython/*', '*/GaudiKernel/*',          # Gaudi framework files
   '*/InstallArea/*/AthenaCommon/*',              # Athena framework files
   '*/python%d.%d/*' % sys.version_info[:2],      # python system
   '*/InstallArea/python/*/*Conf.py',             # generated conf files
   '*/PyUtils/decorator.py',                      # very verbose
   '*/PyUtils/Decorators.py',                     # ditto
   '*/PyUtils/Helper*.py',                        # ditto
   '*/lib/ROOT.py',                               # ROOT import hook gets executed very many times
   ]

# unless they are explicitly included here:
includeTracePattern = [ '*/AthenaCommon/Bootstrap.py',
   '*/AthenaCommon/Atlas.UnixStandardJob.py' ]


### logging and messages -----------------------------------------------------
import Logging
log = Logging.logging.getLogger( 'Athena' )

class IncludeError( RuntimeError ):
   pass


### files locator ------------------------------------------------------------
try:
   optionsPathEnv = os.environ[ 'JOBOPTSEARCHPATH' ]
except Exception:
   optionsPathEnv = os.curdir

optionsPath = re.split( ',|' + os.pathsep, optionsPathEnv )
if '' in optionsPath:
   optionsPath[ optionsPath.index( '' ) ] = str(os.curdir)


### files inclusion ----------------------------------------------------------
def basename2( fn ):
   return os.path.join( os.path.basename( os.path.dirname( fn ) ), os.path.basename( fn ) )

_filecache = {}
_linecache = {}
_notrcache = {}

class Include( object ):
   fid = 0

   def __init__( self, show = True, collect = 1, clean = False ):
      self._show = show
      self._once = []
      self._fcurrent = ''
      self.msg = Logging.logging.getLogger( 'Athena' )

      if clean:
         self._workspace = {}
      else:
         import __main__
         self._workspace = __main__.__dict__

      if collect:
         self._collect = 1
         
   def setShowIncludes( self, show ):
      self._show = show

   def setCollect( self, collect ):
      if collect and not hasattr( self, '_collect' ):
         self._collect = 1
      elif not collect and hasattr( self, '_collect' ):
         del self._collect

   def setClean( self, clean ):
      import __main__
      if clean:
         if self._workspace is __main__.__dict__:
            self._workspace = {}
            self._workspace.update( __main__.__dict__ )
      else:
         self._workspace = __main__.__dict__

   def __call__( self, fn, *args, **kw ):
      """Include <fn> in the current scope by executing it globally."""

    # some basic sanity check first  
      if isinstance(fn, str) and len(fn) == 0:
         raise IncludeError("can not 'include' empty filenames")
      
    # don't include file if not allowed (has to be exact match in py code)
      if fn in self._once:
         self.msg.debug( 'file "%s" is blocked; not included', fn )
         return

    # locate the file
      name = FindFile( os.path.expanduser( os.path.expandvars( fn ) ), optionsPath, os.R_OK )
      if not name:
         name = FindFile( os.path.basename( fn ), optionsPath, os.R_OK )
         if name:
            self.msg.warning( 'using %s instead of %s', name, fn )
         else:
            raise IncludeError( 'include file %s can not be found' % fn )

      self.msg.debug( 'located %s as %s', fn, name )

    # print if 'show' is set to non-null
      show = self._show
      if 'show' in kw:
         show = kw[ 'show' ]

    # notify of start of file inclusion
      if show:
         self.msg.info( 'including file "%s" with ID %d', fn, self.fid )
      else:
         self.msg.info( 'including file "%s"', fn )
      self._fcurrent = name

    # actual inclusion
      if show and self._doTrace( name ):
       # traced
         _filecache[ name ] = open( name, 'r' ).readlines()
         _linecache[ name ] = 0, self.fid
         self.fid += 1
  
         sys.settrace( self._trace_include )
         execfile( name, self._workspace, self._workspace )
         sys.settrace( sys._getframe(0).f_trace )

       # finish file printout
         ncur, fid = _linecache[ name ]
         buf  = _filecache[ name ]
         for i in range( ncur, len(buf) ):
            self._oneline( fid, i, silentMarker, buf )

         del _filecache[ name ]
         del _linecache[ name ]

         self.msg.info( 'end of "%s"', fn )

      else:
       # non-traced
         execfile( name, self._workspace, self._workspace )

      if hasattr( self, '_collect' ):
         if not self._collect % 10:
            import gc
            gc.collect()
         else:
            self._collect += 1

   def block( self, fn ):
      """Disallow the given filename(s) from being included again."""

      if type(fn) == list:
         self._once += fn
      else:
         self._once.append( fn )

   def unblock( self, fn ):
      """Re-allow the given filename from being included."""

      self._once.remove( fn )

 # match files to trace ------------------------------------------------------
   def _doTrace( self, fn ):
    # Tracing into non-included files is controlled with two variables:
    # excludeTracePattern and includeTracePattern. The former goes first,
    # the latter can override any excluded results.

      if fn in _notrcache:
         return False

      doTrace = True
      for tracePattern in excludeTracePattern:
         if fnmatch.fnmatch( fn, tracePattern ):
            doTrace = False
            break

      if not doTrace:
         for tracePattern in includeTracePattern:
            if fnmatch.fnmatch( fn, tracePattern ):
               doTrace = True
               break

      if not doTrace:
         _notrcache[ fn ] = 1

      return doTrace

 # code tracer ---------------------------------------------------------------
   def _trace_include( self, frame, event, arg ):
      fn  = frame.f_code.co_filename
      if not os.path.exists( fn ):
         fn = FindFile( basename2( fn ), sys.path, os.R_OK )

      if not ( fn and self._doTrace( fn ) ):
         return self._trace_include

      if fn not in _filecache:
       # wait until importing of the module is done to minimize pollution
         f = frame.f_back
         while f is not None:
            try:
               if 'import' in _filecache[ f.f_code.co_filename ][ f.f_lineno ]:
                  return self._trace_include
            except Exception:
               pass
            f = f.f_back
         del f

       # import is done, and we're back, accept this file from this point on
         _filecache[ fn ] = open( fn, 'r' ).readlines() or '\n'
         _linecache[ fn ] = sys.maxint, self.fid
         self.fid += 1

      lno = frame.f_lineno
      aln = lno - 1 > 0 and lno - 1 or 0

      ncur, fid = _linecache[ fn ]
      buf = _filecache[ fn ]

      if self._fcurrent != fn:
         self.msg.info( 'continued trace of "%s"', basename2( fn ) )
         self._fcurrent = fn

      if event == 'line':
         for i in range( ncur, aln ):
            self._oneline( fid, i, silentMarker, buf )

         if ncur <= aln:
            self._oneline( fid, aln, activeMarker, buf )
         elif 0 <= aln:
            self._oneline( fid, aln, tracedMarker, buf )

         if ncur < lno:
            _linecache[ fn ] = lno, fid

      elif event == 'call':
         if lno < ncur:
            self._oneline( fid, aln, callMarker, buf )

      elif event == 'return':
         if lno < ncur:
            fln = frame.f_code.co_firstlineno - 1
            self._oneline( fid, fln, returnMarker, None )

      return self._trace_include

 # formatted line printer
   def _oneline( self, fid, lineno, detail, buf ):
      print marker, fidMarker % fid, lineMarker % lineno, detail,

      try:

       # simple eol case
         if not buf or not buf[ lineno ]:
            print
            return

       # in general, an interpreter "line" may be longer than a file line
         line = buf[ lineno ].rstrip()
         while line and ( line[-1] == '(' or line[-1] == '\\' ):
          # this line appears to have a continuation ...
            try:
             # output traced line
               print line

             # output continued line
               lineno += 1
               print marker, fidMarker % fid, lineMarker % lineno, detail,
               line = buf[ lineno ].rstrip()
            except IndexError:
             # shouldn't happen; but must mean that the diagnosis above is
             # wrong and that there is no continuation, keep silent
               break

         print line

      except IndexError:
         log.warning( 'index (%d) out of range while scanning include file %d', lineno, fid )


# use this for backward compatibility
include = Include()
