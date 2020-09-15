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
for i in systWeights:
    systName=safeFileName(i)

    print 'weight name:',i,', output name',systName
 
    rivet = Rivet_i(systName)
    for analysis in analyses.split(","):
      rivet.Analyses +=[analysis]
    
    rivet.RunName = ""
    #rivet.HistoFile = "myanalysis"
    if i!="Nominal" : rivet.WeightName=i
    rivet.HistoFile = systName
    rivet.DoRootHistos = False
    #rivet.IgnoreBeamCheck = True
    rivet.AnalysisPath = os.environ['PWD']
    rivet.CrossSection = 1.0

    job += rivet

