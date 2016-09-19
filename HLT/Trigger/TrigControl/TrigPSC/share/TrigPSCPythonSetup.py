## @file   TrigPSCPythonSetup.py
## @brief  Python setup for online running (similar to athena.py)
## @author Werner Wiedenmann <Werner.Wiedenmann@cern.ch>
###############################################################
 
### basic setup files and definitions ----------------------------------------
pscBootstrapFile     = "TrigServices/TrigServicesBootstrap.py"   # PSC bootstrap
pscServiceSetupBegin = "TrigServices/TrigServicesCommonBegin.py" # Service definitions
pscServiceSetupEnd   = "TrigServices/TrigServicesCommonEnd.py"   # Service definitions

import sys, os, string

### Set up some common flags --------------------------------------------------

### athenaCommonFlags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.isOnline.set_Value_and_Lock(True)
del athenaCommonFlags

### TriggerFlags
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.doHLT.set_Value_and_Lock(True)

### By default do not run in validation mode (do not lock this one!)
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.Online.doValidation = False
    
del TriggerFlags

### logging and messages -----------------------------------------------------
from AthenaCommon.Logging import *
from AthenaCommon.Constants import *
import AthenaCommon.ExitCodes as ExitCodes

from TrigPSC import PscConfig
logLevel=string.upper(PscConfig.optmap['LOGLEVEL'])
logLevel=string.split(logLevel,",")
if len(logLevel)==0:
   logLevel=["INFO","ERROR"]

if len(logLevel)==1:
   logLevel.append("ERROR")

## test and set log level
try:
   exec( 'log.setLevel( logging.%s )' % logLevel[0] )
except:
   sys.exit( ExitCodes.OPTIONS_UNKNOWN )

### file inclusion and tracing -----------------------------------------------
from AthenaCommon.Include import Include, IncludeError, include
showincludes = eval(PscConfig.optmap['SHOWINCLUDE'])
include.setShowIncludes( showincludes )
if showincludes:
   import AthenaCommon.Include as AthCIncMod
   AthCIncMod.marker = ' -#-'      # distinguish bootstrap from other jo-code

if PscConfig.optmap['TRACEPATTERN']:
   import AthenaCommon.Include
   AthenaCommon.Include.tracePattern = PscConfig.optmap['TRACEPATTERN']

#
### gaudi --------------------------------------------------------------------
try:
   from GaudiPython import *
except ImportError:
   from gaudimodule import *

from AthenaCommon.Configurable import *
from AthenaCommon.OldStyleConfig import *

### set output level  --------------------------------------------------------
exec 'OUTPUTLEVEL = %s' % logLevel[0]
if not os.environ.has_key( "POOL_OUTMSG_LEVEL" ):
   exec 'os.environ[ "POOL_OUTMSG_LEVEL" ] = str(%s)' % logLevel[1]
del logLevel

OutputLevel=OUTPUTLEVEL
HLTOutputLevel=OUTPUTLEVEL
         
### Athena configuration -----------------------------------------------------
from AthenaCommon import CfgMgr

## create the application manager and start in a non-initialised state
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ToolSvc, ServiceMgr, theAuditorSvc

## These properties have alread been set on the C++ ApplicationMgr in the Psc
## but the configurable needs to have them set as well
theApp.MessageSvcType    = PscConfig.optmap["MESSAGESVCTYPE"]
theApp.JobOptionsSvcType = PscConfig.optmap["JOBOPTIONSSVCTYPE"]
         
## add the MessageSvc and the JobOptionsSvc to the ServiceMgr
from AthenaCommon.ConfigurableDb import getConfigurable
ServiceMgr += getConfigurable(theApp.JobOptionsSvcType)("JobOptionsSvc")
ServiceMgr += getConfigurable(theApp.MessageSvcType   )("MessageSvc"   )

## set the default values
try:
   include( pscBootstrapFile )
except Exception, e:
   if isinstance( e, IncludeError ):
      print sys.exc_type, e
      theApp._exitstate = ExitCodes.INCLUDE_ERROR
      sys.exit( ExitCodes.INCLUDE_ERROR )
   raise

## properties of the application manager
theApp.StatusCodeCheck = False     # enabled via TriggerFlags.Online.doValidation (see below)
theApp.setOutputLevel(OUTPUTLEVEL)

# Configure the CoreDumpSvc
if not hasattr(ServiceMgr,"CoreDumpSvc"):
   from AthenaServices.Configurables import CoreDumpSvc
   ServiceMgr += CoreDumpSvc()

# check if the CoreDumpSvc can provide the FatalHandler
# in case this is not available try for backward compatibility
# to load the old libAthenaServicesDict and try to install it from there
#
if ServiceMgr.CoreDumpSvc.properties().has_key('FatalHandler'): 
   ServiceMgr.CoreDumpSvc.FatalHandler = -1         # make SIG_INT fatal

## set resource limits
from AthenaCommon.ResourceLimits import SetMaxLimits
SetMaxLimits()
del SetMaxLimits

## reset include markers
if showincludes:
   AthCIncMod.marker = AthCIncMod.__marker__      # reset
   del AthCIncMod
del showincludes

### run optional command before user job options script ----------------------
if PscConfig.optmap['PRECOMMAND']:
   print "\n"
   print " +------------------------------------------------+ "
   print " | Execute command before jobOptions script START.| "
   print " +------------------------------------------------+ "
   print " ---> Command = ", PscConfig.optmap['PRECOMMAND']
   try:
      exec PscConfig.optmap['PRECOMMAND']
   except Exception, e:
      if isinstance( e, IncludeError ):
         print sys.exc_type, e
         theApp._exitstate = ExitCodes.INCLUDE_ERROR
         sys.exit( theApp._exitstate )         
      elif isinstance( e, ImportError ):
         print sys.exc_type, e
         theApp._exitstate = ExitCodes.IMPORT_ERROR
         sys.exit( theApp._exitstate )
      raise
   print " +------------------------------------------------+ "
   print " | Execute command before jobOptions script END.  | "
   print " +------------------------------------------------+ "
   print "\n"

### basic job configuration before user configuration ------------------------
try:
   include( pscServiceSetupBegin )
   include.block( pscServiceSetupBegin )
except Exception, e:
   print sys.exc_type, e
   if isinstance( e, IncludeError ):
      theApp._exitstate = ExitCodes.INCLUDE_ERROR
      sys.exit( theApp._exitstate )      
   raise

if 'interactive' in dir():   
   import TrigPSC.PscConfig 
   TrigPSC.PscConfig.interactive = bool(interactive)
   del interactive
   
### run user jobOptions file -------------------------------------------------
try:
   include( "%s" % PscConfig.optmap['JOBOPTIONSPATH'] )
except Exception, e:
   if isinstance(e,SystemExit):
      raise

   # "sanitize" the traceback under the assumption that it is the user's fault
   import traceback
   exc_info = sys.exc_info()
   tb = traceback.extract_tb( exc_info[2] )

   short_tb = []
   for frame_info in tb:
      if not 'AthenaCommon' in frame_info[0]:
         short_tb.append( frame_info )

   print 'Shortened traceback (most recent user call last):'
   print ''.join( traceback.format_list( short_tb ) ),
   print ''.join( traceback.format_exception_only( exc_info[0], exc_info[1] ) ),

   # additional processing to get right error codes
   import AthenaCommon.ExitCodes as ExitCodes
   if isinstance( e, IncludeError ):
      theApp._exitstate = ExitCodes.INCLUDE_ERROR
      sys.exit( theApp._exitstate )
   elif isinstance( e, ImportError ):
      theApp._exitstate = ExitCodes.IMPORT_ERROR
      sys.exit( theApp._exitstate )

   # Throw exception to stop application
   theApp._exitstate = ExitCodes.UNKNOWN_EXCEPTION
   raise

### basic job configuration after user configuration -------------------------
try:
   include( pscServiceSetupEnd )
   include.block( pscServiceSetupEnd )
except Exception, e:
   if isinstance( e, IncludeError ):
      print sys.exc_type, e
      theApp._exitstate = ExitCodes.INCLUDE_ERROR
      sys.exit( ExitCodes.INCLUDE_ERROR )
   raise

### run optional command after user job options script -----------------------   
if PscConfig.optmap['POSTCOMMAND']:
   print "\n"
   print " +------------------------------------------------+ "
   print " | Execute command after jobOptions script START. | "
   print " +------------------------------------------------+ "
   print " ---> Command = ", PscConfig.optmap['POSTCOMMAND']
   try:
      exec PscConfig.optmap['POSTCOMMAND']
   except Exception, e:
      if isinstance( e, IncludeError ):
         print sys.exc_type, e
         theApp._exitstate = ExitCodes.INCLUDE_ERROR
         sys.exit( ExitCodes.INCLUDE_ERROR )
      elif isinstance( e, ImportError ):
         print sys.exc_type, e
         theApp._exitstate = ExitCodes.IMPORT_ERROR
         sys.exit( ExitCodes.IMPORT_ERROR )
      raise
   print " +------------------------------------------------+ "
   print " | Execute command after jobOptions script END.   | "
   print " +------------------------------------------------+ "
   print "\n"

### final tweaks -------------------------------------------------------------

from TriggerJobOpts.TriggerFlags import TriggerFlags
if TriggerFlags.Online.doValidation():
   theApp.StatusCodeCheck = True

del TriggerFlags

### setup everything ---------------------------------------------------------
theApp.setup()
