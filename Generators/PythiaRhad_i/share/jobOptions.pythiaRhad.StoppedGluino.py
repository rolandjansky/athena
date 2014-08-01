from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel = INFO

#in Simulation/G4Extensions/RHadrons/share/
MASS=1000 #100,300,600,1000,1300,1600,2000
CASE='gluino'
DECAY=True
TIMESHIFT=0 #ns

# Number of events to be processed (default is 10)
theApp.EvtMax = 10

import os
import sys
import random
#from datetime import datetime

os.system('get_files -data PDGTABLE_'+CASE+'_'+str(MASS)+'GeV.MeV')
os.system('mv PDGTABLE_'+CASE+'_'+str(MASS)+'GeV.MeV PDGTABLE.MeV')

random.seed()
R1=random.randint(0,100000000)
R2=random.randint(0,100000000)
R3=random.randint(0,100000000)
R4=random.randint(0,100000000)
PYTHR = "PYTHIA "+str(R1)+" "+str(R2)
PYTHRI = "PYTHIA_INIT "+str(R3)+" "+str(R4)

from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
#ServiceMgr.AtRndmGenSvc.Seeds = ["PYTHIA 4789899 989240512","PYTHIA_INIT 820021 2347532"]
ServiceMgr.AtRndmGenSvc.Seeds = [ PYTHR, PYTHRI ]
print ServiceMgr.AtRndmGenSvc.Seeds

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
if (CASE=='gluino' and DECAY==False):
    job.PythiaRhad.RunGluinoHadrons=True
    job.PythiaRhad.RunStopHadrons=False
    job.PythiaRhad.PythiaCommand += ["pymssm imss 1 1", "pymssm imss 3 1", "pymssm imss 5 1",
                                 "pymssm rmss 3 "+str(MASS)+".0", "pymssm rmss 1 300.0", #gluino and neutralino masses
                                 "pymssm rmss 2 3000.0", "pymssm rmss 4 10000.0",
                                 "pymssm rmss 7 4000.0", "pymssm rmss 8 4000.0",
                                 "pymssm rmss 9 4000.0", "pymssm rmss 10 4000.0",
                                 "pymssm rmss 11 4000.0", "pymssm rmss 12 5000.0",
                                 "pysubs msel 0", "pysubs msub 243 1", "pysubs msub 244 1", "pypars mstp 111 0",
                                 "pyinit pylistf 3", "pyinit dumpr 0 100" ,"pystat 2"
                             ]
    job.PythiaRhad.strip_out_rh=True
elif (CASE=='gluino' and DECAY==True):
    job.PythiaRhad.RunGluinoHadrons=True
    job.PythiaRhad.RunStopHadrons=False
    job.PythiaRhad.xshift=0 #mm
    job.PythiaRhad.yshift=2500 #mm
    job.PythiaRhad.zshift=1000 #mm
    job.PythiaRhad.tshift=TIMESHIFT #ns
    job.PythiaRhad.randomtshift=50 # +-X ns, overrides tshift if non-zero
    job.PythiaRhad.rh_decay=True
    job.PythiaRhad.strip_out_rh=True
    job.PythiaRhad.boost_rh_to_rest_frame=True
    job.PythiaRhad.rotate_rh=True
    job.PythiaRhad.translate_rh_to_stopping_position=True
    job.PythiaRhad.StoppingInput = [ [ 0,0,0,0,0,0 ] ]
    include("StoppingInput.py")
    theApp.EvtMax = len(job.PythiaRhad.StoppingInput)-1
    job.PythiaRhad.PythiaCommand += ["pymssm imss 1 1", "pymssm imss 3 1", "pymssm imss 5 1",
                                 "pymssm rmss 3 "+str(MASS)+".0", "pymssm rmss 1 300.0", #gluino and neutralino masses
                                 "pymssm rmss 2 3000.0", "pymssm rmss 4 10000.0",
                                 "pymssm rmss 7 4000.0", "pymssm rmss 8 4000.0",
                                 "pymssm rmss 9 4000.0", "pymssm rmss 10 4000.0",
                                 "pymssm rmss 11 4000.0", "pymssm rmss 12 5000.0",
#                                decays into gluons
                                 "pymssm rmss 21 300.0e9", "pymssm imss 11 1", #ok to use the Gravitino
                                 "pymssm rmss 29 2.0e6", #planck mass, controls BR(gluino->g+Gravitino)
                                 "pydat3 mdcy 1000022 1 0", #kill neutralino decays
#                                ------------------
                                 "pysubs msel 0", "pysubs msub 243 1", "pysubs msub 244 1", "pypars mstp 111 0",
                                 "pyinit pylisti 12", #dumps the full decay table, etc.
                                 "pyinit dumpr 0 100" #,"pystat 2"
                             ]

#print out MC truth
#from TruthExamples.TruthExamplesConf import PrintMC
#PrintMC.PrintStyle = "Vertex"
#job += PrintMC()

# the following five lines are added for use of POOL persistency:
include( 'AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py' )     
include( 'GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py' )
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "StreamEVGEN" )
Stream1.ItemList += [ 'EventInfo#*', 'McEventCollection#*' ]            
include( 'AthenaSealSvc/AthenaSealSvc_joboptions.py' )                 
AthenaSealSvc.CheckDictionary = TRUE
Stream1.OutputFile = "pythia_"+CASE+"_"+str(MASS)+"GeV.pool.root"
