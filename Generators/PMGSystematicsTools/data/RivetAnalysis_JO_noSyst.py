import os

theApp.EvtMax = -1

import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = ["!TESTSAMPLE!"]
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from Rivet_i.Rivet_iConf import Rivet_i
rivet = Rivet_i()
rivet.AnalysisPath = os.environ['PWD']
analyses= "!RIVETANALYSIS!"
for analysis in analyses.split(","):
  rivet.Analyses +=[analysis]
rivet.RunName = ""
rivet.HistoFile = "Nominal"
rivet.CrossSection = 1.0
job += rivet


