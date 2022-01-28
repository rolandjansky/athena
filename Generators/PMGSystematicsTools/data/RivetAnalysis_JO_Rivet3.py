import os

theApp.EvtMax = -1

import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = ["!TESTSAMPLE!"]

systWeights =!SYSTWEIGHTS!

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from Rivet_i.Rivet_iConf import Rivet_i


from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from GaudiSvc.GaudiSvcConf import THistSvc

svcMgr += THistSvc()

def safeFileName(name):
 name=name.strip()
 name= name.replace(".","p").replace(" ","_")
 name= name.replace("pyoda",".yoda")
 name= name.replace(":","_")
 return name

if systWeights==None: systWeights={'Nominal': 0}

analyses= "!RIVETANALYSIS!"
rivet = Rivet_i()
for analysis in analyses.split(","):
  rivet.Analyses +=[analysis]
rivet.RunName = ""
rivet.DoRootHistos = False
rivet.AnalysisPath = os.environ['PWD']
rivet.CrossSection = 1.0
rivet.SkipWeights=False
job += rivet

