# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# This file is not a full JO. It is utilised in all ExampleRates JobOptions.
#---- Additional Setup -------

theApp.EvtMax=EvtMax #says how many events to run over. Set to -1 for all events

# Create output ROOT file for histograms
if DoHistograms:
  if not hasattr(svcMgr, 'THistSvc'): svcMgr += CfgMgr.THistSvc()
  svcMgr.THistSvc.Output += ["RATESTREAM DATAFILE='" + OutputHistogramFile + "' OPT='RECREATE'"] #add an output root file stream

# Setup for accessing bunchgroup data from the DB
if DoRatesVsPositionInTrain:
  from AthenaCommon.GlobalFlags import globalflags
  globalflags.DataSource = 'geant4' if isMC else 'data' 
  globalflags.DatabaseInstance = 'CONDBR2' 
  from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
  if isMC: ToolSvc += BunchCrossingTool( "MC" )
  else: ToolSvc += BunchCrossingTool( "LHC" )

# Reduce logging - but don't suppress messages
include("AthAnalysisBaseComps/SuppressLogging.py")
MessageSvc.defaultLimit = 9999999
MessageSvc.useColors = True
MessageSvc.Format = "% F%35W%S%7W%R%T %0W%M"

# Execution statistics
from AthenaCommon.AppMgr import theAuditorSvc
from GaudiAud.GaudiAudConf import ChronoAuditor
theAuditorSvc += ChronoAuditor()
theApp.AuditAlgorithms = True  
