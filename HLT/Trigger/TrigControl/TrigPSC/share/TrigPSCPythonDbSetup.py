###############################################################
## @file   TrigPSCPythonDbSetup.py
## @brief  Minimal Python setup for running from TrigDB
## @author Frank Winklmeier
## $Id: TrigPSCPythonDbSetup.py 11 2013-05-14 15:22:39Z ricab $
###############################################################

## This is a very minimal Python setup. It is only included when
## the POSTCOMMAND is non-empty while running from the DB.
## Besides providing basic python bindings it also takes care of
## switching the OutputLevel in case the "-l" option was used.

## !!! Do NOT import theApp. It will screw up the configuration !!!

import string, os, sys


def setTHistSvcOutput():
   """Helper to set THistSvc.Output"""

   ## Do this only in a function to not pollute the global namespace
   from TriggerJobOpts.HLTTriggerGetter import setTHistSvcOutput
   output = []
   setTHistSvcOutput(output)
   setattr(iProperty("THistSvc"),"Output",output)
   return

   
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

if not os.environ.has_key( "POOL_OUTMSG_LEVEL" ):
   exec 'os.environ[ "POOL_OUTMSG_LEVEL" ] = str(%s)' % logLevel[1]

## Minimal Python bindings
from GaudiPython import *
from GaudiPython.Bindings import iProperty
from TrigCommon.TrigPyHelper import trigApp

## Set OutputLevel in JobOptionsSvc if "-l" option was used in athenaMT/PT
if logLevel[0]!="INFO":
   outputLevel = int(locals()[logLevel[0]])
   outputLevelProp = gbl.IntegerProperty("OutputLevel", outputLevel)
   
   ## Reset message levels
   trigApp.service("MessageSvc", gbl.ITrigMessageSvc).resetOutputLevels()
   trigApp.service("MessageSvc", gbl.IMessageSvc).setOutputLevel(outputLevel)

   jobOptSvc = trigApp.service("JobOptionsSvc", gbl.IJobOptionsSvc)
   ## Set OutputLevel in JobOptionsSvc
   for client in jobOptSvc.getClients():
      for prop in jobOptSvc.getProperties(client):
         if prop.name()=="OutputLevel":
            iProperty(client).OutputLevel = outputLevel

## For running with offline THistSvc from online DB
from TrigServices.TriggerUnixStandardSetup import _Conf
if not _Conf.useOnlineTHistSvc:
   isvcMgr = InterfaceCast(gbl.ISvcManager)(gbl.Gaudi.svcLocator())
   ## Change service type from TrigMonTHistSvc to THistSvc
   isvcMgr.declareSvcType("THistSvc","THistSvc")
   setTHistSvcOutput()

if 'interactive' in dir():   
   PscConfig.interactive = bool(interactive)
   del interactive
   
## Some cleanup
del _Conf
del logLevel
