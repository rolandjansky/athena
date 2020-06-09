# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"exec" "`which python`" "-tt" "$0" "$@"

# File: pyroot.py
# Author: Sebastien Binet (binet@cern.ch)

# This script allows you to run ROOT from python.
# It has been heavily based (err... stolen) on athena.py from Wim

from __future__ import print_function

__author__  = 'Sebastien Binet (binet@cern.ch)'
__doc__     = 'For details about pyroot.py, run "less `which pyroot.py`"'

import sys, os
import getopt, string


### recognized user options __________________________________________________
_useropts = "bidc:hv"
_userlongopts = [ "batch", "interactive", "no-display", "debug=", "command=",
                  "help",  "version",]


### explanation of the options -----------------------------------------------
def _help_and_exit( reason = None ):
   print ("""Accepted command line options:
 -b, --batch                          ...  batch mode
 -i, --interactive                    ...  interactive mode [default]
     --no-display                           prompt, but no graphics display
 -c, --command                        ...  one-liner, runs before any scripts
 -h, --help                           ...  print this help message
 -,-- [arg1,...]                      ...  additional arguments passed directly
                                           to user scripts (left untouched)
 [<file1>.py [<file2>.py [...]]]      ...  scripts to run""")

   sys.exit( 1 )


### option handling  ---------------------------------------------------------
scripts,opts = [],[]
runBatch = 0                     # batch mode is NOT the default
display = None                   # only useful in interactive mode: no display
command = ""                     # optional one-line command
userOpts = []                    # left-over opts after '-'


## emulated gnu getopt for p2.2: collect scripts and options
for arg in sys.argv[1:]:
   if arg[-3:] == ".py":
      scripts.append( arg )
   elif arg in ('-','--'):     # rest are user opts, save and done
      userOpts += sys.argv[ sys.argv.index( arg )+1: ]
      break
   else:
      opts.append( arg )

## process user options
try:
   optlist, args = getopt.getopt( opts, _useropts, _userlongopts )
except getopt.error:
   print (sys.exc_value)
   _help_and_exit()

if args:
   print ("Unhandled arguments:", args)
   _help_and_exit()

for opt, arg in optlist:
   if opt in ("-b", "--batch"):
      runBatch = 1
   elif opt in ("-i", "--interactive"):
      runBatch = 0
      defOptions = ""
      if display is None: display = 1
   elif opt in ("--no-display",):
      display = 0
   elif opt in ("-c", "--command"):
      command = string.strip( arg )
   elif opt in ("-h", "--help"):
      _help_and_exit()

if optlist: del opt, arg
del args, optlist, opts
del _useropts, _userlongopts, string, getopt

## for the benefit of PyROOT
if not display and '-b' not in sys.argv:
   sys.argv = sys.argv[:1] + ['-b'] + sys.argv[1:]
del display


### python interpreter configuration -----------------------------------------
if not os.getcwd() in sys.path:
   sys.path = [ os.getcwd() ] + sys.path

if '' not in sys.path:
   sys.path = [ '' ] + sys.path

sys.ps1 = 'pyroot> '
fhistory = os.path.expanduser( '~/.pyroot.history' )

## interface setup as appropriate
if runBatch:
 # in batch there is no need for stdin
   if os.isatty( sys.stdin.fileno() ):
      os.close( sys.stdin.fileno() )
   if 'PYTHONINSPECT' in os.environ:
      os.unsetenv('PYTHONINSPECT')
else:
   os.environ['PYTHONINSPECT'] = '1'
 # readline support
   import rlcompleter, readline  # noqa: F401

   readline.parse_and_bind( 'tab: complete' )
   readline.parse_and_bind( 'set show-all-if-ambiguous On' )

 # history support
   if os.path.exists( fhistory ):
      readline.read_history_file( fhistory )
   readline.set_history_length( 1024 )

   del readline, rlcompleter

### ROOT loading ------------------------------------------------------------
print (sys.ps1+"loading ROOT...")
import ROOT
print (sys.ps1+"loading ROOT... [ok]")
ROOT.gROOT.SetStyle("Plain")
ROOT.gStyle.SetPalette(1)      # less ugly palette colors
ROOT.gStyle.SetOptStat(111111) #
print (sys.ps1+"loaded pyroot style... [ok]")

### execution ----------------------------------------------------------------
if not runBatch:
   import atexit, readline

 # history support
   atexit.register( readline.write_history_file, fhistory )
   del readline, atexit

del fhistory

if command:
   print (sys.ps1+'executing CLI (-c) command: "%s"' % command)
   exec (command)
del command

from past.builtins import execfile
for script in scripts:
   try:
      execfile( script )
   except Exception as e:
      if isinstance(e,SystemExit):
         raise

      import traceback
      traceback.print_exc()

      if runBatch:
          import sys
          sys.exit(2)

    # for interactive: drop into prompt
      break

else:
 ## only get here if all scripts successfully included
   del scripts

 ## in batch, then we are done: exit nicely
   if runBatch:
       import sys
       sys.exit(0)
   else:
       # done, back to the user
       print (sys.ps1+"entering interactive session...")
       pass
