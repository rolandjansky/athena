#!/bin/sh
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# athena.py is born as shell script to preload some optional libraries
#
"""date"

# defaults
export USETCMALLOC=0
export USEIMF=0

# but use tcmalloc by default if TCMALLOCDIR is defined
if [ -n "$TCMALLOCDIR" ]; then
    export USETCMALLOC=1
fi

# parse command line arguments
for a in ${@}
do
    case "$a" in
	--leak-check*)   USETCMALLOC=0;;
	--delete-check*) USETCMALLOC=0;;
	--stdcmalloc)    USETCMALLOC=0;;
	--tcmalloc)      USETCMALLOC=1;;
	--stdcmath)      USEIMF=0;;
	--imf)           USEIMF=1;;
	--preloadlib*)     export ATHENA_ADD_PRELOAD=${a#*=};;
	--drop-and-reload) export ATHENA_DROP_RELOAD=1;;
    esac
done

# Do the actual preloading via LD_PRELOAD
source `which athena_preload.sh `

# Now resurrect ourselves as python script
python_path=`which python3`
"exec" "$python_path" "-tt" "$0" "$@";

"""


# File: athena.py
# Author: Wim Lavrijsen (WLavrijsen@lbl.gov)
# "
# This script allows you to run Athena from python. It is developed against
# the cppyy based GaudiPython and python 2.6/7.x.
#
# Debugging is supported with the '-d' option (hook debugger after running
# all user scripts, and just before calling initialize) and the --debug
# option (requires "conf", "init", or "exec" and will hook just before that
# stage). The hook will give you the gdb prompt, from where you can set
# break points, load additional shared libraries, or drop into interactive
# athena mode (if -i specified on the cli). Alternatively, you can start
# with gdb, like so:
#
#  $ gdb python
#  (gdb) run `which athena.py` [options] [<file1>.py [<file2>.py ...
#
# Usage of valgrind is supported, but it requires full paths and explicit
# arguments in its run, like so:
#
#  $ valgrind `which python` `which athena.py` [options] [<file1>.py ...
#
# or, alternatively (valgrind 3.2.0 and up):
#
#  $ valgrind --trace-children=yes `which athena.py` [options] [<file1>.py ...
#
# Note that any error messages/leaks that valgrind reports on python can be
# ignored, as valgrind is wrong (see the file Misc/README.valgrind in the
# python installation).
#
# Additional details on debugging are available on the Wiki:
#
#   https://uimon.cern.ch/twiki/bin/view/Atlas/StartingDebuggerWithAthenaPy
#

__version__ = '3.3.0'
__author__  = 'Wim Lavrijsen (WLavrijsen@lbl.gov)'
__doc__     = 'For details about athena.py, run "less `which athena.py`"'

import sys, os
import getopt

ldpreload = os.getenv( 'LD_PRELOAD' ) or ''

### parse the command line arguments -----------------------------------------
import AthenaCommon.AthOptionsParser as aop
opts = aop.parse()
_help_and_exit = aop._help_and_exit

### remove preload hack for proper execution of child-processes --------------
if ldpreload:
   if 'TCMALLOCDIR' in os.environ:
       tcmlib = os.getenv( 'TCMALLOCDIR' ) +  "/libtcmalloc.so"
       ldpreload = ldpreload.replace(tcmlib, '' )
       tcmlib = os.getenv( 'TCMALLOCDIR' ) +  "/libtcmalloc_minimal.so"
       ldpreload = ldpreload.replace(tcmlib, '' )
       del tcmlib
   if os.getenv( 'ATHENA_ADD_PRELOAD' ):
      ldpreload = ldpreload.replace(os.getenv( 'ATHENA_ADD_PRELOAD' ), '' )
      os.unsetenv( 'ATHENA_ADD_PRELOAD' )
   ldpreload = ldpreload.replace( '::', ':')
   ldpreload = ldpreload.strip(':')

   if not ldpreload:
      del os.environ[ 'LD_PRELOAD' ]
   else:
      os.environ[ 'LD_PRELOAD' ] = ldpreload
del ldpreload

### start profiler, if requested
if opts.profile_python:
   import cProfile
 # profiler is created and controlled programmatically b/c a CLI profiling of
 # athena.py doesn't work (globals are lost from include() execfile() calls),
 # and because this allows easy excluding of the (all C++) Gaudi run
   cProfile._athena_python_profiler = cProfile.Profile()
   cProfile._athena_python_profiler.enable()

### debugging setup
from AthenaCommon.Debugging import DbgStage
DbgStage.value = opts.dbg_stage

### python interpreter configuration -----------------------------------------
if not os.getcwd() in sys.path:
   sys.path = [ os.getcwd() ] + sys.path

if not '' in sys.path:
   sys.path = [ '' ] + sys.path


## rename ourselfs to athena, both the prompt and the process (for top & ps)
sys.ps1 = 'athena> '
sys.ps2 = '.   ... '

try:
   import ctypes
   from ctypes.util import find_library as ctypes_find_library
   libc = ctypes.cdll.LoadLibrary( ctypes_find_library('c') )
   libc.prctl( 15, 'athena.py', 0, 0, 0 )
except Exception:
   pass            # don't worry about it failing ...

## user session history (deleted in Preparation.py)
fhistory = os.path.expanduser( '~/.athena.history' )


## interface setup as appropriate
if opts.run_batch and not opts.dbg_stage:
 # in batch there is no need for stdin
   if sys.stdin and os.isatty( sys.stdin.fileno() ):
      os.close( sys.stdin.fileno() )
else:
   # Make sure ROOT gets initialized early, so that it shuts down last.
   # Otherwise, ROOT can get shut down before Gaudi, leading to crashes
   # when Athena components dereference ROOT objects that have been deleted.
   import ROOT

 # readline support
   import rlcompleter, readline

   readline.parse_and_bind( 'tab: complete' )
   readline.parse_and_bind( 'set show-all-if-ambiguous On' )

 # history support
   if os.path.exists( fhistory ):
      readline.read_history_file( fhistory )
   readline.set_history_length( 1024 )

   del readline, rlcompleter

## use of shell escapes in interactive mode
if not opts.run_batch:
   import AthenaCommon.ShellEscapes as ShellEscapes
   sys.excepthook = ShellEscapes.ShellEscapes()
   del ShellEscapes


### logging and messages -----------------------------------------------------
from AthenaCommon.Logging import *
_msg = log # from above import...

## test and set log level
try:
   _msg.setLevel (getattr(logging, opts.msg_lvl))
except:
   _help_and_exit()


### default file name for ease of use ----------------------------------------
if not opts.scripts and os.path.exists(opts.default_jobopt):
   _msg.info("using default file %s", opts.default_jobopt)
   opts.scripts.append(opts.default_jobopt)

if not (opts.scripts or opts.fromdb) and opts.run_batch:
   _msg.error( "batch mode requires at least one script" )
   from AthenaCommon.ExitCodes import INCLUDE_ERROR
   _help_and_exit( INCLUDE_ERROR )
del _help_and_exit


### file inclusion and tracing -----------------------------------------------
from AthenaCommon.Include import IncludeError, include
include.setShowIncludes(opts.showincludes)
include.setClean(opts.drop_cfg)


### pre-execution step -------------------------------------------------------
include( "AthenaCommon/Preparation.py" )


### execution of user script and drop into batch or interactive mode ---------
include( "AthenaCommon/Execution.py" )
