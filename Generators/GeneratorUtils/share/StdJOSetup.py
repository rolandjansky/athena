## Generic JO header for use in any evgen-related job options
## Included by StdEvgenSetup.py and StdAnalysisSetup.py

## Set up standard logging
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.MessageSvc.OutputLevel = INFO
if "LOGLEVEL" in dir():
     svcMgr.MessageSvc.OutputLevel = LOGLEVEL

## Import the logging system
from AthenaCommon.Logging import logging

## Number of events to be processed
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 100
if "EVTMAX" in dir():
     theApp.EvtMax = EVTMAX

## Set up the particle property service
include("PartPropSvc/PartPropSvc.py")

## Set up the random number service
from RngComps.RngCompsConf import AtRndmGenSvc
svcMgr += AtRndmGenSvc()

## Get a handle on the top level algorithm sequence
## Several common aliases are provided, but "topAlg" is preferred
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSeq = topSequence
topAlg = topSequence
job = topSequence

## Import the service manager
from AthenaCommon.AppMgr import ServiceMgr
svcMgr = ServiceMgr

## Provide convenience handles for various services
msgSvc = svcMgr.MessageSvc
sgSvc = svcMgr.StoreGateSvc
rngSvc = svcMgr.AtRndmGenSvc
ppSvc = svcMgr.PartPropSvc

## Pull in general Athena job property flags
from AthenaCommon.AthenaCommonFlags import jobproperties
