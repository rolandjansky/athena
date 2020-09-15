import os

theApp.EvtMax = -1

import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = ["!TESTSAMPLE!"]
from PyUtils import AthFile
af = AthFile.fopen(svcMgr.EventSelector.InputCollections[0]) #opens the first file from the InputCollections list
af.fileinfos #this is a dict of dicts, take a look at what's available (e.g. do af.fileinfos.keys() to see the main keys)!
#Below are some example
systWeights=None


def safeFileName(name):
 name=name.strip()
 name= name.replace(".","p").replace(" ","_")
 name= name.replace("pyoda",".yoda")
 name= name.replace(":","_")
 return name


metadata = af.fileinfos['metadata']
if '/Generation/Parameters' in metadata:
    genpars=metadata['/Generation/Parameters']
    if 'HepMCWeightNames' in genpars:
        systWeights=genpars['HepMCWeightNames']
    else:
        print 'HepMCWeightName not found in /Generation/Parameters:'
        print genpars
        #raise RuntimeError('HepMCWeightName not found in /Generation/Parameters. Exiting...')
else:
    print '/Generation/Parameters not found in metadata:'
    print metadata
    #raise RuntimeError('/Generation/Parameters not found in metadata. Exiting...')


from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from Rivet_i.Rivet_iConf import Rivet_i


from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from GaudiSvc.GaudiSvcConf import THistSvc

svcMgr += THistSvc()
analyses= "!RIVETANALYSIS!"
if systWeights==None: systWeights={'Nominal': 0}
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

