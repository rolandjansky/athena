###############################################################
#
# JOB OPTIONS FILE. THIS FILE IS TO DEMONSTRATE HOW THE USER CAN
# CHANGE THE PYTHIA PARAMETERS. IT DOES NOT NECESARILY COMPLY TO
# THE ATLAS DEFAULTS FOR PHYSICS EVENT GENERATION.
#==============================================================
#--------------------------------------------------------------
# Private Application Configuration option
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel = DEBUG

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
####################
job.Pythia.PythiaCommand = ["pyinit pylisti 12"] #print out particle properties settings
job.Pythia.PythiaCommand += ["pyinit pylistf 1"] 
job.Pythia.PythiaCommand += ["pysubs msel 0"]  # !
job.Pythia.PythiaCommand += ["pysubs msub 271 1"]  # squark pair production 
job.Pythia.PythiaCommand += ["pysubs msub 272 1"]  # squark pair production 
job.Pythia.PythiaCommand += ["pysubs msub 273 1"]  # squark pair production 
job.Pythia.PythiaCommand += ["pysubs msub 274 1"]  # squark pair production 
job.Pythia.PythiaCommand += ["pysubs msub 275 1"]  # squark pair production 
job.Pythia.PythiaCommand += ["pysubs msub 276 1"]  # squark pair production 
job.Pythia.PythiaCommand += ["pysubs msub 277 1"]  # squark pair production 
job.Pythia.PythiaCommand += ["pysubs msub 278 1"]  # squark pair production 
job.Pythia.PythiaCommand += ["pysubs msub 279 1"]  # squark pair production 
job.Pythia.PythiaCommand += ["pysubs msub 280 1"]  # squark pair production 
#######
job.Pythia.PythiaCommand+= ["pymssm imss 1 1"] # general MSSM !!
#Pythia.PythiaCommand+= ["pymssm imss 52 1"] #switch on RPV lambda^prime coupling
#Pythia.PythiaCommand += ["pymssm rmss 52 8."] # setting lambda^prime coupling strength to 10^{-rmss(52)}
job.Pythia.PythiaCommand+= ["pymssm imss 52 3"] #switch on RPV lambda^prime coupling
job.Pythia.PythiaCommand += ["pymsrv rvlam 111 0.000002"] # setting lambda^prime coupling strength 
#job.Pythia.PythiaCommand += ["mdcy xxx 1"]
job.Pythia.PythiaCommand += ["pydat1 mstj 22 3"] # allow long decay length
job.Pythia.PythiaCommand += ["pydat1 parj 72 10000."]
## only works for >=0.000009  , ct<=1cm
## with current masses: 0.000002 -> ct=70mm
job.Pythia.PythiaCommand += ["pydat3 mdme 2205 1 0"] # switch explicetly off special RPV decays  
job.Pythia.PythiaCommand += ["pydat3 mdme 2206 1 0"] # switch explicetly off special RPV decays  
##
job.Pythia.PythiaCommand += ["pymssm rmss 1 500."] #M1 gaugino 
job.Pythia.PythiaCommand += ["pymssm rmss 2 5000."] #M2 gaugino 
job.Pythia.PythiaCommand += ["pymssm rmss 3 5000."] #M3 gluino
job.Pythia.PythiaCommand += ["pymssm rmss 4 800."] #mu higgsino, default=800  ,important to make chi10 much lighter than other gauginos
job.Pythia.PythiaCommand += ["pymssm rmss 5 2."]  #tan beta , default=2  ,important to make chi10 much lighter than other gauginos
job.Pythia.PythiaCommand += ["pymssm rmss 6 5000."] #left slepton, makes sneutrino mass high (also eL), to surpress LQD decay to neutrinos
job.Pythia.PythiaCommand += ["pymssm rmss 7 5000."] #right slepton, irrelevant in LQD 
job.Pythia.PythiaCommand += ["pymssm rmss 8 700."] #left squark
job.Pythia.PythiaCommand += ["pymssm rmss 9 700."] #right squark
job.Pythia.PythiaCommand += ["pymssm rmss 10 5000."] #third generation 
job.Pythia.PythiaCommand += ["pymssm rmss 11 5000."] #third generation
job.Pythia.PythiaCommand += ["pymssm rmss 12 5000."] #third generation
job.Pythia.PythiaCommand += ["pymssm rmss 13 5000."] #third generation
job.Pythia.PythiaCommand += ["pymssm rmss 14 5000."] #third generation
##############

from TruthExamples.TruthExamplesConf import DumpMC
job += DumpMC()

#
# End of job options file
#
###############################################################
