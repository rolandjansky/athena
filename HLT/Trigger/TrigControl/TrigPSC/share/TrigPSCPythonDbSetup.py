# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

###############################################################
## @file   TrigPSCPythonDbSetup.py
## @brief  Minimal Python setup for running from TrigDB
## @author Frank Winklmeier
###############################################################

## This is a very minimal Python setup. It is only used when running
## with athenaHLT from the DB. It is not used in a partition!
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

from TrigPSC import PscConfig
logLevel=PscConfig.optmap['LOGLEVEL'].split(',')[0]

## Minimal Python bindings
from GaudiPython import *
from GaudiPython.Bindings import iProperty
from TrigCommon.TrigPyHelper import trigApp

## If HLT PSK is set on command line read it from DB (and not COOL)
if 'hltkey' in PscConfig.optmap['JOBOPTIONSPATH']:
   trigApp.changeJobProperties('HLTPrescaleCondAlg', 'Source', 'DB')

## Set OutputLevel in JobOptionsSvc if "-l" option was used in athenaHLT
if logLevel!="INFO":
   outputLevel = int(locals()[logLevel])

   trigApp.service("MessageSvc", gbl.IMessageSvc).setOutputLevel(outputLevel)
   trigApp.changeJobProperties('.*', 'OutputLevel', str(locals()[logLevel]))

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
