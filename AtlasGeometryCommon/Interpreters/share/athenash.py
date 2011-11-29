#!/bin/sh


"""date"      # executable for the shell, but a string block to python 
exec `which python` -t "$0" "$@"               # exec, as required for gdb 
"""           # python execution starts here, the shell never reaches this


__version__ = '3.0.0'
__author__  = 'Joe Boudreau (boudreau@pitt.edu) based upon work of Wim Lavrijsen (WLavrijsen@lbl.gov)'
__doc__     = 'No Doc.  Sorry'

import sys, os
import getopt, string


runBatch=1

# start-up athfile server early on to prevent race-conditions later on...
try:
   import PyUtils.AthFile as af
   af.msg.debug('starting athfile server...')
except ImportError:
   pass
   
### python interpreter configuration -----------------------------------------
if not os.getcwd() in sys.path:
   sys.path = [ os.getcwd() ] + sys.path

if not '' in sys.path:
   sys.path = [ '' ] + sys.path


### logging and messages -----------------------------------------------------
from AthenaCommon.Logging import *

### file inclusion and tracing -----------------------------------------------
from AthenaCommon.Include import Include, IncludeError, include
include.setShowIncludes( False )

### gaudi --------------------------------------------------------------------
from GaudiPython import *

from AthenaCommon.Configurable import *
from AthenaCommon.OldStyleConfig import *
from AthenaCommon.Constants import *

preconfig = "AthenaCommon"
### Athena configuration -----------------------------------------------------
theApp = iService( "ApplicationMgr" )       # proxy
theApp.StatusCodeCheck = True
theApp.Dlls += [ 'AthenaServices' ]
include( "%s/Bootstrap.py" % preconfig )

## create the application manager and start in a non-initialised state
from AthenaCommon.AppMgr import theApp, ToolSvc, ServiceMgr, theAuditorSvc


## basic job configuration
include( "AthenaCommon/Atlas.UnixStandardJob.py" )
include.block( "AthenaCommon/Atlas.UnixStandardJob.py" )


EventLoopMgr = iService( 'EventLoopMgr' )
AthenaEventLoopMgr = EventLoopMgr                       # for historic reasons

### execution ----------------------------------------------------------------
from AthenaCommon.ResourceLimits import SetMaxLimits
SetMaxLimits()
del SetMaxLimits

include( sys.argv[1] )
try:
   theApp.run()     # runs until theApp.EvtMax events reached
except RuntimeError, e:
   print 'RuntimeError:', str(e)
theApp.exit()    # exits program, yields theApp._exitstate on shell    

