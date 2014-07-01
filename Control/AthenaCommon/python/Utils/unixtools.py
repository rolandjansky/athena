# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: AthenaCommon/python/Utils/unixtools.py
# Author: Wim Lavrijsen  (LBNL, WLavrijsen@lbl.gov)

"""Unix-like tools and helpers."""

import os, string, sys


### data ________________________________________________________________________
__version__ = '1.0.0'
__author__  = 'Wim Lavrijsen (WLavrijsen@lbl.gov)'

__all__ = [ 'FindFile', 'which', 'where',
            'find_datafile',]


### helper ----------------------------------------------------------------------
def FindFile( filename, pathlist, access ):
   """Find <filename> with rights <access> through <pathlist>."""

 # special case for those filenames that already contain a path
   if os.path.dirname( filename ):
      if os.access( filename, access ):
         return filename

 # test the file name in all possible paths until first found
   for path in pathlist:
      f = os.path.join( path, filename )
      if os.access( f, access ):
         return f

 # no such accessible file avalailable
   return None


### UNIX-style which ------------------------------------------------------------
def which( filename, env = os.environ ):
   """Search for <filename> through the PATH in environment <env>. Only executable
files will be returned."""

 # retrieve the value of the PATH environment variable
   try:
      p = env[ 'PATH' ]
   except KeyError:
      p = os.defpath

   return FindFile( filename, string.split( p, os.pathsep ), os.X_OK )


### "which" for python files ----------------------------------------------------
def where( filename, prepath = [] ):
   """Search for <filename> in the python path and the given <prepath>."""

 # look in the given path and in the python path
   pathlist = prepath + sys.path

   for ext in [ '', '.py', '.pyc', '.so' ]:
      result = FindFile( filename + ext, pathlist, os.R_OK )
      if result:
         break

   return result

### pathresolver-like helper function -----------------------------------------
def find_datafile(fname, pathlist=None, access=os.R_OK):
   """the python equivalent to the C++ PathResolver for datafiles.
   """
   if pathlist is None:
      pathlist = os.getenv('DATAPATH').split(os.pathsep)

   if not isinstance(pathlist, list):
      raise TypeError("pathlist must be a list or an iterable (got %s)" %
                      type(pathlist))

   return FindFile(fname, pathlist, access)
