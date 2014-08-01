from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel = INFO

MASS=1000
CASE='gluino'

# Number of events to be processed (default is 10)
MyNevts = 1000
theApp.EvtMax = MyNevts

import os
import sys

os.system('get_files -data PDGTABLE_'+CASE+'_'+str(MASS)+'GeV.MeV')
os.system('mv PDGTABLE_'+CASE+'_'+str(MASS)+'GeV.MeV PDGTABLE.MeV')


from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["PYTHIA 4789899 989240512","PYTHIA_INIT 820021 2347532"]

from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()
from PythiaRhad_i.PythiaRhad_iConf import PythiaRhad
job +=PythiaRhad()
job.PythiaRhad.PythiaCommand = ["pysubs msel 13","pysubs ckin 3 18.","pypars mstp 43 2"]
job.PythiaRhad.PythiaCommand += ["pypars mstp 51 19070", "pypars mstp 52 2",
                             "pypars mstp 53 19070", "pypars mstp 54 2",
                             "pypars mstp 55 19070", "pypars mstp 56 2",
                             "pypars mstp 81 1", "pydat1 mstj 11 4"]

# R-hadron commands:
if (CASE=='gluino'):
    job.PythiaRhad.RunGluinoHadrons=True
    job.PythiaRhad.RunStopHadrons=False
elif (CASE=='stop'):
    job.PythiaRhad.RunGluinoHadrons=False
    job.PythiaRhad.RunStopHadrons=True


if job.PythiaRhad.RunGluinoHadrons: # Gluino hadron setup
    job.PythiaRhad.PythiaCommand += ["pymssm imss 1 1", "pymssm imss 3 1", "pymssm imss 5 1",
                                 "pymssm rmss 3 "+str(MASS)+".0", "pymssm rmss 1 4000.0",
                                 "pymssm rmss 2 4000.0", "pymssm rmss 4 10000.0",
                                 "pymssm rmss 7 4000.0", "pymssm rmss 8 4000.0",
                                 "pymssm rmss 9 4000.0", "pymssm rmss 10 4000.0",
                                 "pymssm rmss 11 4000.0", "pymssm rmss 12 4000.0",
                                 "pysubs msel 0", "pysubs msub 244 1", "pypars mstp 111 0",
                                 "pyinit pylistf 3", "pyinit dumpr 0 100", "pystat 2"
                             ]
elif job.PythiaRhad.RunStopHadrons: # Stop hadron setup
    job.PythiaRhad.PythiaCommand += ["pymssm imss 1 1", "pymssm imss 3 1", "pymssm imss 5 1",
                                 "pymssm rmss 1 4000", "pymssm rmss 2 4000",
                                 "pymssm rmss 3 4000", "pymssm rmss 4 4000",
                                 "pymssm rmss 7 4000.0", "pymssm rmss 8 4000.0",
                                 "pymssm rmss 9 4000.0", "pymssm rmss 10 4000.0",
                                 "pymssm rmss 11 4000.0", "pymssm rmss 12 "+str(MASS)+".0",
                                 "pysubs msel 0", "pysubs msub 261 1",
                                 "pysubs msub 264 1", "pypars mstp 111 0",
                                 "pyinit pylistf 3", "pyinit dumpr 0 20"
                                 ]


# the following five lines are added for use of POOL persistency:
include( 'AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py' )     
include( 'GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py' )
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "StreamEVGEN" )
Stream1.ItemList += [ 'EventInfo#*', 'McEventCollection#*' ]            
include( 'AthenaSealSvc/AthenaSealSvc_joboptions.py' )                 
AthenaSealSvc.CheckDictionary = TRUE
Stream1.OutputFile = "pythia_"+CASE+"_"+str(MASS)+"GeV_"+str(MyNevts)+"_events.pool.root"

