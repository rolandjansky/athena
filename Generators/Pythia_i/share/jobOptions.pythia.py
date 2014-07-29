###############################################################
#
# JOB OPTIONS FILE. THIS FILE IS TO DEMONSTRATE HOW THE USER CAN
# CHANGE THE PYTHIA PARAMETERS. IT DOES NOT NECESARILY COMPLY TO
# THE ATLAS DEFAULTS FOR PHYSICS EVENT GENERATION.
#==============================================================
#--------------------------------------------------------------
# Private Application Configuration option
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel = INFO

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 5
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["PYTHIA 4789899 989240512",
"PYTHIA_INIT 820021 2347532"]
# ServiceManager.AtRndmGenSvc.ReadFromFile = true;

from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()
from Pythia_i.Pythia_iConf import Pythia
job +=Pythia()
job.Pythia.PythiaCommand = ["pysubs msel 13","pysubs ckin 3 18.","pypars mstp 43 2"]
job.Pythia.PythiaCommand += ["pypars mstp 51 19070", "pypars mstp 52 2",
                        "pypars mstp 53 19070", "pypars mstp 54 2",
                        "pypars mstp 55 19070", "pypars mstp 56 2"]

from TruthExamples.TruthExamplesConf import DumpMC
job += DumpMC()
from TruthExamples.TruthExamplesConf import TestHepMC
job += TestHepMC()
job.TestHepMC.CmEnergy=10000000
from TruthExamples.TruthExamplesConf import PrintMC
job += PrintMC()



#
# End of job options file
#
###############################################################
