###############################################################
## @file   TrigPSCPythonDbSetup.py
## @brief  Minimal Python setup for running from TrigDB
## @author Frank Winklmeier
###############################################################

## This is a very minimal Python setup. It is only included when
## the POSTCOMMAND is non-empty while running from the DB.
## Besides providing basic python bindings it also takes care of
## switching the OutputLevel in case the "-l" option was used.

## !!! Do NOT import theApp. It will screw up the configuration !!!

import builtins
printfunc = getattr(builtins,'print')

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
logLevel=PscConfig.optmap['LOGLEVEL'].split(',')[0]

## Minimal Python bindings
from GaudiPython import *
from GaudiPython.Bindings import iProperty
from TrigCommon.TrigPyHelper import trigApp

## Set OutputLevel in JobOptionsSvc if "-l" option was used in athenaMT/PT
if logLevel!="INFO":
   outputLevel = int(locals()[logLevel])

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

## Some cleanup
del _Conf
del logLevel
