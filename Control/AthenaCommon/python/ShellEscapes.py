# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: AthenaCommon/python/ShellEscapes.py
# Author: Wim Lavrijsen (LBNL, WLavrijsen@lbl.gov)

"""Provide shell escapes from the prompt by catching name and syntax errors."""

import os, sys, string
import exceptions, re
import Utils.unixtools as unixtools


### data ________________________________________________________________________
__version__ = '1.1.1'
__author__  = 'Wim Lavrijsen (WLavrijsen@lbl.gov)'

__all__ = [ 'ShellEscapes' ]

_shellCommands = [ 'echo', 'emacs', 'env', 'ls', 'less', 'more', 'pico', 'vi' ]
_unacceptable = [ 'false', 'true', 'sys' ]
_NAME = 'name'
_NAMEREX = re.compile( r"named? '?(?P<%s>[\w\d]+)'?" % _NAME )


### logging and messages --------------------------------------------------------
import logging
log = logging.getLogger( 'ShellEscapes' )


### LazyPython based hook for shell commands ====================================
class ShellEscapes:
   def __init__( self ):
      self._orig_ehook = sys.excepthook
      log.debug( 'shell short-cuts enabled' )

   def uninstall( self ):
      sys.excepthook = self._orig_ehook
      log.debug( 'shell short-cuts disabled' )

   def __call__( self, exctype, value, traceb ):
      global _shellCommands

      cmd = None

    # catch name and syntax errors to perform shell escapes if known
      if isinstance( value, exceptions.NameError ):
         res = _NAMEREX.search( str(value) )
         if res:
            cmd = res.group( _NAME )

          # disallow selected commands/executables
            if cmd in _unacceptable:
               cmd = None

      elif isinstance( value, exceptions.SyntaxError ):
         cmd = value.text[:-1]

    # execute command, if any
      if cmd != None:
         args = string.split( cmd )
         exe = args[0]

       # special cases
         if exe == 'cd' and len(args) == 2:
            os.chdir( args[1] )
            log.info( 'new directory: %s', AthConfig.shell.getcwd() )
            return

         if exe == 'help' and len(args) == 2:
            import __main__
            exec 'help( %s )' % args[1] in __main__.__dict__, __main__.__dict__
            return

       # cache shell command
         if not exe in _shellCommands:
            log.debug( 'accepting executable "%s"', exe )
            if unixtools.which( exe ):
               _shellCommands.append( exe )
            else:
               exe = None

         if exe != None:
            log.debug( 'executing shell command "%s"', cmd )
            os.system( cmd )
            return

    # nothing recognizable: normal exception processing
      self._orig_ehook( exctype, value, traceb )
