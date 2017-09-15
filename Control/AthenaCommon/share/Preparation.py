### preparation code before touching any user scripts ------------------------
import AthenaCommon.Include as AthCIncMod
if opts.showincludes:
   AthCIncMod.marker = ' -#-'      # distinguish bootstrap from other jo-code

if opts.trace_pattern:
   AthCIncMod.tracePattern = opts.trace_pattern


### debugging helper, hooks debugger to running interpreter process ----------
from AthenaCommon.Debugging import hookDebugger

### athena/gaudi -------------------------------------------------------------
from AthenaCommon.Configurable import *
from AthenaCommon.OldStyleConfig import Algorithm, AlgTool, Service

### some useful constants ----------------------------------------------------
from AthenaCommon.Constants import *
# namespace these constants: Lvl.INFO
# so we can write e.g: Units.GeV
import AthenaCommon.Constants     as Lvl
import AthenaCommon.SystemOfUnits as Units

### Athena configuration -----------------------------------------------------
if not opts.minimal:
   from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import theApp
from AthenaCommon.Logging import log

# load all entries so far into the workspace of include()

if 3 < len(opts.preconfig) and opts.preconfig[-3:] == ".py":   # user provided
   _msg.warning("Bootstrap and preconfig are no longer needed. "
                "Please update your scripts !")
   include(opts.preconfig)
else:
   if opts.preconfig != 'AthenaCommon':
      _msg.warning("Bootstrap and preconfig are no longer needed. "
                   "Please update your scripts !")
   include( "%s/Bootstrap.py" % opts.preconfig )
   if not opts.run_batch:                               # i.e. interactive
      theApp.EventLoop = "PyAthenaEventLoopMgr"         # from AthenaServices

## create the application manager and start in a non-initialised state
from AthenaCommon.AppMgr import ToolSvc, ServiceMgr, theAuditorSvc
exec 'theApp.setOutputLevel( %s )' % opts.msg_lvl
theApp._opts = opts                                     # FIXME

## further job messaging configuration
if not os.environ.has_key( "POOL_OUTMSG_LEVEL" ):
   exec 'os.environ[ "POOL_OUTMSG_LEVEL" ] = str(%s)' % opts.msg_lvl

## basic job configuration
include( "AthenaCommon/Atlas.UnixStandardJob.py" )
include.block( "AthenaCommon/Atlas.UnixStandardJob.py" )

## since the cppyy import leads to irreproducible growths in vmem,
## we optionally make sure this growth is bumped upwards to some common
## number (to not wash out other vmem changes):
def vmem_mb():
   for l in open( "/proc/self/status" ):
      if l.startswith( "VmSize:" ):
         return float(l.split()[1])/1024.

def grow_vmem(targetvmem_mb):
   v = vmem_mb()
   global l_extra_vmem_holder
   l_extra_vmem_holder = []
   while vmem_mb() < targetvmem_mb:
      l_extra_vmem_holder += [ " "*1024 ]
   v = vmem_mb() - v
   log.info( "Acquired %f mb of extra vmem",  v )

if not opts.cppyy_minvmem is None:
   min_cppyy_vmem_growth = opts.cppyy_minvmem
   log.debug( "Using cppyy minimum growth from CLI with value %.2f", min_cppyy_vmem_growth )
   vmem_before_cppyy = vmem_mb()
elif os.getenv( "ATHENA_PYCINTEX_MINVMEM" ):
   min_cppyy_vmem_growth = float(os.getenv( "ATHENA_PYCINTEX_MINVMEM" ))
   log.debug( "Using ATHENA_PYCINTEX_MINVMEM with value %.2f", min_cppyy_vmem_growth )
   vmem_before_cppyy = vmem_mb()
else:
   min_cppyy_vmem_growth = None

from PyUtils.Helpers import ROOT6Setup
ROOT6Setup()

if min_cppyy_vmem_growth:
   grow_vmem( vmem_before_cppyy + min_cppyy_vmem_growth )
   del vmem_before_cppyy
del min_cppyy_vmem_growth, grow_vmem, vmem_mb

from cppyy import gbl
## backward compat...
class _AthenaServicesDictBwd:
  @staticmethod
  def SetFatalHandler(flags=-1):
     import AthenaCommon.Logging as L
     info = L.log.info
     info("'gbl.AthenaServices.SetFatalHandler(flag)' is DEPRECATED")
     info("instead, please consider using:")
     info(" svcMgr.CoreDumpSvc.FatalHandler = flag")

     from AthenaCommon.AppMgr import ServiceMgr as svcMgr
     svcMgr.CoreDumpSvc.FatalHandler = flags
setattr(gbl, 'AthenaServices', _AthenaServicesDictBwd)
del _AthenaServicesDictBwd
## end --
if not (not opts.run_batch and theApp.EventLoop == "PyAthenaEventLoopMgr"):
   # make SIG_INT fatal
   svcMgr.CoreDumpSvc.FatalHandler = -1
   pass

## now import the top-level module which eases interactive work and/or
## python-based components: a pure python module
## we have to put it there as some jobOptions might steal the event loop...
## ie: do theApp.initialize();theApp.nextEvent(...) directly in the jobO.

# this code is to be removed; for now flag it with 'minimal' to test the effects
# of its removal
if not opts.minimal:
   from AthenaPython import PyAthena
# -- end of minimal

## user level configuration
try:
   include( "$HOME/.athenarc" )
except IncludeError:
   pass

### max out resource limits --------------------------------------------------
from AthenaCommon.ResourceLimits import SetMaxLimits
SetMaxLimits()
del SetMaxLimits
del sys.modules[ 'AthenaCommon.ResourceLimits' ]

### prettification for interactive use
if not opts.run_batch:
   import atexit, readline

 # finalize on exit (^D)
   atexit.register( theApp.exit )

 # history support
   atexit.register( readline.write_history_file, fhistory )
   del readline, atexit

del fhistory

### pre-import some general job-flags so they can be used in CLI

# this code is to be removed; for now flag it with 'minimal' to test the effects
# of its removal
if not opts.minimal:
   from AthenaCommon.AthenaCommonFlags import jobproperties as jp
   from AthenaCommon.GlobalFlags import jobproperties as jp
   from AthenaCommon.BeamFlags import jobproperties as jp
   try:
      from RecExConfig.RecFlags import rec
   except ImportError:
    # in case we don't have enough project-karma
      pass
# -- end of minimal

if opts.command:
   _msg.info( 'executing CLI (-c) command: "%s"' % opts.command )
   exec opts.command

if DbgStage.value == "conf":
   hookDebugger()

if opts.showincludes:
   AthCIncMod.marker = AthCIncMod.__marker__      # reset
del AthCIncMod

if opts.drop_cfg:
   import AthenaCommon.ConfigurationCleanup
   AthenaCommon.ConfigurationCleanup.doCleanse = True

if opts.do_leak_chk != False:
   from Hephaestus.Auditor import HephaestusAuditor
   theApp.AuditAlgorithms = True
   svcMgr.AuditorSvc += HephaestusAuditor(
      mode = opts.memchk_mode, auditOn = opts.do_leak_chk )

## basic job configuration for Hive and AthenaMP
if not opts.minimal:
   from AthenaCommon.ConcurrencyFlags import jobproperties as jps
   if opts.nprocs and (opts.nprocs >= 1 or opts.nprocs==-1):
      jps.ConcurrencyFlags.NumProcs = opts.nprocs
      _msg.info ("configuring AthenaMP with [%s] sub-workers", 
                 jps.ConcurrencyFlags.NumProcs())

      if (opts.debug_worker == True) :
         jps.ConcurrencyFlags.DebugWorkers = True
         _msg.info ("   Workers will pause after fork until SIGUSR1 signal received")

   if ( (opts.threads and (opts.threads != 0)) or (opts.concurrent_events and (opts.concurrent_events != 0)) ):

      if (opts.threads == 0 and opts.concurrent_events > 0) :
         ## num threads = num concurrent evts
         jps.ConcurrencyFlags.NumThreads = opts.concurrent_events
         jps.ConcurrencyFlags.NumConcurrentEvents = opts.concurrent_events
      elif ( opts.threads > 0 and opts.concurrent_events == 0) :
         ## num concurrent evts = num threads
         jps.ConcurrencyFlags.NumThreads = opts.threads
         jps.ConcurrencyFlags.NumConcurrentEvents = opts.threads
      else :
         ## both concurrent evts and threads set individually
         jps.ConcurrencyFlags.NumThreads = opts.threads
         jps.ConcurrencyFlags.NumConcurrentEvents = opts.concurrent_events
         

      if (jps.ConcurrencyFlags.NumProcs() > 0) :
         _msg.info ("configuring hybrid AthenaMP/AthenaMT with [%s] concurrent threads and [%s] concurrent events per AthenaMP worker", jps.ConcurrencyFlags.NumThreads, jps.ConcurrencyFlags.NumConcurrentEvents)
      elif (jps.ConcurrencyFlags.NumProcs() == 0) :
         _msg.info ("configuring AthenaHive with [%s] concurrent threads and [%s] concurrent events", jps.ConcurrencyFlags.NumThreads(), jps.ConcurrencyFlags.NumConcurrentEvents())
      else:
         # we should never get here
         _msg.error ("ConcurrencyFlags.NumProcs() cannot == -1 !!")
         sys.exit()

      import AthenaCommon.AtlasThreadedJob



