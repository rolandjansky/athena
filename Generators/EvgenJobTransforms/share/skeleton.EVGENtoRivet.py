
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Functionality core of the Rivet_tf transform"""

##==============================================================
## Basic configuration
##==============================================================

## Create sequence for Rivet
import os, re, string
import AthenaCommon.AlgSequence as acas
import AthenaCommon.AppMgr as acam
from AthenaCommon.AthenaCommonFlags import jobproperties
theApp = acam.theApp
topSeq = acas.AlgSequence()
anaSeq = topSeq


##==============================================================
## Configure standard Athena services
##==============================================================

## Special setup for event generation
include("AthenaCommon/Atlas.UnixStandardJob.py")

## Run performance monitoring (memory logging)
from PerfMonComps.PerfMonFlags import jobproperties as perfmonjp
perfmonjp.PerfMonFlags.doMonitoring = True
perfmonjp.PerfMonFlags.doSemiDetailedMonitoring = True

## Jobs should stop if an include fails.
jobproperties.AthenaCommonFlags.AllowIgnoreConfigError = False

## Set up a standard logger
from AthenaCommon.Logging import logging
evgenLog = logging.getLogger('Generate')

## Announce arg checking
evgenLog.debug("****************** CHECKING Rivet ARGS *****************")
evgenLog.debug(str(runArgs))

if not hasattr(runArgs, "rivetAnas"):
  raise RuntimeError("No Rivet analysis provided")

## Announce start of Rivet setup
evgenLog.debug("****************** CONFIGURING Rivet *****************")

import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.FirstEvent = runArgs.firstEvent
theApp.EvtMax = -1
if hasattr(runArgs, "maxEvents"):
  theApp.EvtMax = runArgs.maxEvents

## Add Rivet_i to the job
from Rivet_i.Rivet_iConf import Rivet_i
anaSeq += Rivet_i()
anaSeq.Rivet_i.Analyses = runArgs.rivetAnas
anaSeq.Rivet_i.DoRootHistos = True

if not hasattr(runArgs, "inputEVNTFile") :
  raise RuntimeError("Must specify input EVNT file using inputEVNTFile")
else :
  svcMgr.EventSelector.InputCollections = runArgs.inputEVNTFile

from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()

rootFile="Rivet.root"

if hasattr(runArgs, "outputYODAFile"):
  anaSeq.Rivet_i.HistoFile = runArgs.outputYODAFile
  rootFile = runArgs.outputYODAFile.replace(".yoda", ".root")

svcMgr.THistSvc.Output += ["Rivet DATAFILE='" + rootFile + "' OPT='RECREATE'"]


##==============================================================
## Show the algorithm sequences and algs now that config is complete
##==============================================================
acas.dumpMasterSequence()

##==============================================================
## Dump evgenConfig so it can be recycled in post-run actions
##==============================================================

from PyJobTransformsCore.runargs import RunArguments
runPars = RunArguments()
with open("config.pickle", 'w') as f:
    import cPickle
    cPickle.dump(runPars, f)


##==============================================================
## Get ready to run...
##==============================================================
evgenLog.debug("****************** STARTING Rivet *****************")
